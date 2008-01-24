#ifndef _MESSAGE_H
#define _MESSAGE_H

int message_add(const char *text, Uint32 flags);
int message_draw(void);

/*****************************************************************************/

typedef struct {
  char *message;
  Uint32 flags;
  void *next;
} MessageEntry;

#define MESSAGE_DEBUG        (1L << 0)
#define MESSAGE_WARNING      (1L << 1)
#define MESSAGE_ERROR        (1L << 2)

#endif
