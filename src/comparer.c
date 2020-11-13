#include "lib.c"
#include <stdio.h>

#define err(...) (fprintf(stderr, __VA_ARGS__))

int main( int arga, char* argb[] ){
    if (arga > 2) return -1;

    FILE *bmp_file_1, *bmp_file_2;

    bmp_file_1 = fopen(argb[1], "rb");
    bmp_file_2 = fopen(argb[2], "rb");

    struct file_bmp bmp_info1, bmp_info2;
    int error_returned = get_file_info(bmp_file_1, &bmp_info1);
    if (error_returned){
        err("Impossible to read the first file\n");
        return -1;
    }
    error_returned = get_file_info(bmp_file_2, &bmp_info2);
    if (error_returned){
        err("Impossible to read the second file\n");
        return -1;
    }

    error_returned = compare_info(&bmp_info1, &bmp_info2);
    if (error_returned) return -1;

    if (!bmp_info1.color_numbers) bmp_info1.color_numbers = 256;
    unsigned int palette_size = bmp_info1.color_numbers;
    error_returned = compare_pixels(bmp_file_1, bmp_file_2, &bmp_info1, &palette_size);
    if (error_returned) return -1;
    return 0;
}

