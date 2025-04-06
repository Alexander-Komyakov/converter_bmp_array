# Чтобы заработал новый действующий бинарь, который форматирует изображение в требуемый размер
# Требуется скачать 2 хидера
wget https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
wget https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h
# Компиляция программы
gcc -o jpg_converter jpg_converter.c -lm
# Запускаем программу
./jpg_converter input.jpg output.c 320x240
# номер коммита для использования - f62ae602fa83aefc605f5aa335519c437a573167
