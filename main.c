#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int main(int argc, char * argv[]) {
    char * path = argv[1];
    printf("%s\n", path);

    FILE * image = fopen(path, "rb");
    if (image == NULL) {
        perror("File not opened");
        return 1;
    }
    char *header_file = (char*) malloc(sizeof(char)*1);
    fread(header_file, sizeof(char)*2, 1, image);
    if (header_file[0] != 66 || header_file[1] != 77) {
        fclose(image);
        free(header_file);
        printf("Erorr, it's not bmp\n");
        return 1;
    }

    uint32_t *size_file = (uint32_t*) malloc(sizeof(uint32_t));
    fread(size_file, sizeof(uint32_t), 1, image);

    printf("Size file: %d\n", size_file[0]);

    uint32_t *zero_file = (uint32_t*) malloc(sizeof(uint32_t));
    fread(zero_file, sizeof(uint32_t), 1, image);

    uint32_t *position_file = (uint32_t*) malloc(sizeof(uint32_t));
    fread(position_file, sizeof(uint32_t), 1, image);

    uint16_t *one_file = (uint16_t*) malloc(sizeof(uint16_t));

    char * save_path = argv[2];
    printf("%s\n", save_path);

    FILE * save_image = fopen(save_path, "wt");

    char* path_header = strndup(save_path, strlen(path));
    path_header[strlen(path_header) - 1] = 'h';
    fprintf(save_image, "#include \"%s\"\n", path_header);
    char* name_no_extension = strndup(save_path, strlen(path));
    name_no_extension[strlen(name_no_extension)-2] = '\0';
    fprintf(save_image, "\nconst uint16_t image_%s_pixels[%d] = {", name_no_extension, *size_file);
    fseek(image, 0, SEEK_END);
    for (int i = 0; i < size_file[0]/2; i++) {
        //if (fread(one_file, sizeof(uint16_t), 1, image) == 4) {
        fseek(image, -2 * sizeof(uint16_t), SEEK_CUR);
        if (fread(one_file, sizeof(uint16_t), 1, image) == 4) {
            break;
        }
        if ( i % 10 == 0) {
            fprintf(save_image, "\n");
        }
        uint16_t swapest_bits = (one_file[0] << 8) | (one_file[0] >> 8);
        fprintf(save_image, "0x%04X, ", swapest_bits);
    }
    fprintf(save_image, "\n};\n");
    fprintf(save_image, "Image image_%s = {\n", name_no_extension);
    fprintf(save_image, "    .x = 0,\n");
    fprintf(save_image, "    .y = 0,\n");
    fprintf(save_image, "    .width = 0,\n");
    fprintf(save_image, "    .height = 0,\n");
    fprintf(save_image, "    .size_image = %d,\n", *size_file);
    fprintf(save_image, "    .pixels = image_%s_pixels\n", name_no_extension);
    fprintf(save_image, "};\n");


    FILE * image_header = fopen(path_header, "wt");
    if (image_header == NULL) {
        perror("File header not opened");
        return 1;
    }
    fprintf(image_header, "#include \"image_structure.h\"\n\n");
    fprintf(image_header, "extern const uint16_t image_%s_pixels[%d];\n", name_no_extension, *size_file);
    fprintf(image_header, "extern Image image_%s;\n", name_no_extension);

    fclose(image_header);

    fclose(image);
    free(header_file);
    free(zero_file);
    free(position_file);
    free(one_file);

    fclose(save_image);
    return 1;
}
