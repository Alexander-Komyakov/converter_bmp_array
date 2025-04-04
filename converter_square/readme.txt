# Конвертация из gif в bmp
convert input.gif output.bmp
# Изменение размера на 60x67
convert output.bmp -resize 60x67 resized.bmp
# Конвертация для 16 bit - RGB565
convert E96vzVyXMAM7XeE.jpg -resize 60x45 -type truecolor -depth 16 -define bmp:format=bmp4 -define bmp:subtype=RGB565 debil.bmp
# Явное указание конвертации с соответсвующим размером. Может не работать на большом количестве файлов при создании массива
convert picture/E96vzVyXMAM7XeE.jpg -resize 320x240! -type truecolor -define bmp:format=bmp3 image.bmp
# Скрипт для конвертации (проверено что не работает нормально, так как конвертация происходит только квадрата)
./converter.sh E96vzVyXMAM7XeE.jpg 150x150 image.bmp image.c
