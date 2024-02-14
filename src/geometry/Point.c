#include "geometry/Point.h"
#include <math.h>
#include "matrix/Matrix.h"
#include "imageProcessing/Image.h"
#include "imageProcessing/SquareDetection.h"
const size_t POINT_RADIUS = 3;


Point* P_Create(int x, int y)
{
    Point* point = malloc(sizeof(Point));
    point->x = x;
    point->y = y;
    return point;
}

void P_DrawPixelFrom3D(Matrix* img,size_t fromX, size_t fromY, size_t toX, size_t toY)
{
    size_t inputIndex = (fromX + fromY * img->cols) * 3;
    size_t outputIndex = (toX + toY * img->cols) * 3;

    img->data[outputIndex] = img->data[inputIndex];
    img->data[outputIndex + 1] = img->data[inputIndex + 1];
    img->data[outputIndex + 2] = img->data[inputIndex + 2];
}

void P_DrawSegment(Matrix* matrix, Point* p1, Point* p2, Color color, int grayscale)
{
    float dx = p2->x - p1->x;
    float dy = p2->y - p1->y;
    float steps = 0;
    if(fabsf(dx) > fabsf(dy))
    {
        steps = fabsf(dx);
    }
    else
    {
        steps = fabsf(dy);
    }
    float xInc = dx / steps;
    float yInc = dy / steps;
    float x = p1->x;
    float y = p1->y;
    for (size_t i = 0; i < steps; i++)
    {
        
        size_t x_int = (int)x;
        size_t y_int = (int)y;
        
        if(x_int >= 0 && x_int < matrix->cols && y_int >= 0 && y_int < matrix->rows)
        {
            if(!grayscale)
            {
                x_int *= 3;
                y_int *= 3;
            }
            matrix->data[y_int * matrix->cols + x_int] = color.r / 255.0f;
            if(!grayscale)
            {
                matrix->data[y_int * matrix->cols + x_int + 1] = color.g / 255.0f;
                matrix->data[y_int * matrix->cols + x_int + 2] = color.b / 255.0f;
            }
        }
        x += xInc;
        y += yInc;
    }
}

int P_IsSegmentComplete(const Matrix* m, Point* a, Point* b, int searchRadius)
{
    float dx = b->x - a->x;
    float dy = b->y - a->y;
    float steps = 0;
    if(fabsf(dx) > fabsf(dy))
    {
        steps = fabsf(dx);
    }
    else
    {
        steps = fabsf(dy);
    }
    float xInc = dx / steps;
    float yInc = dy / steps;
    float x = a->x;
    float y = a->y;
    for (size_t i = 0; i < steps; i++)
    {
        int found = 0;
        for (int xRadius = -searchRadius; xRadius < searchRadius && !found; xRadius++)
        {
            for (int yRadius = -searchRadius; yRadius < searchRadius && !found; yRadius++)
            {
                size_t x_int = (int)x + xRadius;
                size_t y_int = (int)y + yRadius;
                if(x_int >= 0 && x_int < m->cols && y_int >= 0 && y_int < m->rows)
                {
                    if(m->data[y_int * m->cols + x_int] >= 0.9)
                    {
                        found = 1;
                        break;
                    }
                }
            }
        }
        if(!found)
        {
            return 0;
        }
        
        x += xInc;
        y += yInc;
    }
    return 1;

}

float P_Distance(Point* p, Point* other)
{
    return sqrt(pow(p->x - other->x, 2) + pow(p->y - other->y, 2));
}

Point P_Add(Point* p1, Point* p2)
{
    Point res = {p1->x + p2->x, p1->y + p2->y};
    return res;
}

int P_Equals(Point* p1, Point* p2)
{
    return (p1->x == p2->x && p1->y == p2->y);
}


