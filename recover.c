// PROGRAM: Recover recovers deleted .jpeg images from a forensic image of a memory card

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t byte;

const int block_size = 512;

int main(int argc, char *argv[])
{
    // Check for command line argument
    if (argc != 2)
    {
        printf("\nUsage: ./recover IMAGE\n");
        return 1;
    }

    // open input file
    FILE *raw_file = fopen(argv[1], "r");
    if (raw_file == NULL)
    {
        printf("\nCould not open card.raw\n");
        return 2;
    }

    byte *buffer = malloc(block_size * sizeof(byte));
    FILE *img;
    int i = 0, foundfirstImg = 0;

    // read forensic image
    while (fread(buffer, 1, block_size, raw_file) == block_size)
    {
        // detect start of new image
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            // close previous image if first image was already found
            if (foundfirstImg == 1)
            {
                fclose(img);
            }
            else
            {
                foundfirstImg = 1;
            }
            // create image file name
            char *filename = (char *) malloc(8 * sizeof(char));
            sprintf(filename, "%03d.jpg", i);

            // create image file
            img = fopen(filename, "w");
            if (img == NULL)
            {
                printf("\nCould not create %s\n", filename);
                return 3;
            }

            free(filename);
            ++i;
        }
        if (foundfirstImg == 1)
        {
            // write to image file from buffer
            fwrite(buffer, 1, block_size, img);
        }
    }

    free(buffer);
    fclose(img);
    fclose(raw_file);
    return 0;
}
