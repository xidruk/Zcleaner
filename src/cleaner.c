#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "../include/cleaner.h"
#include "../include/utils.h"
#include "../include/gdefs.h"

// =? Function to remove files and subdirectories in a directory !
static void clean_directory(const char* _path__)
{
    DIR* _dir__ = opendir(_path__);
    if (!_dir__)
    {
        char _Mcg__[RPATH_MAZ_S + 50]; 
        snprintf(_Mcg__, RPATH_MAZ_S + 50, "Could not open directory: %s", _path__);
        print_message("warning", _Mcg__);
        return;
    }
    
    struct dirent* _n_Try__; 
    char* _fl_pth__ = malloc(RPATH_MAZ_S * sizeof(char)); 
    if (!_fl_pth__)
    {
        closedir(_dir__);
        return;
    }
    
    while ((_n_Try__ = readdir(_dir__)) != NULL)
    {
        // =? Skip "." and ".." directories !
        if (strcmp(_n_Try__->d_name, ".") == 0 || strcmp(_n_Try__->d_name, "..") == 0)
        {
            continue;
        }
        
        snprintf(_fl_pth__, RPATH_MAZ_S, "%s/%s", _path__, _n_Try__->d_name);
        
        struct stat _st__;
        if (stat(_fl_pth__, &_st__) == -1)
        {
            continue;
        }
        
        if (S_ISDIR(_st__.st_mode))
        {
            // =? Recursively clean subdirectory !
            clean_directory(_fl_pth__);
            // =? Remove empty directory !
            if (rmdir(_fl_pth__) != 0)
            {
                char _Mcg__[RPATH_MAZ_S + 50];
                snprintf(_Mcg__, RPATH_MAZ_S + 50, "Could not remove directory: %s", _fl_pth__);
                print_message("warning", _Mcg__);
            }
        } else
        {
            // =? Remove file !
            if (unlink(_fl_pth__) != 0)
            {
                char _Mcg__[RPATH_MAZ_S + 50];
                snprintf(_Mcg__, RPATH_MAZ_S + 50, "Could not remove file: %s", _fl_pth__);
                print_message("warning", _Mcg__);
            }
        }
    }
    
    free(_fl_pth__);
    closedir(_dir__);
}


SessionStats clean_directories(DirectoryConfig* _confi_G__)
{
    SessionStats _ses_Stt__ = {0}; 
    char* _h_Dir__ = get_home_directory(); 
    
    // =? Get total home directory size !
    _ses_Stt__.total_home_size = get_total_home_size();
    
    // =? Get filesystem stats before cleaning !
    if (_h_Dir__)
    {
        get_filesystem_stats(_h_Dir__, &_ses_Stt__.fs_total_size, 
                            &_ses_Stt__.fs_used_before, &_ses_Stt__.fs_avail_before);
    }
    
    // =? Calculate total size before cleaning !
    printf("\nCalculating sizes before cleaning...\n");
    for (int _i__ = 0; _i__ < _confi_G__->count; _i__++)
    {
        unsigned long long _dir_S__ = get_directory_size(_confi_G__->directories[_i__]);
        _ses_Stt__.before_size += _dir_S__;
        
        // =? Add to home size if it's in the home directory !
        if (_h_Dir__ && strncmp(_confi_G__->directories[_i__], _h_Dir__, strlen(_h_Dir__)) == 0)
        {
            _ses_Stt__.home_before_size += _dir_S__;
        }
        
        char _sz_Str__[32];
        format_size(_dir_S__, _sz_Str__, sizeof(_sz_Str__));
        printf("  %s: %s\n", _confi_G__->directories[_i__], _sz_Str__);
    }
    
    // =? Clean each directory !
    printf("\nCleaning directories...\n");
    for (int _i__ = 0; _i__ < _confi_G__->count; _i__++)
    {
        printf("  Cleaning: %s\n", _confi_G__->directories[_i__]);
        clean_directory(_confi_G__->directories[_i__]);
    }
    
    // =? Calculate total size after cleaning !
    printf("\nCalculating sizes after cleaning...\n");
    for (int _i__ = 0; _i__ < _confi_G__->count; _i__++)
    {
        unsigned long long _dir_S__ = get_directory_size(_confi_G__->directories[_i__]);
        _ses_Stt__.after_size += _dir_S__;
        
        // =? Add to home size if it's in the home directory !
        if (_h_Dir__ && strncmp(_confi_G__->directories[_i__], _h_Dir__, strlen(_h_Dir__)) == 0)
        {
            _ses_Stt__.home_after_size += _dir_S__;
        }
        
        char _sz_Str__[32];
        format_size(_dir_S__, _sz_Str__, sizeof(_sz_Str__));
        printf("  %s: %s\n", _confi_G__->directories[_i__], _sz_Str__);
    }
    
    // =? Get filesystem stats after cleaningm !
    if (_h_Dir__)
    {
        get_filesystem_stats(_h_Dir__, &_ses_Stt__.fs_total_size, 
                            &_ses_Stt__.fs_used_after, &_ses_Stt__.fs_avail_after);
        free(_h_Dir__);
    }

    return _ses_Stt__;
}

