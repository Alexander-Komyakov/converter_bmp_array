# Конвертация из gif в bmp
convert input.gif output.bmp
# Изменение размера на 60x67
convert output.bmp -resize 60x67 resized.bmp
# Конвертация для 16 bit - RGB565
convert E96vzVyXMAM7XeE.jpg -resize 60x45 -type truecolor -depth 16 -define bmp:format=bmp4 -define bmp:subtype=RGB565 debil.bmp
#
convert picture/E96vzVyXMAM7XeE.jpg -resize 320x240! -type truecolor -define bmp:format=bmp3 image.bmp 


./converter.sh picture/E96vzVyXMAM7XeE.jpg 150x150 image.bmp image.c





# Чтобы заработал новый действующий бинарь, который форматирует изображение в требуемый размер
# Требуется скачать 2 хидера
wget https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
wget https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h
# Компиляция программы
gcc -o converter converter.c -lm
# Запускаем программу
./converter input.jpg output.c 320x240
