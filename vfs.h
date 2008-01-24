#ifndef _VFS_H
#define _VFS_H

#include <stdio.h>

#ifdef HAVE_ZLIB
#include <zlib.h>
typedef struct {
  gzFile fd;
} vfs_FILE;
#else
#define vfs_FILE FILE
#endif

vfs_FILE *vfs_fopen(char *path, char *mode);
int       vfs_fclose(vfs_FILE *stream);
int       vfs_fgetc(vfs_FILE *stream);
char     *vfs_fgets(char *s, int size, vfs_FILE *stream);

#endif
