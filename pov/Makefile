POV     = povray
VIEWER  = display
CNV     = convert
RM      = rm
CP      = cp

IMGDIR  = ../data/images/

IMGS = track_v.png track_h.png \
       track_c_n.png track_c_e.png track_c_w.png track_c_s.png

.PHONY: all tiles clean view
.SUFFIXES: .pov .png

CROP_track_v   = 181x91+331+327
SIZE_track_v   = 128x64
CROP_track_h   = ${CROP_track_v}
SIZE_track_h   = ${SIZE_track_v}
CROP_track_c_n = 725x362+59+327
SIZE_track_c_n = 512x256
CROP_track_c_e = ${CROP_track_c_n}
SIZE_track_c_e = ${SIZE_track_c_n}
CROP_track_c_w = ${CROP_track_c_n}
SIZE_track_c_w = ${SIZE_track_c_n}
CROP_track_c_s = ${CROP_track_c_n}
SIZE_track_c_s = ${SIZE_track_c_n}

all: tiles

tiles: ${IMGS}

${IMGS}: macros.inc settings.inc povray.ini

.pov.png: $< povray.ini macros.inc
	$(POV) povray.ini +W1024 +H768 -I$< +FN +O${@:.png=_rend.png}
	$(CNV) -crop ${CROP_${@:.png=}} ${@:.png=_rend.png} png:${@:.png=_crop.png}
	$(CNV) -geometry ${SIZE_${@:.png=}}! png:${@:.png=_crop.png} $@
	$(RM) -f ${@:.png=_rend.png} ${@:.png=_crop.png}

copy: ${IMGS}
	$(CP) ${IMGS} ${IMGDIR}

clean:
	$(RM) -f ${IMGS} *.png

view: ${IMGS}
	$(VIEWER) ${IMGS}

