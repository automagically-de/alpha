#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "SDL.h"

#include "conf.h"
#include "misc.h"

ConfStruct config;

typedef enum {
  CONF_BOOL,
  CONF_UINT,
  CONF_STRING,
  CONF_FUNC
} OptionType;

typedef enum {
  CONF_ARGNO,
  CONF_ARGSEP
} OptionArg;

typedef struct {
  const char *name;
  void *var;
  OptionType type;
  OptionArg arg;
  const char *set;
} OptionStruct;

/*****************************************************************************/

void conf_usage(void);
void conf_version(void);  
void conf_setdefaults(void);
void conf_parseargs(int *argcp, char ***argv, 
                    OptionStruct *options, Uint32 numoptions);
void conf_parsefile(const char *filename, OptionStruct *options, 
                    Uint32 numoptions);

/*****************************************************************************/

int conf_init(int *argcp, char ***argvp)
{
#ifdef HAVE_SDLMIXER
#define NumMixer 2
#else
#define NumMixer 0
#endif

#define OptionNumber (10 + NumMixer)
  static OptionStruct options[] = {
    {"fullscreen", &config.window.fullscreen, CONF_BOOL,   CONF_ARGNO,  "on"},
    {"resizable",  &config.window.resizable,  CONF_BOOL,   CONF_ARGNO,  "on"},
    {"invertmouse",&config.scrolling.invert,  CONF_BOOL,   CONF_ARGNO,  "on"},
    {"noalpha",    &config.performance.alpha, CONF_BOOL,   CONF_ARGNO, "off"},
#ifdef HAVE_SDLMIXER
    {"nosound",    &config.sound.enabled,     CONF_BOOL,   CONF_ARGNO, "off"},
    {"nomusic",    &config.sound.music,       CONF_BOOL,   CONF_ARGNO, "off"},
#endif
    {"progname",   &config.progname,          CONF_STRING, CONF_ARGSEP, NULL},
    {"map",        &config.map.name,          CONF_STRING, CONF_ARGSEP, NULL}, 
    {"width",      &config.window.width,      CONF_UINT,   CONF_ARGSEP, NULL},
    {"height",     &config.window.height,     CONF_UINT,   CONF_ARGSEP, NULL},
    {"help",       (void*)conf_usage,         CONF_FUNC,   CONF_ARGNO,  NULL},
    {"version",    (void*)conf_version,       CONF_FUNC,   CONF_ARGNO,  NULL},
  };

#ifdef DEBUG
  fprintf(stderr, "DEBUG: conf_init: size of ConfStruct: %d bytes\n",
          sizeof(ConfStruct));
#endif

  /** save program's filename */
  config.progname = malloc(strlen((*argvp)[0]));
  strcpy(config.progname, (*argvp)[0]);
  (*argvp)++;
  (*argcp)--;
  
  /** init config structure with hard-coded default values */
  conf_setdefaults();

  /** parse config files */
  conf_parsefile(".alpharc", options, OptionNumber);

  /** parse command line */
  conf_parseargs(argcp, argvp, options, OptionNumber);

  return EXIT_SUCCESS;
}

/*****************************************************************************/

void conf_usage(void)
{
  fprintf(stderr, "%s: %s version %d.%d.%d\n", config.progname, "IsoTest",
          0, 0, 1);
  fprintf(stderr, "usage: %s [<option> ...]\n", config.progname);
  fprintf(stderr, 
    "   -help              Show this message and exit.\n"
    "   -width <WIDTH>     Set window width to WIDTH.\n"
    "   -height <HEIGHT>   Set window height to HEIGHT.\n"
    "   -fullscreen        Start in fullscreen mode.\n"
    "   -resizable         Window is resizable.\n"
    "   -invertmouse       Right-Mouse-Button-Scrolling is inverted\n"
    );
#ifdef HAVE_SDLMIXER
  fprintf(stderr,
    "   -nosound           Disable sound\n"
    "   -nomusic           Don't play music\n"
    );
#endif
  fprintf(stderr,
    "   -version           Show version information and exit\n"
    );

  exit(EXIT_FAILURE);
}

