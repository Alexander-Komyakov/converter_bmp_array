#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
    // Проверка аргументов командной строки
    if (argc < 3) {
        printf("Usage: %s <input.bmp> <output.c>\n", argv[0]);
        return 1;
    }

    char *path = argv[1];
    char *save_path = argv[2];

    // Открытие BMP-файла
    FILE *image = fopen(path, "rb");
    if (image == NULL) {
        perror("Error opening input file");
        return 1;
    }

    // Проверка сигнатуры BMP (первые 2 байта: 'B' и 'M')
    char header[2];
    if (fread(header, sizeof(char), 2, image) != 2 || header[0] != 'B' || header[1] != 'M') {
        fclose(image);
        printf("Error: Not a BMP file\n");
        return 1;
    }

    // Чтение размера файла из заголовка BMP (смещение 2)
    uint32_t file_size;
    fseek(image, 2, SEEK_SET);  // Переходим к полю размера файла
    if (fread(&file_size, sizeof(uint32_t), 1, image) != 1) {
        fclose(image);
        printf("Error reading file size\n");
        return 1;
    }

    printf("File size: %u bytes\n", file_size);

    // Пропускаем остальные поля заголовка (переходим к данным)
    uint32_t data_offset;
    fseek(image, 10, SEEK_SET);  // Поле смещения данных в BMP
    if (fread(&data_offset, sizeof(uint32_t), 1, image) != 1) {
        fclose(image);
        printf("Error reading data offset\n");
        return 1;
    }

    // Переходим непосредственно к данным изображения
    fseek(image, data_offset, SEEK_SET);

    // Создаём выходной C-файл
    FILE *save_image = fopen(save_path, "wt");
    if (save_image == NULL) {
        fclose(image);
        perror("Error creating output file");
        return 1;
    }

    // Записываем заголовок C-файла
    fprintf(save_image, "#include <stdint.h>\n\n");
    fprintf(save_image, "uint16_t my_image[] = {\n");

    // Читаем данные из BMP и записываем в C-файл
    uint16_t pixel;
    int count = 0;
    while (fread(&pixel, sizeof(uint16_t), 1, image) == 1) {
        // Меняем порядок байтов (если нужно)
        uint16_t swapped = (pixel << 8) | (pixel >> 8);
        
        // Записываем в файл
        fprintf(save_image, "0x%04X, ", swapped);
        
        // Перенос строки каждые 10 элементов
        if (++count % 10 == 0) {
            fprintf(save_image, "\n");
        }
    }

    // Завершаем массив
    fprintf(save_image, "\n};\n");

    // Закрываем файлы
    fclose(image);
    fclose(save_image);

    printf("Conversion completed successfully. Saved to %s\n", save_path);
    return 0;
}
