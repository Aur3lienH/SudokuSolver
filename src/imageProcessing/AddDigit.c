#include "imageProcessing/AddDigit.h"
#include "imageProcessing/Image.h"
#define STB_TRUETYPE_IMPLEMENTATION  
#include "imageProcessing/stb_truetype.h"

const char* fontPath = "fonts/roboto.ttf";
const float FONT_FORCE = 0.5f;
const float OVERLIGN_FORCE = 0.5f;


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
            image->data[index] = (color.r / 255.0f) * (bitmap[i] / 255.0f) * FONT_FORCE + image->data[index] * (1 - FONT_FORCE);
            image->data[index + 1] = (color.g / 255.0f) * (bitmap[i] / 255.0f) * FONT_FORCE + image->data[index + 1] * (1 - FONT_FORCE);
            image->data[index + 2] = (color.b / 255.0f) * (bitmap[i] / 255.0f) * FONT_FORCE + image->data[index + 2] * (1 - FONT_FORCE);
        }
    }
    stbtt_FreeBitmap(bitmap, NULL);
}

void OverlignDigit(Matrix* input, size_t rowIndex, size_t colIndex,size_t cellWidth ,Color color)
{
    size_t startRow = rowIndex * cellWidth;
    size_t startCol = colIndex * cellWidth;

    for (size_t i = startRow; i < startRow + cellWidth; i++)
    {
        for (size_t j = startCol; j < startCol + cellWidth; j++)
        {
            size_t index = (i * input->cols + j) * 3;
            input->data[index] = input->data[index] * (1 - OVERLIGN_FORCE) + color.r / 255.0f * OVERLIGN_FORCE;
            input->data[index + 1] = input->data[index + 1] * (1 - OVERLIGN_FORCE) + color.g / 255.0f * OVERLIGN_FORCE;
            input->data[index + 2] = input->data[index + 2] * (1 - OVERLIGN_FORCE) + color.b / 255.0f * OVERLIGN_FORCE;
        }
    }
}


void OverlignMatrix(Matrix* input, int** error)
{
    for (size_t i = 0; i < 9; i++)
    {
        for (size_t j = 0; j < 9; j++)
        {
            if(error[i][j] == -1)
            {
                OverlignDigit(input,i,j,input->cols / 9,Color_Create(255,0,0));
            }
        }
    }
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
