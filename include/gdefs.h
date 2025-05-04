#ifndef GDEFS_H
#define GDEFS_H

// =? Defs 

typedef struct {
    unsigned long long before_size;
    unsigned long long after_size;
    unsigned long long home_before_size;
    unsigned long long home_after_size;
    
    unsigned long long total_home_size;
    unsigned long long fs_total_size;
    unsigned long long fs_used_before;
    unsigned long long fs_avail_before;
    unsigned long long fs_used_after;
    unsigned long long fs_avail_after;
} SessionStats;

typedef struct {
    char **directories;
    int count;
} DirectoryConfig;

#ifndef RPATH_MAZ_S 
#define RPATH_MAZ_S 4096
#endif 

#ifndef INITIAL_CAPACITY
#define INITIAL_CAPACITY 10
#endif

#endif