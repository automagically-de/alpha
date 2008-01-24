#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "conf.h"

char *_misc_string_copym(const char *src, const char *file, int line)
{
  char *dst;
  int size;

  if(src == NULL) return NULL;
  
  size = strlen(src) + 1;
  dst = malloc(size);
  if(dst == NULL)
  {
    fprintf(stderr, "ERROR: %s, %d: couldn't malloc memory for string copy\n",
            file, line);
    return NULL;
  }
  config.memory.used += size;

  strcpy(dst, src);
  return dst;
}

/*****************************************************************************/

void *_misc_malloc(size_t size, const char *file, int line)
{
  void *ptr;

  ptr = malloc(size);
  if(ptr == NULL)
  {
    fprintf(stderr, "ERROR: %s, %d: couldn't malloc enough memory\n",
            file, line);
    return NULL;
  }

  memset(ptr, 0, size);
  config.memory.used += size;

  return ptr;
}

/*****************************************************************************/

void misc_free(void *ptr, size_t size)
{
  if(ptr == NULL) return;
  free(ptr);
  config.memory.used -= size;
}

/*****************************************************************************/

void misc_string_free(char *string)
{
  Uint32 mem;
  if(string == NULL) return;
  
  mem = strlen(string) + 1;
  misc_free(string, mem);
}

