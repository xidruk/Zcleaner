#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <string.h>
#include <limits.h>
#include <pwd.h>
#include <unistd.h>
#include "../include/utils.h"
#include "../include/gdefs.h"

char* get_home_directory(void)
{
    // =? Try to get home directory from environment variable !
    const char* _h_Env__ = getenv("HOME");
    if (_h_Env__)
    {
        return strdup(_h_Env__);
    }
    
    // =? If environment variable is not available, try from passwd file !
    struct passwd* _pw__ = getpwuid(getuid());
    if (_pw__ && _pw__->pw_dir)
    {
        return strdup(_pw__->pw_dir);
    }

    return NULL;
}

char* expand_path(const char* _pth__)
{
    if (!_pth__)
    {
        return NULL;
    }
    
    // =? Handle ~ at the beginning of the path !
    if (_pth__[0] == '~')
    {
        char* _h_Dir__ = get_home_directory();
        if (!_h_Dir__)
        {
            return strdup(_pth__); // =? Return original path if home not found !
        }
        
        size_t _h_Len__ = strlen(_h_Dir__);
        size_t _p_Len__ = strlen(_pth__);
        
        // =? Allocate memory for expanded path !
        char* _xpd__ = malloc((_h_Len__ + _p_Len__) * sizeof(char));
        if (!_xpd__)
        {
            free(_h_Dir__);
            return NULL;
        }
        
        // =? Copy home directory !
        strcpy(_xpd__, _h_Dir__);
        
        // =? Append the rest of the path (skipping ~) !
        if (_p_Len__ > 1)
        {
            strcat(_xpd__, _pth__ + 1);
        }
        
        free(_h_Dir__);
        return _xpd__;
    } else
    {
        // =? No expansion needed, just copy !
        return strdup(_pth__);
    }
}

unsigned long long get_directory_size(const char* _pth__)
{
    unsigned long long _tot_Sz__ = 0;
    DIR* _dir__ = opendir(_pth__);
    
    if (!_dir__)
    {
        return 0;
    }
    
    struct dirent* _n_Try__;
    char* _fl_pth__ = malloc(RPATH_MAZ_S * sizeof(char));
    
    if (!_fl_pth__)
    {
        closedir(_dir__);
        return 0;
    }
    
    while ((_n_Try__ = readdir(_dir__)) != NULL)
    {
        // =? Skip "." and ".." directories !
        if (strcmp(_n_Try__->d_name, ".") == 0 || strcmp(_n_Try__->d_name, "..") == 0)
        {
            continue;
        }
        
        snprintf(_fl_pth__, RPATH_MAZ_S, "%s/%s", _pth__, _n_Try__->d_name);
        
        struct stat _st__;
        if (stat(_fl_pth__, &_st__) == -1)
        {
            continue;
        }
        
        if (S_ISDIR(_st__.st_mode))
        {
            // =? Add size of subdirectory !
            _tot_Sz__ += get_directory_size(_fl_pth__);
        } else
        {
            // =? Add file size !
            _tot_Sz__ += _st__.st_size;
        }
    }

    free(_fl_pth__);
    closedir(_dir__);
    return _tot_Sz__;
}

unsigned long long get_total_home_size()
{
    char* _h_Dir__ = get_home_directory();
    if (!_h_Dir__)
    {
        return 0;
    }

    unsigned long long _size__ = get_directory_size(_h_Dir__);
    free(_h_Dir__);
    return _size__;
}

void get_filesystem_stats(const char* _pth__, unsigned long long* _tot_Sz__, unsigned long long* _usd_Sz__, unsigned long long* _av_Sz__)
{
    struct statvfs _stat__;
    
    if (statvfs(_pth__, &_stat__) != 0)
    {
        *_tot_Sz__ = 0;
        *_usd_Sz__ = 0;
        *_av_Sz__ = 0;
        return;
    }
    
    *_tot_Sz__ = (unsigned long long)_stat__.f_blocks * _stat__.f_frsize;
    *_av_Sz__ = (unsigned long long)_stat__.f_bavail * _stat__.f_frsize;
    *_usd_Sz__ = *_tot_Sz__ - (unsigned long long)_stat__.f_bfree * _stat__.f_frsize;
}

void format_size(unsigned long long _size__, char* _buf__, size_t _buf_Sz__)
{
    const char* _units__[] = {"B", "KB", "MB", "GB", "TB"};
    int _u_Idx__ = 0;
    double _sz_D__ = (double)_size__;
    
    while (_sz_D__ >= 1024.0 && _u_Idx__ < 4)
    {
        _sz_D__ /= 1024.0;
        _u_Idx__++;
    }
    
    snprintf(_buf__, _buf_Sz__, "%.2f %s", _sz_D__, _units__[_u_Idx__]);
}

int file_exists(const char* _pth__)
{
    FILE* _file__ = fopen(_pth__, "r");
    if (_file__)
    {
        fclose(_file__);
        return 1;
    }
    return 0;
}

int directory_exists(const char* _pth__)
{
    DIR* _dir__ = opendir(_pth__);
    if (_dir__)
    {
        closedir(_dir__);
        return 1;
    }
    return 0;
}

void print_message(const char* _type__, const char* _msg__)
{
    if (strcmp(_type__, "error") == 0)
    {
        printf("❌ ERROR: %s\n", _msg__);
    } else if (strcmp(_type__, "warning") == 0)
    {
        printf("⚠️  WARNING: %s\n", _msg__);
    } else if (strcmp(_type__, "success") == 0)
    {
        printf("✅ SUCCESS: %s\n", _msg__);
    } else if (strcmp(_type__, "info") == 0)
    {
        printf("ℹ️  INFO: %s\n", _msg__);
    } else {
        printf("%s: %s\n", _type__, _msg__);
    }
}

void print_logo(void)
{
    printf("\n");
    printf("███████╗░█████╗░██╗░░░░░███████╗░█████╗░███╗░░██╗███████╗██████╗░\n");
    printf("╚════██║██╔══██╗██║░░░░░██╔════╝██╔══██╗████╗░██║██╔════╝██╔══██╗\n");
    printf("░░███╔═╝██║░░╚═╝██║░░░░░█████╗░░███████║██╔██╗██║█████╗░░██████╔╝\n");
    printf("██╔══╝░░██║░░██╗██║░░░░░██╔══╝░░██╔══██║██║╚████║██╔══╝░░██╔══██╗\n");
    printf("███████╗╚█████╔╝███████╗███████╗██║░░██║██║░╚███║███████╗██║░░██║\n");
    printf("╚══════╝░╚════╝░╚══════╝╚══════╝╚═╝░░╚═╝╚═╝░░╚══╝╚══════╝╚═╝░░╚═╝\n");
    printf("\n");
}

void zbranding(void)
{
    printf("\n");
    printf("Developed by Xidruk , GitHub: [github.com/xidruk]        \n");
    printf("Source code: [github.com/xidruk/Zcleaner]              \n");
    printf("-------------------------------------------------------------------\n");
    printf("  🌟  Star this repository if you find it useful!                  \n");
    printf("  🔄  Contributions and feedback are welcome                       \n");
    printf("  🐛  Report issues on GitHub                                      \n");
    printf("\n");
}