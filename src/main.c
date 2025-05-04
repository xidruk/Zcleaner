#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "../include/config.h"
#include "../include/cleaner.h"
#include "../include/utils.h"

void print_usage()
{
    printf("Usage: zclean [OPTIONS]\n");
    printf("Options:\n");
    printf("  --add <directory>    Add directory to cleaning list\n");
    printf("  --drop <directory>   Remove directory from cleaning list\n");
    printf("  --help               Display this help message\n");
    printf("\nWithout options, zclean will clean all directories in config.\n");
}

int main(int argc, char *argv[])
{
    
    const char* _confi_Gpth__ = "conf/dirconf.dcon";
    
    print_logo();
    int _res__ = edit_conf(_confi_Gpth__);
    if (_res__ < 0)
    {
        fprintf(stderr, "Failed to update configuration file (error code: %d)\n", _res__);
    }
    // =? Process command line arguments !
    if (argc > 1)
    {
        if (strcmp(argv[1], "--help") == 0)
        {
            print_usage();
            return EXIT_SUCCESS;
        } else if (strcmp(argv[1], "--add") == 0)
        {
            if (argc < 3)
            {
                print_message("error", "Missing directory argument for --add");
                print_usage();
                return EXIT_FAILURE;
            }
            
            // =? Expand path (handle ~ and other shell expansions) !
            char* _xpd_pth__ = expand_path(argv[2]);
            if (!_xpd_pth__)
            {
                print_message("error", "Failed to expand path");
                return EXIT_FAILURE;
            }
            
            // =? Check if path is dangerous !
            if (is_dangerous_path(_xpd_pth__))
            {
                print_message("error", "Path is dangerous and cannot be added for safety reasons");
                free(_xpd_pth__);
                return EXIT_FAILURE;
            }
            
            if (!directory_exists(_xpd_pth__))
            {
                print_message("error", "Directory does not exist");
                free(_xpd_pth__);
                return EXIT_FAILURE;
            }
            
            int _res__ = add_directory_to_config(_confi_Gpth__, _xpd_pth__);
            free(_xpd_pth__);
            
            if (_res__)
            {
                print_message("success", "Directory added to cleaning list");
                return EXIT_SUCCESS;
            } else
            {
                print_message("error", "Failed to add directory to cleaning list");
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[1], "--drop") == 0)
        {
            if (argc < 3)
            {
                print_message("error", "Missing directory argument for --drop");
                print_usage();
                return EXIT_FAILURE;
            }
            
            // =? Expand path (handle ~ and other shell expansions) !
            char* _xpd_pth__ = expand_path(argv[2]);
            if (!_xpd_pth__)
            {
                print_message("error", "Failed to expand path");
                return EXIT_FAILURE;
            }
            
            int _res__ = remove_directory_from_config(_confi_Gpth__, _xpd_pth__);
            free(_xpd_pth__);
            
            if (_res__)
            {
                print_message("success", "Directory removed from cleaning list");
                return EXIT_SUCCESS;
            } else
            {
                print_message("error", "Failed to remove directory from cleaning list");
                return EXIT_FAILURE;
            }
        } else
        {
            print_message("error", "Unknown option");
            print_usage();
            return EXIT_FAILURE;
        }
    }
    
    // =? Check if config file exists !
    if (!file_exists(_confi_Gpth__))
    {
        print_message("error", "Configuration file not found");
        return EXIT_FAILURE;
    }
    
    // =? Read configuration !
    DirectoryConfig* _confi_G__ = read_config(_confi_Gpth__);
    if (!_confi_G__ || _confi_G__->count == 0)
    {
        print_message("error", "Failed to read configuration or no directories specified");
        free_config(_confi_G__);
        return EXIT_FAILURE;
    }
    
    print_message("info", "Zcleaner started...");
    
    // =? Clean directories and get stats !
    SessionStats _ses_Stt__ = clean_directories(_confi_G__);
    
    // =? Display stats !
    display_stats(_ses_Stt__);
    
    // =? Free memory !
    free_config(_confi_G__);
    
    print_message("success", "Zcleaner finished successfully!");
    zbranding();
    return EXIT_SUCCESS;
}