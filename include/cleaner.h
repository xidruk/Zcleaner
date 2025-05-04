#ifndef CLEANER_H
#define CLEANER_H

#include "config.h"
#include "gdefs.h"


SessionStats clean_directories(DirectoryConfig* config);
void display_stats(SessionStats stats);

#endif