void display_stats(SessionStats _ses_Stt__)
{
    char _bf_Buf__[32];
    char _af_Buf__[32];
    char _fr_Buf__[32];
    char _h_bf_Buf__[32];
    char _h_af_Buf__[32];
    char _h_fr_Buf__[32];
    
    // =? New buffers !
    char _tot_H_Buf__[32];
    char _fs_Tot_Buf__[32];
    char _fs_UsB_Buf__[32];
    char _fs_UsA_Buf__[32];
    char _fs_AvB_Buf__[32];
    char _fs_AvA_Buf__[32];
    
    // =? Format existing stats !
    format_size(_ses_Stt__.before_size, _bf_Buf__, sizeof(_bf_Buf__));
    format_size(_ses_Stt__.after_size, _af_Buf__, sizeof(_af_Buf__));
    format_size(_ses_Stt__.before_size - _ses_Stt__.after_size, _fr_Buf__, sizeof(_fr_Buf__));
    
    format_size(_ses_Stt__.home_before_size, _h_bf_Buf__, sizeof(_h_bf_Buf__));
    format_size(_ses_Stt__.home_after_size, _h_af_Buf__, sizeof(_h_af_Buf__));
    format_size(_ses_Stt__.home_before_size - _ses_Stt__.home_after_size, _h_fr_Buf__, sizeof(_h_fr_Buf__));
    
    // =? Format new stats !
    format_size(_ses_Stt__.total_home_size, _tot_H_Buf__, sizeof(_tot_H_Buf__));
    format_size(_ses_Stt__.fs_total_size, _fs_Tot_Buf__, sizeof(_fs_Tot_Buf__));
    format_size(_ses_Stt__.fs_used_before, _fs_UsB_Buf__, sizeof(_fs_UsB_Buf__));
    format_size(_ses_Stt__.fs_used_after, _fs_UsA_Buf__, sizeof(_fs_UsA_Buf__));
    format_size(_ses_Stt__.fs_avail_before, _fs_AvB_Buf__, sizeof(_fs_AvB_Buf__));
    format_size(_ses_Stt__.fs_avail_after, _fs_AvA_Buf__, sizeof(_fs_AvA_Buf__));
    
    // =? Calculate percentages !
    float _h_Pct__ = (_ses_Stt__.total_home_size > 0) ? 
                    ((float)(_ses_Stt__.home_before_size - _ses_Stt__.home_after_size) / _ses_Stt__.total_home_size) * 100 : 0;
    
    float _fs_Pct_B__ = (_ses_Stt__.fs_total_size > 0) ? 
                       ((float)_ses_Stt__.fs_used_before / _ses_Stt__.fs_total_size) * 100 : 0;
    
    float _fs_Pct_A__ = (_ses_Stt__.fs_total_size > 0) ? 
                      ((float)_ses_Stt__.fs_used_after / _ses_Stt__.fs_total_size) * 100 : 0;
    
    printf("\n╔═════════════════════════════════════════════════════════════╗\n");
    printf("║                     CLEANING RESULTS                        ║\n");
    printf("╠═════════════════════════════════════════════════════════════╣\n");
    printf("║ Total Before:  %-27s                ║\n", _bf_Buf__);
    printf("║ Total After:   %-27s                ║\n", _af_Buf__);
    printf("║ Total Freed:   %-27s                ║\n", _fr_Buf__);
    printf("╠═════════════════════════════════════════════════════════════╣\n");
    printf("║ Home Before:   %-27s                ║\n", _h_bf_Buf__);
    printf("║ Home After:    %-27s                ║\n", _h_af_Buf__);
    printf("║ Home Freed:    %-27s                ║\n", _h_fr_Buf__);
    printf("╠═════════════════════════════════════════════════════════════╣\n");
    printf("║ Total Home Size: %-27s              ║\n", _tot_H_Buf__);
    printf("║ Cleaned Home %%:  %.2f%%                                    ║\n", _h_Pct__);
    printf("╠═════════════════════════════════════════════════════════════╣\n");
    printf("║ Filesystem Size: %-27s              ║\n", _fs_Tot_Buf__);
    printf("║ Used Before:     %-27s (%.1f%%)           ║\n", _fs_UsB_Buf__, _fs_Pct_B__);
    printf("║ Used After:      %-27s (%.1f%%)           ║\n", _fs_UsA_Buf__, _fs_Pct_A__);
    printf("║ Available Before: %-27s              ║\n", _fs_AvB_Buf__);
    printf("║ Available After:  %-27s              ║\n", _fs_AvA_Buf__);
    printf("╚═════════════════════════════════════════════════════════════╝\n");
}
