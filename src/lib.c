#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define unsupported_format -1
#define incorrect_image_structure -2

struct file_bmp {
    unsigned short categorize;
    unsigned int size;
    int width;
    int height;
    unsigned int pixel_array_offset;
    unsigned short bits_per_pixel;
    unsigned int image_size;
    unsigned int horizontal_resolution;
    unsigned int vertical_resolution;
    unsigned int color_numbers;
    unsigned int number_of_important_colors;
};

int read_unsigned_int(unsigned int* f, FILE* stream) {
    unsigned char buffer[4];
    if(fread(buffer , 4, 1, stream) != 1) {
        if(feof(stream) != 0) {
            fprintf(stderr, "There are no bytes left to read in the stream.\n");
        } else {
            fprintf(stderr, "We cannot read this file.\n");
        }
    return 0;
    }
    *f = (buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0]);
    return 1;
}

int read_unsigned_short(unsigned short* f, FILE* stream) {
    unsigned char buffer[2];
    if(fread(buffer, 2, 1, stream) != 1) {
        if(feof(stream) != 0) {
            fprintf(stderr, "There are no bytes left to read in the stream.\n");
        } else {
            fprintf(stderr, "Cannot read file.\n\n");
        }
        return 0;
    }
    *f = (buffer[1] << 8 | buffer[0]);
    return 1;
}

int read_char(unsigned char* f, FILE* stream ) {
    unsigned char buffer[1];
    if(fread(buffer, 1, 1, stream ) != 1) {
        if(feof(stream) != 0) {
            fprintf(stderr, "There are no bytes left to read in the stream.\n");
        } else {
            fprintf(stderr, "We cannot read this file.\n");
        }
        return 0;
    }
    *f = buffer[0];
    return 1;
}

int write_unsigned_int(unsigned int f, FILE* stream ) {
    unsigned char buffer_2[4];
    buffer_2[3] = (unsigned char)((f & 0xff000000) >> 24);
    buffer_2[2] = (unsigned char)((f & 0x00ff0000) >> 16);
    buffer_2[1] = (unsigned char)((f & 0x0000ff00) >> 8);
    buffer_2[0] = (unsigned char)((f & 0x000000ff) >> 0);
    return (stream  && fwrite(buffer_2, 4, 1, stream ) == 1);
}

int write_int(int f, FILE* stream ) {
    char buffer_2[4];
    buffer_2[3] = (char)((f & 0xff000000) >> 24);
    buffer_2[2] = (char)((f & 0x00ff0000) >> 16);
    buffer_2[1] = (char)((f & 0x0000ff00) >> 8);
    buffer_2[0] = (char)((f & 0x000000ff) >> 0);
    return (stream  && fwrite(buffer_2, 4, 1, stream ) == 1);
}

int write_unsigned_short(unsigned short f, FILE* stream ) {
    char buffer_2[2];
    buffer_2[1] = (unsigned char)((f & 0xff00) >> 8);
    buffer_2[0] = (unsigned char)((f & 0x00ff) >> 0);
    return (stream  && fwrite(buffer_2, 2, 1, stream ) == 1);
}

int write_byte(unsigned char byte, FILE* stream ) {
    char buffer_2[1];
    buffer_2[0] = (unsigned char)byte;
    return (stream  && fwrite(buffer_2, 1, 1, stream ) == 1);
}

bool check_type(unsigned short categorize) {
    char categorizes[6][2] = {"BM", "BA", "CI", "CP", "IC", "PT"};
    for(unsigned int i = 0; i < 6; i++) {
        if(memcmp(&categorize, categorizes, 2) == 0) return true;
    }
    return false;
}

int get_file_info(FILE* stream , struct file_bmp *file_info) {
    bool is_error = false;

    unsigned int file_size;
    fseek(stream , 0, SEEK_END);
    file_size = ftell(stream);
    fseek(stream , 0, SEEK_SET);
    if(read_unsigned_short(&file_info->categorize, stream ) == 0) is_error = true;
    if(!check_type(file_info->categorize)) {
        fprintf(stderr, "Try to import the file expansion like .bmp\n");
        return unsupported_format;
    }

    if(read_unsigned_int(&file_info->size, stream ) == 0) is_error = true;
    file_info->size = file_size;

    unsigned int reserved;
    if(read_unsigned_int(&reserved, stream ) == 0) is_error = true;
    if(reserved != 0) {
        fprintf(stderr, "File structure incorrect.\n");
        return incorrect_image_structure;
    }

    if(read_unsigned_int(&file_info->pixel_array_offset, stream ) == 0) is_error = true;

    unsigned bmp_version;
    if(read_unsigned_int(&bmp_version, stream ) == 0) is_error = true;
    if(bmp_version != 4) {
        fprintf(stderr, "The version of bmp file should be 3\n");
        return unsupported_format;
    }

    if(read_unsigned_int(&file_info->width, stream ) == 0) is_error = true;
    if(read_unsigned_int(&file_info->height, stream ) == 0) is_error = true;

    unsigned short number_of_color_planes;
    if(read_unsigned_short(&number_of_color_planes, stream ) == 0) is_error = true;
    if(number_of_color_planes != 1) {
        fprintf(stderr, "the incorrect file structure\n");
        return incorrect_image_structure;
    }

    if(read_unsigned_short(&file_info->bits_per_pixel, stream ) == 0) is_error = true;
    if(file_info->bits_per_pixel != 8 && file_info->bits_per_pixel != 24) {
        fprintf(stderr, "the file should have 8 or 24 bits per pixel\n");
        return unsupported_format;
    }

    unsigned int compression_method;
    if(read_unsigned_int(&compression_method, stream ) == 0) is_error = true;
    if(compression_method != 0) {
        fprintf(stderr, "it doesn't support compression\n");
        return unsupported_format;
    }

    if(read_unsigned_int(&file_info->image_size, stream ) == 0) is_error = true;
    if(file_info->image_size != 0) {
        unsigned int calculated_size = file_info->size - file_info->pixel_array_offset;
        if(calculated_size != file_info->image_size) {
            fprintf(stderr, "The size are different\n");
            return incorrect_image_structure;
        }
    }

    if(read_unsigned_int(&file_info->horizontal_resolution, stream) == 0) is_error = true;
    if(read_unsigned_int(&file_info->vertical_resolution, stream) == 0) is_error = true;
    if(read_unsigned_int(&file_info->color_numbers, stream) == 0) is_error = true;
    if(read_unsigned_int(&file_info->number_of_important_colors, stream) == 0) is_error = true;

    if (is_error){
        fprintf(stderr, "BMP structure cannot be readed.\n");
        return incorrect_image_structure;
    }
    return 0;
}

