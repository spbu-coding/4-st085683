#include "qdbmp.h"
#include "lib.c"
#include <stdio.h>
#include <string.h>
#define error(...) (fprintf(stderr, __VA_ARGS__))

BMP*	bmp;

int read_arguments( int arga, char* argb[], int* is_mine ){
    if ( arga != 4 ){
        error("Incorrect arguments");
        return -1;
    }

    if (strcmp(argb[1], "--mine") != 0) {
        if (strcmp(argb[1], "--theirs") == 0) {
            *is_mine = 0;
        } else {
            error("Incorrect arguments");
            return -1;
        }
    } else {
        *is_mine = 1;
    }

    for (int c = 2; c < 4; ++c){
        int index_expansion = 0;
        for (long unsigned int d = 0; d < strlen(argb[c]); ++d){
            if (argb[c][d] == '.') index_expansion = d + 1;
        }
        if (index_expansion != 0) {
            if (argb[c][index_expansion] != 'b'){
                error("Incorrect expansion");
                return -1;
            }
            if (argb[c][index_expansion + 1] != 'm'){
                error("Incorrect expansion");
                return -1;
            }
            if (argb[c][index_expansion + 2] != 'p'){
                error("Incorrect expansion");
                return -1;
            }
        }else{
            error("Incorrect expansion");
            return -1;
        }
    }
    return 0;
}

int make_their_bmp(){

    UCHAR	e, g, h;
    UINT	width, height;
    USHORT depth;
    UINT	f, y;

    width = BMP_GetWidth( bmp );
    height = BMP_GetHeight( bmp );
    depth = BMP_GetDepth( bmp );

    if (depth == 24) {
        for (f = 0; f < width; ++f) {
            for (y = 0; y < height; ++y) {

                BMP_GetPixelRGB(bmp, f, y, &e, &g, &h);
                if(BMP_LAST_ERROR_CODE != BMP_OK)
                    return -3;
                BMP_SetPixelRGB(bmp, f, y, ~e, ~g, ~h);
            }
        }
    }else if (depth == 8){
        for (unsigned int d = 0; d < bmp->Header.ColorsUsed; ++d){
            BMP_GetPaletteColor(bmp, d, &e, &g, &h);
            if (BMP_LAST_ERROR_CODE != BMP_OK)
                return -3;
            BMP_SetPaletteColor(bmp, d, ~e, ~g, ~h);
        }

    }else return -3;

    return 0;
}

int make_mine_bmp( char* argb[] ){
    struct file_bmp file_info;
    FILE* stream;
    stream = fopen(argb[2], "rb+");
    int err = get_file_info(stream, &file_info);
    if (err != 0) return err;

    fseek(stream, 0, SEEK_SET);
    FILE *write_file;
    write_file = fopen(argb[3], "wb+");
    err= convert_bmp(stream, write_file, &file_info);
    if (err != 0) return err;
    return 0;
}

int main( int arga, char* argb[] ){
    int is_mine, return_make_bmp;

    int read_arg_return = read_arguments(arga, argb, &is_mine);
    if (read_arg_return){
        return read_arg_return;
    }

    if (is_mine){
        return_make_bmp = make_mine_bmp(argb);
        if (return_make_bmp)
            return return_make_bmp;
    }else{
        bmp = BMP_ReadFile( argb[ 2 ] );
        if(BMP_LAST_ERROR_CODE != BMP_OK)
            return -3;
        return_make_bmp = make_their_bmp();
        BMP_WriteFile( bmp, argb[ 3 ] );
        if(BMP_LAST_ERROR_CODE != BMP_OK)
            return -3;
        BMP_Free( bmp );
        if (return_make_bmp)
            return return_make_bmp;
    }
    return 0;
}





