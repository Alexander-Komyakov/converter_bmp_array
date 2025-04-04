#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;
#pragma pack(pop)

uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

void convert_jpg_to_bmp(const char* jpg_path, const char* bmp_path, int width, int height) {
    int w, h, channels;
    unsigned char* data = stbi_load(jpg_path, &w, &h, &channels, 3);
    if (!data) {
        printf("Error loading JPG image\n");
        exit(1);
    }

    // Создаем временный буфер для RGB565
    uint16_t* rgb565_data = malloc(width * height * sizeof(uint16_t));
    
    // Ресайз и конвертация в RGB565
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int src_x = x * w / width;
            int src_y = y * h / height;
            uint8_t* pixel = data + (src_y * w + src_x) * 3;
            rgb565_data[y * width + x] = rgb888_to_rgb565(pixel[0], pixel[1], pixel[2]);
        }
    }

    // Создаем BMP файл
    FILE* f = fopen(bmp_path, "wb");
    if (!f) {
        printf("Error creating BMP file\n");
        exit(1);
    }

    BITMAPFILEHEADER bfh = {
        .bfType = 0x4D42,
        .bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + width * height * 2,
        .bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER),
        .bfReserved1 = 0,
        .bfReserved2 = 0
    };

    BITMAPINFOHEADER bih = {
        .biSize = sizeof(BITMAPINFOHEADER),
        .biWidth = width,
        .biHeight = height,
        .biPlanes = 1,
        .biBitCount = 16,
        .biCompression = 3, // BI_BITFIELDS
        .biSizeImage = width * height * 2,
        .biXPelsPerMeter = 0,
        .biYPelsPerMeter = 0,
        .biClrUsed = 0,
        .biClrImportant = 0
    };

    fwrite(&bfh, sizeof(bfh), 1, f);
    fwrite(&bih, sizeof(bih), 1, f);
    
    // Маски цветов для RGB565
    uint32_t masks[3] = {0xF800, 0x07E0, 0x001F};
    fwrite(masks, sizeof(masks), 1, f);
    
    fwrite(rgb565_data, sizeof(uint16_t), width * height, f);
    fclose(f);
    free(rgb565_data);
    stbi_image_free(data);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <input.jpg> <output.c> <width>x<height>\n", argv[0]);
        return 1;
    }

    const char *input_path = argv[1];
    const char *output_path = argv[2];
    const char *dimensions = argv[3];

    // Парсим размеры
    int width, height;
    if (sscanf(dimensions, "%dx%d", &width, &height) != 2) {
        printf("Invalid dimensions format. Use WxH (e.g. 240x320)\n");
        return 1;
    }

    // Конвертируем JPG в временный BMP
    char temp_bmp[] = "/tmp/temp_converter.bmp";
    convert_jpg_to_bmp(input_path, temp_bmp, width, height);

    // Читаем BMP и генерируем C файл
    FILE *bmp_file = fopen(temp_bmp, "rb");
    if (!bmp_file) {
        perror("Error opening temporary BMP file");
        return 1;
    }

    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER info_header;
    
    if (fread(&file_header, sizeof(BITMAPFILEHEADER), 1, bmp_file) != 1) {
        fclose(bmp_file);
        printf("Error reading BMP file header\n");
        return 1;
    }

    if (file_header.bfType != 0x4D42) {
        fclose(bmp_file);
        printf("Error: Not a valid BMP file\n");
        return 1;
    }

    if (fread(&info_header, sizeof(BITMAPINFOHEADER), 1, bmp_file) != 1) {
        fclose(bmp_file);
        printf("Error reading BMP info header\n");
        return 1;
    }

    int bmp_width = info_header.biWidth;
    int bmp_height = abs(info_header.biHeight);
    size_t pixel_count = bmp_width * bmp_height;

    printf("Converting image %dx%d to C structure...\n", width, height);

    // Читаем данные изображения
    fseek(bmp_file, file_header.bfOffBits, SEEK_SET);
    uint16_t *pixels = malloc(pixel_count * sizeof(uint16_t));
    if (!pixels) {
        fclose(bmp_file);
        printf("Memory allocation failed\n");
        return 1;
    }

    if (fread(pixels, sizeof(uint16_t), pixel_count, bmp_file) != pixel_count) {
        fclose(bmp_file);
        free(pixels);
        printf("Error reading pixel data\n");
        return 1;
    }
    fclose(bmp_file);
    remove(temp_bmp);

    // Создаем выходной файл
    FILE *output_file = fopen(output_path, "w");
    if (!output_file) {
        free(pixels);
        perror("Error creating output file");
        return 1;
    }

    // Записываем заголовок
    fprintf(output_file, "#include <stdint.h>\n");
    fprintf(output_file, "#include <stdio.h>\n\n");
    fprintf(output_file, "typedef struct {\n");
    fprintf(output_file, "    uint16_t x;\n");
    fprintf(output_file, "    uint16_t y;\n");
    fprintf(output_file, "    uint16_t width;\n");
    fprintf(output_file, "    uint16_t height;\n");
    fprintf(output_file, "    size_t size_image;\n");
    fprintf(output_file, "    uint16_t color[%zu];\n", pixel_count);
    fprintf(output_file, "} Image;\n\n");
    fprintf(output_file, "const Image my_image = { \n");
    fprintf(output_file, "0, 0, %d, %d, %zu, {\n", width, height, pixel_count);

    // Записываем данные пикселей
    for (size_t i = 0; i < pixel_count; i++) {
        // BMP хранится как little-endian, меняем порядок байтов
        uint16_t pixel = (pixels[i] << 8) | (pixels[i] >> 8);
        fprintf(output_file, "0x%04X", pixel);
        
        if (i < pixel_count - 1) {
            fprintf(output_file, ",");
        }
        
        if ((i + 1) % 10 == 0) {
            fprintf(output_file, "\n");
        } else {
            fprintf(output_file, " ");
        }
    }

    fprintf(output_file, "\n}};\n");
    fclose(output_file);
    free(pixels);

    printf("Conversion completed successfully!\n");
    printf("Output saved to: %s\n", output_path);
    return 0;
}
