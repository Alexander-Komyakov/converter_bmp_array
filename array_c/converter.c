#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
    // Проверка аргументов командной строки
    if (argc < 3) {
        printf("Usage: %s <input.bmp> <output.c>\n", argv[0]);
        return 1;
    }

    const char *input_path = argv[1];
    const char *output_path = argv[2];

    // Открываем входной BMP-файл
    FILE *bmp_file = fopen(input_path, "rb");
    if (!bmp_file) {
        perror("Error opening input file");
        return 1;
    }

    // Проверяем сигнатуру BMP ('BM')
    char signature[2];
    if (fread(signature, 1, 2, bmp_file) != 2 || signature[0] != 'B' || signature[1] != 'M') {
        fclose(bmp_file);
        printf("Error: Not a valid BMP file\n");
        return 1;
    }

    // Читаем размер файла (поле по смещению 2)
    uint32_t file_size;
    fseek(bmp_file, 2, SEEK_SET);
    if (fread(&file_size, sizeof(uint32_t), 1, bmp_file) != 1) {
        fclose(bmp_file);
        printf("Error reading file size\n");
        return 1;
    }

    // Читаем смещение до данных (поле по смещению 10)
    uint32_t data_offset;
    fseek(bmp_file, 10, SEEK_SET);
    if (fread(&data_offset, sizeof(uint32_t), 1, bmp_file) != 1) {
        fclose(bmp_file);
        printf("Error reading data offset\n");
        return 1;
    }

    // Вычисляем размер данных изображения
    uint32_t image_data_size = file_size - data_offset;
    uint32_t array_length = image_data_size / sizeof(uint16_t);

    printf("Converting BMP to C array...\n");
    printf("Total file size: %u bytes\n", file_size);
    printf("Image data size: %u bytes\n", image_data_size);
    printf("Array elements: %u\n", array_length);

    // Создаём выходной C-файл
    FILE *output_file = fopen(output_path, "w");
    if (!output_file) {
        fclose(bmp_file);
        perror("Error creating output file");
        return 1;
    }

    // Записываем заголовок C-файла
    fprintf(output_file, "#include <stdint.h>\n\n");
    fprintf(output_file, "uint16_t my_image[%u] = {\n", array_length);

    // Переходим к началу данных изображения
    fseek(bmp_file, data_offset, SEEK_SET);

    // Читаем и конвертируем данные
    uint16_t pixel;
    for (uint32_t i = 0; i < array_length; i++) {
        if (fread(&pixel, sizeof(uint16_t), 1, bmp_file) != 1) {
            printf("Warning: File ended prematurely at element %u\n", i);
            break;
        }

        // Меняем порядок байтов (little-endian to big-endian)
        uint16_t swapped = (pixel << 8) | (pixel >> 8);
        
        // Записываем в файл
        fprintf(output_file, "0x%04X", swapped);
        
        // Добавляем запятую, если это не последний элемент
        if (i < array_length - 1) {
            fprintf(output_file, ",");
        }
        
        // Добавляем пробел или перенос строки
        if ((i + 1) % 10 == 0) {
            fprintf(output_file, "\n");
        } else {
            fprintf(output_file, " ");
        }
    }

    // Завершаем массив
    fprintf(output_file, "\n};\n");

    // Закрываем файлы
    fclose(bmp_file);
    fclose(output_file);

    printf("Conversion completed successfully!\n");
    printf("Output saved to: %s\n", output_path);
    return 0;
}
