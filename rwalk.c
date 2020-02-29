#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <png.h>

#define DEFAULT_WIDTH 10000
#define DEFAULT_HEIGHT 10000

#define MAX_WIDTH 1000000
#define MAX_HEIGHT 1000000
#define MAX_STEPS 1000000000

char *create_rwalk(long long x_dim, long long y_dim, long long steps);
int save_png(char *raw_data, FILE *fp, long long image_width, long long image_height);

int main(int argc, char **argv)
{
    long long image_width = DEFAULT_WIDTH;
    long long image_height = DEFAULT_HEIGHT;
    long long walk_steps = 0;
    char save_file_path[256] = {0};

    if (argc != 5 && argc != 3)
    {
        printf("Syntax: rwalk <save file path> <number of steps> [optional: <height> <width>]\n");
    }
    else
    {
        memcpy(save_file_path, argv[1], strlen(argv[1]) < 256 ? strlen(argv[1]) : 255);
        walk_steps = atoi(argv[2]);

        if (argc == 5)
        {
            image_height = atoi(argv[3]);
            image_width = atoi(argv[4]);
        }
    }

    if (image_width < MAX_WIDTH && image_width > 0 &&
        image_height < MAX_HEIGHT && image_height > 0 &&
        walk_steps < MAX_STEPS && walk_steps > 0)
    {
        char *base_rwalk = create_rwalk(image_height, image_width, walk_steps);     


        FILE *fp = fopen(save_file_path, "wb");
        if (!fp)
        {
            printf("Opening file failed. Check path and try again.\n");
        }
        else
        {
            int result = save_png(base_rwalk, fp, image_width, image_height);
            printf(result == 0 ? "Success\n" : "Failure\n");
        }
        
    }
    else
    {
        printf("Input validation failed. Input for each parameter must be positive, and less than:\n%d\t\t[steps]\n%d\t\t\t[height]\n%d\t\t\t[width]\n", MAX_STEPS, MAX_HEIGHT, MAX_WIDTH);
    }
}

int save_png(char *raw_data, FILE *fp, long long image_width, long long image_height)
{
    printf("Creating PNG...\n");

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, image_width, image_height, 1, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    //png_set_tIME(png_ptr, info_ptr, time(NULL));

    png_text title_text;
    title_text.compression = PNG_TEXT_COMPRESSION_NONE;
    title_text.key = "Title";
    title_text.text = "Rwalk: random walk";
    png_set_text(png_ptr, info_ptr, &title_text, 1);

    printf("Writing data to memory...\n");

    png_write_info(png_ptr, info_ptr);
    png_bytep row = (png_bytep)calloc(image_width, sizeof(char));

    for (size_t i = 0; i < image_height; i++)
    {
        for (size_t j = 0; j < image_width / 8; j++)
        {
            png_byte add_byte = 0;
            for (size_t k = 0; k < 8; k++)
            {
                add_byte += raw_data[(i * image_width) + (j * 8) + k] == -1 ? pow(2, 7 - k) : 0;
            }
            
            row[j] = add_byte;           
        }
        png_write_row(png_ptr, row);
    }

    printf("PNG encoding...\n");

    png_write_end(png_ptr, NULL);

    printf("Finishing up...\n");

    return 0;
}

char *create_rwalk(long long x_dim, long long y_dim, long long steps)
{
    char *grid = (char *)calloc(x_dim * y_dim, sizeof(char));

    long long c_x = 0;
    long long c_y = 0;
    int f_x = 0;
    int f_y = 0;

    srand(time(NULL));

    for (long long i = 0; i < steps; i++)
    {
        f_x = (rand() % 3) - 1;
        c_x = (c_x + f_x > 0) && (c_x + f_x < x_dim) ? c_x + f_x : c_x;

        f_y = (rand() % 3) - 1;
        c_y = (c_y + f_y > 0) && (c_y + f_y < y_dim) ? c_y + f_y : c_y;

        grid[(c_x * y_dim) + c_y] = 255;
    }

    return grid;
}
