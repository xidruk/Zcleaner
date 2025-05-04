#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <limits.h>
#include "gdefs.h"

unsigned long long get_directory_size(const char* path);
unsigned long long get_total_home_size();
void get_filesystem_stats(const char* path, unsigned long long* total_size, 
                         unsigned long long* used_size, unsigned long long* avail_size);
void format_size(unsigned long long size, char* buffer, size_t buffer_size);
int file_exists(const char* path);
int directory_exists(const char* path);
void print_message(const char* type, const char* message);
char* expand_path(const char* path);
char* get_home_directory();
int edit_conf(const char *_file_path);
void print_logo(void);
void zbranding(void);

#endif
