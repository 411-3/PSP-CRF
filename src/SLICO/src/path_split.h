#include <stdio.h>
#include <string.h>

void _splitpath(const char * path, char *drive, char *dir, char *fname, char *ext);
static void _split_whole_name(char *whole_name, char *fname, char *ext);

