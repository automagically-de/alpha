#include <stdlib.h>

#include "list.h"
#include "misc.h"

List *list_create(void)
{
  List *list = misc_malloc(sizeof(List));
  if(list)
  {
    list->first = NULL;
    list->active = NULL;
    list->n_items = 0;
    return list;
  }
  else return NULL;
}

/*****************************************************************************/

int list_rewind(List *list)
{
  if(list == NULL) return EXIT_FAILURE;
  list->active = list->first;
  return EXIT_SUCCESS;
}

/*****************************************************************************/

void *_list_getfirst(List *list)
{
  list_rewind(list);
  return _list_getnext(list);
}

/*****************************************************************************/

void *_list_getnext(List *list)
{
  void *data;
  if(list == NULL) return NULL;
  if(list->active == NULL) return NULL;
  data = list->active->data;
  list->active = (ListItem*)list->active->next;
  return data;
}

/*****************************************************************************/

void *_list_prepend(List *list, size_t size)
{
  ListItem *new;

  if(list == NULL) return NULL;

  new = misc_malloc(sizeof(ListItem));
  if(new == NULL) return NULL;

  new->data = misc_malloc(size);
  if(new->data == NULL)
  {
    misc_free(new, sizeof(ListItem));
    return NULL;
  }

  if(list->first == list->active) list->active = new;
  new->next = list->first;
  list->first = new;

  return new->data;
}

/*****************************************************************************/

void *_list_append(List *list, size_t size)
{
  ListItem *new;

  if(list == NULL) return NULL;
  
  new = misc_malloc(sizeof(ListItem));
  if(new == NULL) return NULL;

  new->data = misc_malloc(size);
  if(new->data == NULL)
  {
    misc_free(new, sizeof(ListItem));
    return NULL;
  }

  new->next = NULL;
  new->datasize = size;
  list->n_items ++;

  if(list->first == NULL)
  {
    list->first = new;
    list->active = new;
  }
  else
  {
    ListItem *item = list->first;
    while(item->next != NULL) item = item->next;
    item->next = new;
  }
  return new->data;
}

/*****************************************************************************/

int list_deletebyindex(List *list, unsigned int index)
{
  ListItem *item, *prev;
  int i;

  if(list == NULL) return EXIT_FAILURE;
  if(index >= list->n_items) return EXIT_FAILURE;

  item = list->first;
  list->n_items --;

  if(index == 0)
  {
    /* remove first */
    list->first = (ListItem*)item->next;
    if(list->active == item) list->active = (ListItem*)item->next;
    misc_free(item->data, item->datasize);
    misc_free(item, sizeof(ListItem));
    return EXIT_SUCCESS;
  }
  for(i=1; i<index; i++) item = (ListItem*)item->next;
  prev = item;
  item = item->next;
  prev->next = item->next;
  if(list->active == item) list->active = list->first;
  misc_free(item->data, item->datasize);
  misc_free(item, sizeof(ListItem));
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int list_deletebydata(List *list, void *data)
{
  ListItem *item, *prev;

  if(list == NULL) return EXIT_FAILURE;

  item = prev = list->first;
  while((item != NULL) && (item->data != data))
  {
    prev = item;
    item = (ListItem*)item->next;
  }
  if(item == NULL) return EXIT_FAILURE;

  if(item == list->first) list->first = (ListItem*)item->next;
  else prev->next = item->next;

  if(list->active == item) list->active = list->first;

  misc_free(item->data, item->datasize);
  misc_free(item, sizeof(ListItem));
  list->n_items --;
  return EXIT_SUCCESS;
}

