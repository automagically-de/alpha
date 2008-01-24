#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "misc.h"
#include "parse.h"

#define WHITESPACES " \t\n\r"

int parse_line(const char *line, char *varname, int lng_varname,
                                 char *value,   int lng_value)
{
  int lvar = 0, lval = 0;
  char *bufp = (char *)line;

  if(line == NULL) return E_PARSE_NULLBUF;
  if((varname == NULL) || (lng_varname == 0)) return E_PARSE_NULLBUF;
  if((value == NULL) || (lng_value == 0)) return E_PARSE_NULLBUF;

  /* remove leading whitespaces */
  while(*bufp && strchr(WHITESPACES, *bufp)) bufp++;
  /* get var name */
  while(*bufp && ! strchr(WHITESPACES "=", *bufp))
  {
    if(*bufp == '#')
    {
      varname[lvar] = '\0';
      strncpy(value, "on", lng_value);
      return EXIT_SUCCESS;
    }
    if(lvar < lng_varname) varname[lvar] = *bufp;
    lvar++;
    bufp++;
  }
  varname[lvar] = '\0';
  while(*bufp && strchr(WHITESPACES, *bufp)) bufp++;
  if(*bufp && (*bufp == '='))
  {
    bufp++;
    while(*bufp && strchr(WHITESPACES, *bufp)) bufp++;
  }
  else
  {
    while(*bufp && strchr(WHITESPACES, *bufp)) bufp++;
    if(*bufp && (*bufp != '#')) return E_PARSE_ERROR;
    else
    {
      /* boolean var/value */
      strncpy(value, "on", lng_value);
      return EXIT_SUCCESS;
    }
  }
  /* normal value */
  if(! *bufp)
  {
    value[0] = '\0';
    return EXIT_SUCCESS;
  }
  if((*bufp == '"') || (*bufp == '\''))
  {
    char quotechar = *bufp;
    bufp++;
    /* quoted string */
    while(*bufp && (*bufp != quotechar))
    {
      char next;
      if(*bufp == '\\')
      {
        switch(*(++bufp))
        {
          case 'n': next = '\n'; break;
          case 't': next = '\t'; break;
          default: next = *bufp; break;
        }
      }
      else next = *bufp;
      if(lval < lng_value) value[lval] = next;
      lval++;
      bufp++;
    }
    value[lval] = '\0';
    if((!*bufp) || (*bufp != quotechar)) return E_PARSE_UNTERM;
  }
  else
  {
    /* unquoted string */
    while(*bufp)
    {
      char next;
 
      if(*bufp == '#')
      {
        while((lval > 0) && strchr(WHITESPACES, value[lval-1])) lval--;
        value[lval] = '\0';
        return EXIT_SUCCESS;
      }

      if(*bufp == '\\')
      {
        switch(*(++bufp))
        {
          case 'n': next = '\n'; break;
          case 't': next = '\t'; break;
          default: next = *bufp; break;
        }
      }
      else next = *bufp;
      if(lval < lng_value) value[lval] = next;
      lval++;
      bufp++;
    }
    while((lval > 0) && strchr(WHITESPACES, value[lval-1])) lval--;
    value[lval] = '\0';
  }
  return EXIT_SUCCESS;
}


