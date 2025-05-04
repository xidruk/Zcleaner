#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <limits.h>
#include "gdefs.h"

DirectoryConfig* read_config(const char* config_path);
void free_config(DirectoryConfig* config);
int add_directory_to_config(const char* config_path, const char* directory);
int remove_directory_from_config(const char* config_path, const char* directory);
int is_dangerous_path(const char* path);

#endif 