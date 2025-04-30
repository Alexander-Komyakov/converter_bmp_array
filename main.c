#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int main(int argc, char * argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input.bmp> <output.c>\n", argv[0]);
        return 1;
    }

    char * path = argv[1];
    printf("%s\n", path);

    FILE * image = fopen(path, "rb");
    if (image == NULL) {
        perror("File not opened");
        return 1;
    }

    // Проверка сигнатуры BMP
    char signature[2];
    fread(signature, sizeof(char), 2, image);
    if (signature[0] != 'B' || signature[1] != 'M') {
        fclose(image);
        printf("Error, it's not a BMP file\n");
        return 1;
    }

    // Чтение размера файла
    uint32_t file_size;
    fread(&file_size, sizeof(uint32_t), 1, image);

    // Пропускаем 4 байта (зарезервированные)
    fseek(image, 4, SEEK_CUR);

    // Чтение смещения до данных пикселей
    uint32_t pixel_offset;
    fread(&pixel_offset, sizeof(uint32_t), 1, image);

    // Чтение размера DIB-заголовка
    uint32_t dib_header_size;
    fread(&dib_header_size, sizeof(uint32_t), 1, image);

    // Чтение ширины и высоты изображения
    int32_t width, height;
    fread(&width, sizeof(int32_t), 1, image);
    fread(&height, sizeof(int32_t), 1, image);

    printf("File size: %u bytes\n", file_size);
    printf("Image width: %d pixels\n", width);
    printf("Image height: %d pixels\n", height);

    char* save_path = argv[2];
    printf("%s\n", save_path);

    FILE * save_image = fopen(save_path, "wt");
    if (!save_image) {
        perror("Failed to create output file");
        fclose(image);
        return 1;
    }

    char* path_header = strndup(save_path, strlen(save_path));
    path_header[strlen(path_header) - 1] = 'h';
    fprintf(save_image, "#include \"%s\"\n", path_header);
    
    char* name_no_extension = strndup(save_path, strlen(save_path));
    name_no_extension[strlen(name_no_extension)-2] = '\0';
    
    fprintf(save_image, "\nconst uint16_t image_%s_pixels[%d] = {", name_no_extension, width * height);
    
    // Рассчитываем выравнивание строк
    int row_size = width * 2; // 2 байта на пиксель
    int padding = (4 - (row_size % 4)) % 4;
    
    // Перемещаемся к началу данных пикселей
    fseek(image, pixel_offset, SEEK_SET);
    
    // === ИСПРАВЛЕННАЯ ЧАСТЬ ===
    uint16_t pixel; // Добавлено объявление переменной
    uint16_t* pixels = malloc(width * height * sizeof(uint16_t));
    if (!pixels) {
        perror("Memory allocation failed");
        fclose(image);
        fclose(save_image);
        return 1;
    }

    // Читаем строки в обратном порядке (снизу вверх)
    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            if (fread(&pixel, sizeof(uint16_t), 1, image) != 1) {
                break;
            }
            uint16_t swapped_pixel = (pixel << 8) | (pixel >> 8);
            pixels[y * width + x] = swapped_pixel;
        }
        // Пропускаем байты выравнивания
        fseek(image, padding, SEEK_CUR);
    }

    // Записываем пиксели в файл
    for (int i = 0; i < width * height; i++) {
        if (i % 10 == 0) {
            fprintf(save_image, "\n");
        }
        fprintf(save_image, "0x%04X, ", pixels[i]);
    }
    free(pixels);
    // === КОНЕЦ ИСПРАВЛЕННОЙ ЧАСТИ ===
    
    fprintf(save_image, "\n};\n");
    fprintf(save_image, "Image image_%s = {\n", name_no_extension);
    fprintf(save_image, "    .x = 0,\n");
    fprintf(save_image, "    .y = 0,\n");
    fprintf(save_image, "    .width = %d,\n", width);
    fprintf(save_image, "    .height = %d,\n", height);
    fprintf(save_image, "    .size_image = %d,\n", width * height);
    fprintf(save_image, "    .pixels = image_%s_pixels\n", name_no_extension);
    fprintf(save_image, "};\n");

    // Создание header-файла
    FILE * image_header = fopen(path_header, "wt");
    if (image_header == NULL) {
        perror("File header not opened");
        fclose(save_image);
        fclose(image);
        free(name_no_extension);
        free(path_header);
        return 1;
    }
    
    fprintf(image_header, "#include \"image_structure.h\"\n\n");
    fprintf(image_header, "extern const uint16_t image_%s_pixels[%d];\n", name_no_extension, width * height);
    fprintf(image_header, "extern Image image_%s;\n", name_no_extension);

    // Освобождение ресурсов
    fclose(image_header);
    fclose(image);
    fclose(save_image);
    free(name_no_extension);
    free(path_header);

    return 0;
}
