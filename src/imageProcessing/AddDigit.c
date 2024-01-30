#include "imageProcessing/AddDigit.h"
#include "imageProcessing/Image.h"
#define STB_TRUETYPE_IMPLEMENTATION  
#include "imageProcessing/stb_truetype.h"

const char* fontPath = "fonts/roboto.ttf";



stbtt_fontinfo* initFont()
{
    unsigned char *fontBuffer = NULL;
    stbtt_fontinfo* fontInfo = malloc(sizeof(stbtt_fontinfo));

    // Load the font file into a buffer
    long size;
    FILE *fontFile = fopen(fontPath, "rb");
    if (!fontFile) {
        fprintf(stderr, "Error: Could not open font file\n");
        exit(1);
    }

    fseek(fontFile, 0, SEEK_END);
    size = ftell(fontFile);  // Get the size of the font file
    fseek(fontFile, 0, SEEK_SET);

    fontBuffer = malloc(size);
    if (fread(fontBuffer, 1, size, fontFile) != size) {
        fprintf(stderr, "Error: Could not read font file\n");
        exit(1);
    }
    fclose(fontFile);

    if (!stbtt_InitFont(fontInfo, fontBuffer, stbtt_GetFontOffsetForIndex(fontBuffer,0))) {
        fprintf(stderr, "Error: Failed to initialize font\n");
        exit(1);
    }


    return fontInfo;
}

//The image is 3 matrices, one for each color
void AddDigit(stbtt_fontinfo* fontInfo,Matrix* image, int digit, int x, int y, Color color)
{
    int codepoint = '0' + digit; // Character to render
    float scale = stbtt_ScaleForPixelHeight(fontInfo, 50); // Set font size to 24 pixels high

    int width, height, xoff, yoff;
    unsigned char *bitmap = stbtt_GetCodepointBitmap(fontInfo, 0, scale, codepoint, &width, &height, &xoff, &yoff);

    // Now, 'bitmap' contains the grayscale image of the character
    // You need to implement the code to draw this bitmap on your target surface
    // This part depends on your specific graphics context

    // After you're done with the bitmap, free it
    for(int i = 0; i < width * height; i++)
    {
        if(bitmap[i] != 0)
        {
            size_t index = ((y + i / width) * image->cols + x + i % width) * 3;
            image->data[index] = color.r;
            image->data[index + 1] = color.g;
            image->data[index + 2] = color.b;
        }
    }
        stbtt_FreeBitmap(bitmap, NULL);
}


void AddMissingDigits(Matrix* image, int** digits, int cellWidth, Color color)
{
    stbtt_fontinfo* fontInfo = initFont();
    for (size_t i = 0; i < 9; i++)
    {
        for (size_t j = 0; j < 9; j++)
        {
            if(digits[i][j] != 0)
            {
                AddDigit(fontInfo,image,digits[i][j],j * cellWidth + cellWidth / 2 - 10,i * cellWidth + cellWidth / 2 - 10, color);
            }  
        }
    }
}