void P_DrawSDL(Image* _image, Point* p, __uint32_t _color)
{
#if MOBILE

    Image* image = (Image*)_image;
    
    for (size_t x = p->x - POINT_RADIUS; x < p->x + POINT_RADIUS; x++)
    {
        for (size_t y = p->y - POINT_RADIUS; y < p->y + POINT_RADIUS; y++)
        {
            size_t index = (x + y * image->width) * 4;
            if(index >= 0 && index < image->width * image->height * 4)
            {
                __uint32_t* color = (__uint32_t*)&image->pixels[index];
                *color = _color;
            }
        }
        
    }
    
#else



    //Lock surface
    printf("start\n");
    if( SDL_MUSTLOCK( _image ) )
    {
        printf("here\n");
        SDL_LockSurface( _image );
    }
    printf("here 2\n");
    Uint32* pixels = (Uint32*)_image->pixels;
    printf("here 3 \n");
    for (int x = p->x - POINT_RADIUS; x < p->x + POINT_RADIUS; x++)
    {
        for (int y = p->y - POINT_RADIUS; y < p->y + POINT_RADIUS; y++)
        {
            if(x >= 0 && x < _image->w && y >= 0 && y < _image->h)
            {
                printf("x : %i y : %i\n",x,y);
                pixels[y * _image->w + x] = (Uint32)_color;
            }
        }
    }

    //Unlock surface
    if( SDL_MUSTLOCK( _image ) )
    {
        SDL_UnlockSurface( _image );
    }
#endif
    
}


void P_Free(Point* point)
{
    free(point);
}

PointSet* P_GetAllPointBetween(Matrix* matrix, Point* p1, Point* p2) {
    PointSet* res = malloc(sizeof(PointSet));

    float dx = p2->x - p1->x;
    float dy = p2->y - p1->y;
    float steps = 0;
    if(fabsf(dx) > fabsf(dy))
    {
        steps = fabsf(dx);
    }
    else
    {
        steps = fabsf(dy);
    }
    float xInc = dx / steps;
    float yInc = dy / steps;
    float x = p1->x;
    float y = p1->y;
    res->points = NULL;
    res->size = 0;
    for (size_t i = 0; i < steps; i++)
    {
        
        size_t x_int = (int)x;
        size_t y_int = (int)y;
        
        if(x_int >= 0 && x_int < matrix->cols && y_int >= 0 && y_int < matrix->rows)
        {
            Point* p = P_Create(x_int, y_int);
            res->points = realloc(res->points, sizeof(Point) * (res->size + 1));
            res->points[res->size] = *p;
            res->size++;
        }
        x += xInc;
        y += yInc;
    }
    return res;
}

void P_GetPerpendicular(Point* direction)
{
    direction->x = -direction->x;
    direction->y = -direction->y;
}

void P_PointSetDraw(Matrix* image, PointSet* pointSet, Color color, int grayscale)
{
    for (size_t i = 0; i < pointSet->size; i++)
    {
        Point* p = &pointSet->points[i];
        size_t x_int = (size_t)p->x;
        size_t y_int = (size_t)p->y;
        if(x_int >= 0 && x_int < image->cols && y_int >= 0 && y_int < image->rows)
        {
            if(!grayscale)
            {
                x_int *= 3;
                y_int *= 3;
            }
            image->data[y_int * image->cols + x_int] = color.r / 255.0f;
            if(!grayscale)
            {
                image->data[y_int * image->cols + x_int + 1] = color.g / 255.0f;
                image->data[y_int * image->cols + x_int + 2] = color.b / 255.0f;
            }
        }
    }
}


void P_DrawPixel(Matrix* img, size_t x, size_t y, Color color)
{
    size_t x_int = (size_t)x;
    size_t y_int = (size_t)y;
    if(x_int >= 0 && x_int < img->cols && y_int >= 0 && y_int < img->rows)
    {
        x_int *= 3;
        y_int *= 3;
        img->data[y_int * img->cols + x_int] = color.r / 255.0f;
        img->data[y_int * img->cols + x_int + 1] = color.g / 255.0f;
        img->data[y_int * img->cols + x_int + 2] = color.b / 255.0f;
    }
}


Color P_GetColor(Matrix* img, size_t x, size_t y)
{
    size_t x_int = (size_t)x;
    size_t y_int = (size_t)y;
    x_int *= 3;
    y_int *= 3;
    Color color = {img->data[y_int * img->cols + x_int] * 255.0f, img->data[y_int * img->cols + x_int + 1] * 255.0f, img->data[y_int * img->cols + x_int + 2] * 255.0f};
    return color;
}


Point P_Center(Point* points, size_t size)
{
    Point center = {0, 0};
    for (size_t i = 0; i < size; i++)
    {
        center.x += points[i].x;
        center.y += points[i].y;
    }
    center.x /= size;
    center.y /= size;
    return center;
}

void P_Mult(Point* p, float factor)
{
    p->x *= factor;
    p->y *= factor;
}

void P_Sub(Point* p1, Point* p2)
{
    p1->x -= p2->x;
    p1->y -= p2->y;
}