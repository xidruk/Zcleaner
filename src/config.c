#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <pwd.h>
#include <unistd.h>
#include "../include/config.h"
#include "../include/utils.h"
#include "../include/gdefs.h"


// dangerous paths 
const char* _dng_Pths__[] = {
    "/",
    "/bin",
    "/boot",
    "/dev",
    "/etc",
    "/lib",
    "/lib64",
    "/media",
    "/mnt",
    "/opt",
    "/proc",
    "/root",
    "/run",
    "/sbin",
    "/srv",
    "/sys",
    "/usr",
    "/var",
    NULL
};

int is_dangerous_path(const char* _pth__)
{
    // =? Check against list of dangerous paths !
    for (int _i__ = 0; _dng_Pths__[_i__] != NULL; _i__++)
    { 
        if (strcmp(_pth__, _dng_Pths__[_i__]) == 0)
        {
            return 1;
        }
    }
    
    // =? Check for home root directory !
    char* _h_Dir__ = get_home_directory();
    if (_h_Dir__)
    {
        int _is_H__ = (strcmp(_pth__, _h_Dir__) == 0);
        free(_h_Dir__);
        if (_is_H__)
        {
            return 1;
        }
    }
    
    return 0;
}

DirectoryConfig* read_config(const char* _confi_Gpth__)
{
    FILE* _file__ = fopen(_confi_Gpth__, "r");
    if (!_file__)
    {
        return NULL;
    }

    DirectoryConfig* _confi_G__ = malloc(sizeof(DirectoryConfig));
    if (!_confi_G__)
    {
        fclose(_file__);
        return NULL;
    }
    
    // =? Initialize with default capacity ! // capacity aka _capacity__
    int _capacity__ = INITIAL_CAPACITY;
    _confi_G__->directories = malloc(_capacity__ * sizeof(char*));
    _confi_G__->count = 0;
    
    if (!_confi_G__->directories)
    {
        free(_confi_G__);
        fclose(_file__);
        return NULL;
    }

    char* _line__ = malloc(RPATH_MAZ_S * sizeof(char));
    if (!_line__)
    {
        free(_confi_G__->directories);
        free(_confi_G__);
        fclose(_file__);
        return NULL;
    }
    
    while (fgets(_line__, RPATH_MAZ_S, _file__))
    {
        // =? Remove newline character !
        size_t _len__ = strlen(_line__);
        if (_len__ > 0 && (_line__[_len__-1] == '\n' || _line__[_len__-1] == '\r'))
        {
            _line__[_len__-1] = '\0';
        }
        
        // =? Skip empty lines and comments !
        if (strlen(_line__) == 0 || _line__[0] == '#')
        {
            continue;
        }
        
        // =? Skip dangerous paths ! expanded_path aka _xpd_path__
        char* _xpd_path__ = expand_path(_line__);
        if (!_xpd_path__)
        {
            continue;
        }
        
        if (is_dangerous_path(_xpd_path__))
        {
            char _MCG__[RPATH_MAZ_S + 50];
            snprintf(_MCG__, RPATH_MAZ_S + 50, "Skipping dangerous path: %s", _xpd_path__);
            print_message("warning", _MCG__);
            free(_xpd_path__);
            continue;
        }
        
        // =? Resize array if needed !
        if (_confi_G__->count >= _capacity__)
        {
            _capacity__ *= 2;
            char** _tmp__ = realloc(_confi_G__->directories, _capacity__ * sizeof(char*));
            if (!_tmp__)
            {
                for (int _i__ = 0; _i__ < _confi_G__->count; _i__++)
                {
                    free(_confi_G__->directories[_i__]);
                }
                free(_confi_G__->directories);
                free(_confi_G__);
                free(_line__);
                free(_xpd_path__);
                fclose(_file__);
                return NULL;
            }
            _confi_G__->directories = _tmp__;
        }
        
        // =? Copy directory path (use expanded path) !
        _confi_G__->directories[_confi_G__->count] = _xpd_path__;
        
        _confi_G__->count++;
    }
    
    free(_line__);
    fclose(_file__);
    return _confi_G__;
}

void free_config(DirectoryConfig* _confi_G__)
{
    if (!_confi_G__)
    {
        return;
    }
    
    if (_confi_G__->directories)
    {
        for (int _i__ = 0; _i__ < _confi_G__->count; _i__++)
        {
            free(_confi_G__->directories[_i__]);
        }
        free(_confi_G__->directories);
    }
    
    free(_confi_G__);
}