/*****************************************************************************/

void conf_version(void)
{
  fprintf(stderr, "%s: %s version %u.%u.%u\n", config.progname, "IsoTest",
          0, 0, 1);
  exit(EXIT_FAILURE);
}

/*****************************************************************************/

void conf_setdefaults(void)
{
  config.window.width = 640;
  config.window.height = 480;
  config.window.fullscreen = 0;
  config.window.resizable = 0;

#ifdef HAVE_SDLMIXER
  config.sound.enabled = 1;
  config.sound.music = 1;
  config.sound.frequency = 44100;
  config.sound.musiclist = NULL;
#endif

  config.map.tile.width = 128;
  config.map.tile.height = 64;

  config.map.pixels.width = 0;               /* generated */
  config.map.pixels.height = 0;              /* generated */

  config.map.name = misc_string_copym("test.map");
  config.map.buffer = NULL;
  config.map.heights = NULL;
 
  config.map.scale = 1.0; 
  config.map.width = 0;
  config.map.height = 0;

  config.minimap.alpha = 0xC0;
#ifdef MMBG
  config.minimap.background = 0xFF80FF;
#else
  config.minimap.background = -1;
#endif
  config.minimap.scalex = 1;
  config.minimap.scaley = 0.5;
  config.minimap.ground = NULL;
  config.minimap.tracks = NULL;

  config.message.surface = NULL;
  config.message.queue = NULL;
  config.message.visnum = 0;

  config.buffer.width = 300;
  config.buffer.height = 300;
  config.buffer.keep_invis = 2;
  config.buffer.num_x = 0;                   /* generated */
  config.buffer.num_y = 0;                   /* generated */

  config.tooltip.surface = NULL;
  config.tooltip.enabled = 1;

  config.scrolling.borderwidth = 10;
  config.scrolling.distancex = 30;
  config.scrolling.distancey = config.scrolling.distancex / 2;
  config.scrolling.invert = 0;

  config.caches.imagecache = NULL;
  config.caches.tileinfos = NULL;

  config.routing.waypoints = NULL;
  config.routing.railblocks = NULL;

  config.performance.alpha = 1;

  config.memory.max = (32<<20);
  config.memory.used = 0;

  config.flags = 0;

  /** initial memory */
  config.memory.used += strlen(config.map.name);
}

/*****************************************************************************/

