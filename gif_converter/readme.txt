# Для конвертирования gif-файлов в массив устанавливаем зависимость
sudo apt-get install libgif-dev
# Делаем билд
gcc -o gif_converter gif_converter.c -lgif
# Уменьшаем количество анимаций в gif-файле. В данном примере с 5 до самого последнего. 
# К примеру в gifka.gif с девочкой - 64 анимации. Увидеть количество можно в gimp
convert gifka_backup.gif -coalesce -delete 5--1 ban.gif
# Конвертируем набор гифок в виде набора массивов
./gif_converter animation.gif image.c 320x240

