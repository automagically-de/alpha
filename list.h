#ifndef _LIST_H
#define _LIST_H

#include <stdlib.h>

typedef struct {
  void *data;
  void *next;
  size_t datasize;
} ListItem;

typedef struct {
  ListItem *first;
  ListItem *active;
  unsigned int n_items;
} List;

#define list_getfirst(list, type) (type *)_list_getfirst(list)
#define list_getnext(list, type) (type *)_list_getnext(list)
#define list_prepend(list, type) (type *)_list_prepend((list), sizeof(type))
#define list_append(list, type) (type *)_list_append((list), sizeof(type))

List *list_create(void);
int list_rewind(List *list);
int list_deletebyindex(List *list, unsigned int index);
int list_deletebydata(List *list, void *data);

void *_list_getfirst(List *list);
void *_list_getnext(List *list);
void *_list_prepend(List *list, size_t size);
void *_list_append(List *list, size_t size);

#endif /* _LIST_H */
