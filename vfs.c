#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vfs.h"
#include "misc.h"

vfs_FILE *vfs_fopen(char *path, char *mode)
{
#ifdef HAVE_ZLIB
  gzFile gzfile;
  vfs_FILE *vfs_file;
  char filename[1024];
  
  strncpy(filename, path, 1024);
  strncat(filename, ".gz", 1024);
  gzfile = gzopen(filename, mode);
  if(gzfile == NULL) gzfile = gzopen(path, mode);
  if(gzfile == NULL)
  {
#ifdef DEBUG
    fprintf(stderr, "DEBUG: vfs_fopen: couldn't open neither '%s' nor '%s'.\n",
            filename, path);
#endif /* DEBUG */
    return NULL;
  }
  vfs_file = misc_malloc(sizeof(vfs_FILE));
  vfs_file->fd = gzfile;
  return vfs_file;
#else /* HAVE_ZLIB */
  return fopen(path, mode);
#endif /* HAVE_ZLIB */
}

/*****************************************************************************/

int vfs_fclose(vfs_FILE *stream)
{
#ifdef HAVE_ZLIB
  if(stream != NULL)
  {
    int retval = gzclose(stream->fd);
    misc_free(stream, sizeof(vfs_FILE));
    return retval;
  }
  else return EXIT_FAILURE;
#else /* HAVE_ZLIB */
  return fclose(stream);
#endif /* HAVE_ZLIB */
}

/*****************************************************************************/

int vfs_fgetc(vfs_FILE *stream)
{
#ifdef HAVE_ZLIB
  if(stream != NULL)
  {
    return gzgetc(stream->fd);
  }
  else return -1;
#else /* HAVE_ZLIB */
  return fgetc(stream);
#endif /* HAVE_ZLIB */
}

/*****************************************************************************/

char *vfs_fgets(char *s, int size, vfs_FILE *stream)
{
#ifdef HAVE_ZLIB
  if(stream != NULL)
  {
    return gzgets(stream->fd, s, size);
  }
  else return NULL;
#else /* HAVE_ZLIB */
  return fgets(s, size, stream);
#endif /* HAVE_ZLIB */
}