void conf_parseargs(int *argcp, char ***argvp, 
                    OptionStruct *options, Uint32 numoptions)
{
  Uint8 bool;
  Uint32 i;
  Uint32 uval = 0;
  int found;
  char text[1024];
  void (*function)();
  char *err, *svar = NULL;

#ifdef DEBUG_VV
  fprintf(stderr, "DEBUG: conf_parseargs: num options: %d\n", *argcp);
  for(i=0; i<*argcp; i++) 
    fprintf(stderr, "DEBUG: conf_parseargs: option '%s'\n", (*argvp)[i]);
#endif

  while(*argcp > 0)
  {
    found = -1;
    for(i=0; i<numoptions; i++)
    {
#ifdef HAVE_SNPRINTF
      snprintf(text, 1024, "-%s", options[i].name);
#else
      sprintf(text, "-%s", options[i].name);
#endif
      if(strcmp(text, **argvp) == 0)
      {
        found = i;
        break;
      }
    }
    if(found != -1)
    {
      /** match found */
#ifdef DEBUG
      fprintf(stderr, "DEBUG: conf_parseargs: known option: '%s'\n", **argvp);
#endif
      switch(options[found].type)
      {
        case CONF_BOOL:
          bool = 0;
          if(options[found].set != NULL)
          {
            if(strcmp(options[found].set, "1") == 0) bool = 1;
            if(strcmp(options[found].set, "on") == 0) bool = 1;
            if(strcmp(options[found].set, "On") == 0) bool = 1;
            if(strcmp(options[found].set, "true") == 0) bool = 1;
            if(strcmp(options[found].set, "True") == 0) bool = 1;
          }
          *(Uint8*)options[found].var = bool;
          break;
 
        case CONF_UINT:
          err = NULL;
          switch(options[found].arg)
          {
            case CONF_ARGNO:
              uval = strtoul(options[found].set, &err, 0);
              break;
            case CONF_ARGSEP:
              if((*argcp) > 1)
              {
                /** get next argument */
                (*argcp)--;
                (*argvp)++;
                uval = strtoul(**argvp, &err, 0);
              }
              else
              {
                /** argument required */
                fprintf(stderr, "ERROR: option '%s' requires an argument\n",
                        **argvp);
                exit(EXIT_FAILURE);
              }
              break;
          }
          if((err == NULL) || (strlen(err) == 0))
          {
            /** number conversion successful */
            *(Uint32*)options[found].var = uval;
          }
          else
          {
            fprintf(stderr, "ERROR: couldn't convert '%s' to numeric value\n",
                    err);
            exit(EXIT_FAILURE);
          }
          break;

        case CONF_STRING:
          switch(options[found].arg)
          {
            case CONF_ARGNO:
              /* casted from 'const char *' */
              svar = (char *)options[found].set;
              break;
            case CONF_ARGSEP:
              if((*argcp) > 1)
              {
                (*argcp)--;
                (*argvp)++;
                svar = **argvp;
              }
              else
              {
                fprintf(stderr, "ERROR: option '%s' requires an argument\n",
                        **argvp);
                exit(EXIT_FAILURE);
              }
              break;
          }
          if(*(char**)options[found].var == NULL)
          {
            /** not yet allocated, create new string */
            *(char**)options[found].var = malloc(strlen(svar)+1);
            strcpy(*(char**)options[found].var, svar);
          }
          else
          {
            /** string exists, allocate new size */
            *(char**)options[found].var = 
              (char *)realloc(*(char **)options[found].var, strlen(svar)+1);
            strcpy(*(char**)options[found].var, svar);
            (*(char**)options[found].var)[strlen(svar)] = '\0';
            /** ^^^^^^^^^^^^^^^^^^^ this smells like a segmentation fault */
          }
          break;

        case CONF_FUNC:
          /** get function and call it */
          function = (void(*)())options[found].var;
          (*function)();
          break;
      }
    }
    else
    {
      /** unknown option found */
      fprintf(stderr, "ERROR: unknown option in commandline: '%s'\n", **argvp);
    }
    (*argcp)--;
    (*argvp)++;
  }
}

/*****************************************************************************/

char *_conf_getvalue(char *buffer, int maxlen, const char *line)
{
  char *bufp = (char *)line;
  int pos;

  buffer[0] = '\0';
  while(*bufp && isspace(*bufp)) bufp++;
  if(!*bufp || (*bufp != '=')) return NULL; else bufp++;
  while(*bufp && isspace(*bufp)) bufp++;
  if(!*bufp) return 0;
  pos = 0;
  if(*bufp == '\"')
  {
    bufp++;
    /** quoted string */
    while(*bufp && (*bufp != '\"') && (*bufp != '\n') && (pos < (maxlen - 1)))
    {
      if(*bufp == '\\') bufp++;
      buffer[pos] = *bufp;      
      pos++;
      bufp++;
    }
  }
  else
  {
    /** unquoted string / number */
    while(*bufp && (*bufp != '#') && (*bufp != '\n') && (pos < (maxlen - 1)))
    {
      if(*bufp == '\\') bufp++;
      buffer[pos] = *bufp;
      pos++;
      bufp++;
    }
    /** remove trailing spaces */
    while(isspace(buffer[pos-1])) pos--;
  }
  buffer[pos] = '\0';
  return buffer;
}

/*****************************************************************************/

