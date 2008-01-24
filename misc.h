#ifndef _MISC_H
#define _MISC_H

char *_misc_string_copym(const char *src, const char *file, int line);
void *_misc_malloc(size_t size, const char *file, int line);
void misc_free(void *ptr, size_t size);
void misc_string_free(char *string);

#define misc_string_copym(src) _misc_string_copym((src), __FILE__, __LINE__)
#define misc_malloc(size)      _misc_malloc((size), __FILE__, __LINE__)

#endif
