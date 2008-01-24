#!/bin/sh

if [ $# -ne 3 ]; then
  echo "$0 <infile> <width> <height>"
  exit 1;
fi

infile="$1"
width="$2"
height="$3"
frms=30

ox=329
oy=263

x=`echo "$ox - ($width / 2)" | bc`
y=`echo "$oy - ($height / 2)" | bc`

CONVERT="convert"
POVRAY="povray"
POVOPT="povray.ini +I$infile +KI0.0 +KF1.0 +KFI1 +KFF$frms +FN"

$POVRAY $POVOPT || exit 1

permille="907"

newwidth=`echo "($width * $permille) / 1000" | bc`
newheight=`echo "($width * $permille) / 1000" | bc`
fullwidth=`echo "$newwidth * $frms" | bc`
fullname=`echo $infile | sed -e 's/\.pov$/-rotated.png/'`

$CONVERT -size "${fullwidth}x${newheight}" -colorspace Transparent NULL: \
  "$fullname"
$CONVERT -draw 'matte 0,0 reset' "$fullname" "$fullname"

for i in `seq -w 1 $frms`; do
  echo -n "frame $i: "
  name="`echo $infile | sed -e 's/\.pov$//'`$i"
  $CONVERT -crop "${width}x${height}+${x}+${y}" "png:$name.png" \
    "png:$name-cropped.png"
  echo -n "cropped"
  $CONVERT -geometry "${newwidth}x${newheight}" "png:$name-cropped.png" \
    "png:$name-resized.png"
  echo -n ", resized"
  #rm "$name-cropped.png"
  offx=`echo "($i - 1) * $newwidth" | bc`
  $CONVERT -draw "image Over $offx,0 $newwidth,$newheight $name-resized.png" \
    "$fullname" "$fullname"
  echo ", done"
  #rm "$name-resized.png"
  #rm "$name.png"
done