int convert_bmp(FILE *inputFile, FILE *outputFile, struct file_bmp *file_info){
    unsigned int PALETTE_BEGINNING = 54, PALETTE_END = file_info->pixel_array_offset;
    unsigned char byte = 0;

    while(ftell(inputFile) < file_info->size){
        if (read_char(&byte, inputFile) == 0){
            fprintf(stderr, "File cannot be readed.\n");
            return incorrect_image_structure;
        }
        if (file_info->bits_per_pixel == 8 && ftell(inputFile)>PALETTE_BEGINNING &&ftell(inputFile)<PALETTE_END && (ftell(inputFile)-PALETTE_BEGINNING)%4 != 0){
            byte = ~byte;
        }else if (file_info->bits_per_pixel == 24 && ftell(inputFile)>file_info->pixel_array_offset){
            byte = ~byte;
        }
        write_byte(byte, outputFile);
    }
    return 0;
}

int compare_info(struct file_bmp *first_info, struct file_bmp *second_info){
    if (abs(first_info->width) != abs(second_info->width)){
        fprintf(stderr, "Widths are different.");
        return -1;
    }
    if (first_info->height != second_info->height) {
        fprintf(stderr, "Heights are different.");
        return -1;
    }
    if (first_info->bits_per_pixel != second_info->bits_per_pixel){
        fprintf(stderr, "Different bits per pixel.");
        return -1;
    }
    if ((first_info->color_numbers != second_info->color_numbers) && first_info->bits_per_pixel == 8){
        fprintf(stderr, "Color numbers in palette are different.");
        return -1;
    }
    return 0;
}



int compare_pixels(FILE *bmp_file_1, FILE *bmp_file_2, struct file_bmp *file_info, unsigned int *palette_size){
    unsigned int count_of_pixels = 0;
    if (file_info->width < 0) file_info->width = abs(file_info->width);
    if (file_info->height < 0) file_info->height = abs(file_info->height);

    if (file_info->bits_per_pixel == 8){
        unsigned int palette1[256];
        if (fread(palette1, sizeof(unsigned int), *palette_size, bmp_file_1) != *palette_size){
            fprintf(stderr, "First picture palettes cannot be readed.\n");
            return -1;
        }
        unsigned int palette2[256];
        if (fread(palette2, sizeof(unsigned int), *palette_size, bmp_file_2) != *palette_size){
            fprintf(stderr, "Palette of second picture can't be readed.\n");
            return -1;
        }

        unsigned char pixel1 = 0, pixel2 = 0;
        for (int i = 0; i < file_info->height; i++){
            for(int j = 0; j < file_info->width; j++){
                if (fread(&pixel1, 1, 1, bmp_file_1) != 1){
                    fprintf(stderr, "Cannot read the pixel of picture 1\n");
                    return -1;
                }
                if (fread(&pixel2, 1, 1, bmp_file_2) != 1){
                    fprintf(stderr, "Cannot read the pixel of picture 2\n");
                    return -1;
                }
                if (palette1[pixel1] != palette2[pixel2]){
                    fprintf(stderr,"%d %d\n", j, i);
                    count_of_pixels++;
                }
                if (count_of_pixels >= 100) break;
            }
            if (count_of_pixels >= 100) break;
        }
    }else{
        char pixel1[3], pixel2[3];
        for (int i = 0; i < file_info->height; i++) {
            for (int j = 0; j < file_info->width; j++) {
                if (fread(pixel1, 1, 3, bmp_file_1) != 3){
                    fprintf(stderr, "Cannot read the pixel of picture 1\n");
                    return -1;
                }
                if (fread(pixel2, 1, 3, bmp_file_2) != 3){
                    fprintf(stderr, "Cannot read the pixel of picture 2\n");
                    return -1;
                }

                int rgbp1 = (0x00 << 24 | pixel1[2] << 16 | pixel1[1] << 8 | pixel1[0]);
                int rgbp2 = (0x00 << 24 | pixel2[2] << 16 | pixel2[1] << 8 | pixel2[0]);

                if (rgbp1 != rgbp2){
                    fprintf(stderr,"%d %d\n", j, i);
                    count_of_pixels++;
                }
                if (count_of_pixels >= 100) break;
            }
            if (count_of_pixels >= 100) break;
        }
    }
    return 0;
}
