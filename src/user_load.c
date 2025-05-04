#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/utils.h"

char *get_home_path(void)
{
    char *_h_path__ = NULL;
    _h_path__ = getenv("HOME");
    if (!_h_path__)
        return (NULL);
    return (strdup(_h_path__));
}

int edit_conf(const char *_file_pth__)
{
    if (!_file_pth__)
        return (-1);
    
    char *_h_path__ = get_home_path();
    if (!_h_path__)
        return (-2);
    
    // =? Read original file !
    FILE *_orig_F__ = fopen(_file_pth__, "r");
    if (!_orig_F__)
    {
        free(_h_path__);
        return (-3);
    }
    
    // =? Create temporary file !
    char _tmp_Fpth__[1024];
    snprintf(_tmp_Fpth__, sizeof(_tmp_Fpth__), "%s.tmp", _file_pth__);
    FILE *_tmp_F__ = fopen(_tmp_Fpth__, "w");
    if (!_tmp_F__)
    {
        fclose(_orig_F__);
        free(_h_path__);
        return (-4);
    }
    
    char _line__[1024];
    char _usr_nme__[] = "/home/xidruk/";
    size_t _usr_Len__ = strlen(_usr_nme__);
    
    // =? Process each line !
    while (fgets(_line__, sizeof(_line__), _orig_F__))
    {
        // =? Check if line starts with the username path !
        if (strncmp(_line__, _usr_nme__, _usr_Len__) == 0)
        {
            // =? Create new line with user's home path !
            char _mod_Line__[1024];
            snprintf(_mod_Line__, sizeof(_mod_Line__), "%s/%s", _h_path__, _line__ + _usr_Len__);
            fputs(_mod_Line__, _tmp_F__);
        } else
        {
            // =? Keep original line !
            fputs(_line__, _tmp_F__);
        }
    }
    
    // =? Close files !
    fclose(_orig_F__);
    fclose(_tmp_F__);
    
    // =? Replace original with temp file !
    rename(_tmp_Fpth__, _file_pth__);
    free(_h_path__);
    return (0);
}