void conf_parsefile(const char *filename, OptionStruct *options,
                    Uint32 numoptions)
{
  FILE *file;
  char buffer[1024], varname[1024], value[1024];
  char *bufp, *varp, *err, *svar = NULL;
  int i, len, line, found = -1, bool;
  Uint32 uval = 0;

  file = fopen(filename, "r");
  if(file == NULL)
  {
#ifdef DEBUG
    fprintf(stderr, "DEBUG: conf_parsefile: couldn't open '%s'\n", filename);
#endif
    return;
  }

#ifdef DEBUG
  fprintf(stderr, "DEBUG: conf_parsefile: read config file '%s'\n", filename);
#endif
  
  line = 0;
  while(fgets(buffer, 1024, file) != NULL)
  {
    line++;
    /** remove leading spaces */
    bufp = buffer;
    while(*bufp && isspace(*bufp)) bufp++;
    /** ignore comment */
    if(*bufp == '#') continue;
    /** variable name */
    len = 0;
    varp = bufp;
    while(*bufp && isalnum(*bufp))
    {
      len++;
      bufp++;
    }
    if(len != 0)
    {
      strncpy(varname, varp, len);
      varname[len] = '\0';
#ifdef DEBUG
      fprintf(stderr, "DEBUG: conf_parsefile: varname '%s' found\n", varname);
#endif
      for(i=0; i<numoptions; i++)
      {
        found = -1;
        if(strcmp(varname, options[i].name) == 0)
        {
          found = i;
          break;
        }
      }
      if(found != -1)
      {
        switch(options[found].type)
        {
          case CONF_BOOL:
            bool = 0;
            if(options[found].set != NULL)
            {
              if(strcmp(options[found].set, "1") == 0) bool = 1;
              if(strcmp(options[found].set, "on") == 0) bool = 1;
              if(strcmp(options[found].set, "On") == 0) bool = 1;
              if(strcmp(options[found].set, "true") == 0) bool = 1;
              if(strcmp(options[found].set, "True") == 0) bool = 1;
            }
            *(Uint8*)options[found].var = bool;
            break;

          case CONF_UINT:
            err = NULL;
            switch(options[found].arg)
            {
              case CONF_ARGNO:
                uval = strtoul(options[found].set, &err, 0);
                break;
              case CONF_ARGSEP:
                _conf_getvalue(value, 1024, bufp);
                if(strlen(value) != 0)
                {
                  uval = strtoul(value, &err, 0);
                }
                else
                {
                  fprintf(stderr, "ERROR: '%s': line %d: option '%s' requires "
                                  "an argument\n", filename, line, varname);
                  /** exit() on error in config file? */
                }
                break;
            }
            if((err == NULL) || (strlen(err) == 0))
            {
              *(Uint32*)options[found].var = uval;
            }
            else
            {
              fprintf(stderr, "ERROR: '%s': line %d: couldn't convert '%s' to "
                              "numeric value\n", filename, line, value);
            }
            break;

          case CONF_STRING:
            switch(options[found].arg)
            {
              case CONF_ARGNO:
                svar = (char *)options[found].set;
                break;
              case CONF_ARGSEP:
                svar = _conf_getvalue(value, 1024, bufp);
                if(strlen(svar) == 0)
                {
                  fprintf(stderr, "ERROR: '%s': line %d: option '%s' requires "
                                  "an argument\n", filename, line, varname);
                }
                break;
            }
            if(strlen(svar) > 0)
            {
              if(*(char**)options[found].var == NULL)
              {
                /** not yet allocated, create new string */
                *(char**)options[found].var = malloc(strlen(svar)+1);
                strcpy(*(char**)options[found].var, svar);
              }
              else
              {
                /** string exists, allocate new size */
                *(char**)options[found].var =
                 (char *)realloc(*(char **)options[found].var, strlen(svar)+1);
                strcpy(*(char**)options[found].var, svar);
                (*(char**)options[found].var)[strlen(svar)] = '\0';
              }
            }
            break;

          case CONF_FUNC:
            /** functions not handled in config files? */
            break;
        }
      }
      else /** unknown varname */
      {
        fprintf(stderr, "ERROR: config file '%s': line %d: unknown variable "
                        "'%s'\n", filename, line, varname);
      }
    }
  }
  fclose(file);
}


