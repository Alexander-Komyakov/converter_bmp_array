#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int check_bmp(char * path) {
    printf("%s\n", path);
    FILE * image = fopen(path, "rb");
    if (image == NULL) {
        perror("File not opened");
        return 1;
    }
    char *begin_file = (char*) malloc(sizeof(char)*2);
    fread(begin_file, sizeof(char)*2, 2, image);
    if (begin_file[0] == 66 && begin_file[1] == 77) {
        fclose(image);
        free(begin_file);
        return 0;
    }
    fclose(image);
    free(begin_file);
    return 1;
}

int main() {
    printf("BMP = %i\n", check_bmp("aboba.bmp"));
    return 0;
}
