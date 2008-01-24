#ifndef _PARSE_H
#define _PARSE_H

int parse_line(const char *line, char *varname, int lng_varname,
                                 char *value,   int lng_value);

#define E_PARSE_ERROR     -(1 << 0)
#define E_PARSE_UNTERM    -(1 << 1)
#define E_PARSE_NULLBUF   -(1 << 2)

#endif /* _PARSE_H */
