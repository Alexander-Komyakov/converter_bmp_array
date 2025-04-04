#!/bin/bash
# $1 - jpg-файл, $2 - размер (формат 123x123), $3 - итоговый bmp-файл, $4 - image.c на выходе - массив
convert $1 -resize $2 -type truecolor -depth 16 -define bmp:format=bmp4 -define bmp:subtype=RGB565 $3
./converter $3 $4
