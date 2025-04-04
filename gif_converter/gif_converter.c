#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <gif_lib.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    size_t size_image;
    uint16_t* pixels;
} Image;
#pragma pack(pop)

typedef struct {
    uint16_t width;
    uint16_t height;
    uint16_t* pixels;
} GifFrame;

uint16_t rgb_to_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

GifFrame* load_gif_frame(GifFileType* gif, SavedImage* frame, ColorMapObject* color_map) {
    GifFrame* result = malloc(sizeof(GifFrame));
    result->width = frame->ImageDesc.Width;
    result->height = frame->ImageDesc.Height;
    result->pixels = malloc(result->width * result->height * sizeof(uint16_t));

    ColorMapObject* cmap = frame->ImageDesc.ColorMap ? frame->ImageDesc.ColorMap : color_map;

    for (int y = 0; y < result->height; y++) {
        for (int x = 0; x < result->width; x++) {
            int px = y * result->width + x;
            GifByteType color_idx = frame->RasterBits[px];
            GifColorType color = cmap->Colors[color_idx];
            result->pixels[px] = rgb_to_rgb565(color.Red, color.Green, color.Blue);
        }
    }

    return result;
}

void save_all_frames(const char* filename, GifFrame** frames, int frame_count) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        perror("Error creating output file");
        return;
    }

    // Write header
    fprintf(f, "#include <stdint.h>\n");
    fprintf(f, "#include \"image.h\"\n\n");

    // Write all pixel arrays first
    for (int i = 0; i < frame_count; i++) {
        GifFrame* frame = frames[i];
        fprintf(f, "const uint16_t my_image_pixels_%d[%d * %d] = {\n", 
                i+1, frame->width, frame->height);

        for (int j = 0; j < frame->width * frame->height; j++) {
            fprintf(f, "0x%04X", frame->pixels[j]);
            if (j < frame->width * frame->height - 1) fprintf(f, ",");
            if ((j + 1) % 10 == 0) fprintf(f, "\n");
            else fprintf(f, " ");
        }

        fprintf(f, "\n};\n\n");
    }

    // Then write all Image structures
    for (int i = 0; i < frame_count; i++) {
        GifFrame* frame = frames[i];
        fprintf(f, "const Image my_image_%d = { \n", i+1);
        fprintf(f, "    .x = 0,\n");
        fprintf(f, "    .y = 0,\n");
        fprintf(f, "    .width = %d,\n", frame->width);
        fprintf(f, "    .height = %d,\n", frame->height);
        fprintf(f, "    .size_image = %d,\n", frame->width * frame->height);
        fprintf(f, "    .pixels = my_image_pixels_%d\n", i+1);
        fprintf(f, "};\n\n");
    }

    fclose(f);
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Usage: %s <input.gif> <output_file> <width>x<height>\n", argv[0]);
        return 1;
    }

    int width, height;
    if (sscanf(argv[3], "%dx%d", &width, &height) != 2) {
        printf("Invalid dimensions format\n");
        return 1;
    }

    int error = 0;
    GifFileType* gif = DGifOpenFileName(argv[1], &error);
    if (!gif) {
        printf("GIF open error: %d\n", error);
        return 1;
    }

    if (DGifSlurp(gif) != GIF_OK) {
        printf("GIF read error\n");
        DGifCloseFile(gif, &error);
        return 1;
    }

    printf("Processing GIF with %d frames\n", gif->ImageCount);

    // First load all frames
    GifFrame** frames = malloc(gif->ImageCount * sizeof(GifFrame*));
    for (int i = 0; i < gif->ImageCount; i++) {
        frames[i] = load_gif_frame(gif, &gif->SavedImages[i], gif->SColorMap);
        printf("Loaded frame %d/%d\n", i+1, gif->ImageCount);
    }

    // Then save them all to a single file
    save_all_frames(argv[2], frames, gif->ImageCount);

    // Cleanup
    for (int i = 0; i < gif->ImageCount; i++) {
        free(frames[i]->pixels);
        free(frames[i]);
    }
    free(frames);
    DGifCloseFile(gif, &error);

    printf("Conversion completed! Output saved to %s\n", argv[2]);
    return 0;
}
