###############################################################################
## configuration ##############################################################
##
## program name (important, has to be set)
PROGRAM = alpha
##
## prefix (needed later for installation)
#PREFIX = /usr/local
##
## data directory
DATADIR = ./data
#DATADIR = ${PREFIX}/share/${PROGRAM}
##
##=============================================================================
##
DEFS =
##
## debug output
DEFS += -DDEBUG
##
## debug very verbose: really noisy, old debugging stuff
#DEFS += -DDEBUG_VV
##
## needed for snprintf
DEFS += -D_GNU_SOURCE
##
## have snprintf function, implies _GNU_SOURCE
DEFS += -DHAVE_SNPRINTF
##
## compile with zlib (compressed maps)
DEFS += -DHAVE_ZLIB
##
## SDL_mixer for audio support
DEFS += -DHAVE_SDLMIXER
##
## stat() syscall?
DEFS += -DHAVE_STAT
##
## throttle output to 25 fps
DEFS += -DDELAY
##
## minimap background
#DEFS += -DMMBG
##
##=============================================================================
##
## debug
CFLAGS = -Wall -g -ansi -pedantic
##
## optimized (warning: -O3 breaks something in video)
#CFLAGS = -Wall -O2 -march=i686 -mcpu=i686
##
##============================================================================
##
## generate playlist
GEN_PLAYLIST = 1
##
##============================================================================
##
CC = gcc
##
###############################################################################
## end of configuration - edit below this line at your own risk ###############

DEFS += -DDATADIR="\"${DATADIR}\""

INCS = $(shell sdl-config --cflags)

# GNU make only?
ifneq (,$(findstring -DHAVE_ZLIB,${DEFS}))
  ZLIBS = -lz
else
  ZLIBS = 
endif

ifneq (,$(findstring -DHAVE_SDLMIXER,${DEFS}))
  MIXLIBS = -lSDL_mixer
else
  MIXLIBS = 
endif

LIBS = -lm $(shell sdl-config --libs) -lSDL_image ${MIXLIBS} ${ZLIBS}

OBJS = main.o game.o video.o imagecache.o mapengine.o minimap.o gui.o \
       waypoint.o route.o \
       audio.o music.o tooltip.o image_scale.o tiles.o message.o \
       widgetbox.o widget.o widget_button.o widget_frame.o widget_label.o \
       widget_scrollbox.o windowlist.o window.o \
       gfx_prim.o text.o mem.o conf.o maploader.o \
       misc.o calc.o list.o parse.o vfs.o

DATA =

ifeq (1,${GEN_PLAYLIST})
  DATA += data/music/music.pls
endif

PNAME = $(shell basename `pwd`)

## Rules ######################################################################

.PHONY: all clean new ignore smalldist dist package stat dep depend

.SUFFIXES: .c .o

all: ${PROGRAM} ${DATA}

new: clean all

ignore:
	cd .. && find ${PNAME} -name .xvpics -o -name .thumbnails > \
      ${PNAME}/.ignore
	echo ".ignore" >> .ignore

smalldist: clean ignore
	shopt -s nullglob; \
      cd .. && ls -1 ${PNAME}/data/music/*.mod >> ${PNAME}/.ignore
	cd .. && tar -c ${PNAME} -X ${PNAME}/.ignore | \
      bzip2 -9 >${HOME}/${PNAME}-$(shell date +"%Y%m%d")-small.tar.bz2
	@du -h ${HOME}/${PNAME}-$(shell date +"%Y%m%d")-small.tar.bz2

dist package: clean ignore
	cd .. && tar -c ${PNAME} -X ${PNAME}/.ignore | \
      bzip2 -9 >${HOME}/${PNAME}-$(shell date +"%Y%m%d").tar.bz2
	@du -h ${HOME}/${PNAME}-$(shell date +"%Y%m%d").tar.bz2

stat:
	wc -lc `ls -1 *.c *.h | sort`

${PROGRAM}: ${OBJS}
	$(CC) -o $@ ${OBJS} ${LIBS}

.c.o: $(<:.c=.h)
	$(CC) -c ${DEFS} ${CFLAGS} ${INCS} -o $@ $<

data/music/music.pls: data/music/
	shopt -s nullglob; ls -1 data/music/*.mod | sed 's#^data/music/##' >$@

$(OBJS): %.o: %.c %.h

# recompile all on conf structure changes
$(OBJS): conf.h

clean:
	rm -f ${OBJS} ${PROGRAM} *~ core
	$(MAKE) -C pov clean


