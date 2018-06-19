#include <stdio.h>
#include <string.h>

void _splitpath(const char *path, char *drive, char *dir, char *fname, char *ext);
static void _split_whole_name(char *whole_name, char *fname, char *ext);

/* main test */
/*
int main(void)
{
	char *path = "/home/test/dir/f123.txt";
	// char *path = "/home/test/dir/123.txt";
	// char *path = "/home/test/dir/123";
	// char *path = "123";
	// char *path = "123.txt";

	char drive[128];
	char dir[128];
	char fname[128];
	char ext[128];

	_splitpath(path, drive, dir, fname, ext);
	printf("path  = %s\n", path);
	printf("dir   = %s\n", dir);
	printf("fname = %s\n", fname);
	printf("ext   = %s\n", ext);
	
	return 0;
}
*/

void _splitpath(const char *path, char *drive, char *dir, char *fname, char *ext)
{
	char * p_whole_name;

	char * path2 = new char[strlen(path) + 1];	
	strcpy(path2, path);	

	drive[0] = '\0';
	if (NULL == path2)
	{
		dir[0] = '\0';
		fname[0] = '\0';
		ext[0] = '\0';
		return;
	}

	if ('/' == path2[strlen(path2)])
	{
		strcpy(dir, path2);
		fname[0] = '\0';
		ext[0] = '\0';
		return;
	}

	p_whole_name = rindex(path2, '/');
	if (NULL != p_whole_name)
	{
		p_whole_name++;
		_split_whole_name(p_whole_name, fname, ext);

		snprintf(dir, p_whole_name - path2, "%s", path2);
	}
	else
	{
		_split_whole_name(path2, fname, ext);
		dir[0] = '\0';
	}
	delete [] path2;
}

static void _split_whole_name( char *whole_name, char *fname, char *ext)
{
	char *p_ext;

	p_ext = rindex(whole_name, '.');
	if (NULL != p_ext)
	{
		strcpy(ext, p_ext);
		snprintf(fname, p_ext - whole_name + 1, "%s", whole_name);
	}
	else
	{
		ext[0] = '\0';
		strcpy(fname, whole_name);
	}
}