int add_directory_to_config(const char* _confi_Gpth__, const char* _dir__)
{
    // =? Check if the directory is already in the config !
    DirectoryConfig* _confi_G__ = read_config(_confi_Gpth__);
    if (_confi_G__)
    {
        for (int _i__ = 0; _i__ < _confi_G__->count; _i__++)
        {
            if (strcmp(_confi_G__->directories[_i__], _dir__) == 0)
            {
                print_message("warning", "Directory already in cleaning list");
                free_config(_confi_G__);
                return 1; // =? Directory already exists, consider it success !
            }
        }
        free_config(_confi_G__);
    }
    
    // =? Append the directory to the config file !
    FILE* _file__ = fopen(_confi_Gpth__, "a");
    if (!_file__)
    {
        // =? Try to create the file if it doesn't exist !
        _file__ = fopen(_confi_Gpth__, "w");
        if (!_file__)
        {
            return 0;
        }
    }
    
    fprintf(_file__, "%s\n", _dir__);
    fclose(_file__);
    
    return 1;
}

int remove_directory_from_config(const char* _confi_Gpth__, const char* _dir__)
{
    // =? Read current config !
    DirectoryConfig* _confi_G__ = read_config(_confi_Gpth__);
    if (!_confi_G__)
    {
        return 0;
    }
    
    // =? Create a temporary file !
    char _tmp_Pth__[RPATH_MAZ_S];
    snprintf(_tmp_Pth__, RPATH_MAZ_S, "%s.tmp", _confi_Gpth__);
    
    FILE* _tmp_F__ = fopen(_tmp_Pth__, "w");
    if (!_tmp_F__)
    {
        free_config(_confi_G__);
        return 0;
    }
    
    int _found__ = 0;
    
    // =? Open original file to copy non-matching lines !
    FILE* _orig_F__ = fopen(_confi_Gpth__, "r");
    if (!_orig_F__)
    {
        fclose(_tmp_F__);
        remove(_tmp_Pth__);
        free_config(_confi_G__);
        return 0;
    }
    
    char* _line__ = malloc(RPATH_MAZ_S * sizeof(char));
    if (!_line__)
    {
        fclose(_orig_F__);
        fclose(_tmp_F__);
        remove(_tmp_Pth__);
        free_config(_confi_G__);
        return 0;
    }
    
    while (fgets(_line__, RPATH_MAZ_S, _orig_F__))
    {
        // =? Remove newline character for comparison !
        size_t _len__ = strlen(_line__);
        if (_len__ > 0 && (_line__[_len__-1] == '\n' || _line__[_len__-1] == '\r'))
        {
            _line__[_len__-1] = '\0';
        }
        
        // =? Skip empty lines and comments !
        if (strlen(_line__) == 0 || _line__[0] == '#')
        {
            fprintf(_tmp_F__, "%s\n", _line__);
            continue;
        }
        
        // =? Expand both paths for comparison !
        char* _xpd_L__ = expand_path(_line__);
        char* _xpd_D__ = expand_path(_dir__);
        
        int _match__ = 0;
        if (_xpd_L__ && _xpd_D__)
        {
            _match__ = (strcmp(_xpd_L__, _xpd_D__) == 0);
        }
        
        if (_xpd_L__) free(_xpd_L__);
        if (_xpd_D__) free(_xpd_D__);
        
        // =? Copy non-matching lines to temp file !
        if (!_match__)
        {
            fprintf(_tmp_F__, "%s\n", _line__);
        } else
        {
            _found__ = 1;
        }
    }
    
    free(_line__);
    fclose(_orig_F__);
    fclose(_tmp_F__);
    
    // =? Replace original file with temp file !
    if (remove(_confi_Gpth__) != 0)
    {
        remove(_tmp_Pth__);
        free_config(_confi_G__);
        return 0;
    }
    
    if (rename(_tmp_Pth__, _confi_Gpth__) != 0)
    {
        free_config(_confi_G__);
        return 0;
    }
    
    free_config(_confi_G__);
    
    if (!_found__)
    {
        print_message("warning", "Directory not found in cleaning list");
    }
    
    return 1;
}