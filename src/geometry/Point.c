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

void P_DrawSegment(Matrix* matrix, Point* p1, Point* p2, float value)
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
            matrix->data[y_int * matrix->cols + x_int] = value;
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
    return p1->x == p2->x && p1->y == p2->y;
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


PointSet* P_GetAllPointBetween(Point a, Point b)
{
    PointSet* pointSet = malloc(sizeof(PointSet));
    pointSet->size = 0;
    pointSet->points = malloc(sizeof(Point) * (abs(a.x - b.x) + abs(a.y - b.y)));
    float dx = b.x - a.x;
    float dy = b.y - a.y;
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
    float x = a.x;
    float y = a.y;
    for (size_t i = 0; i < steps; i++)
    {
        pointSet->points[i] = (Point){x,y};
        x += xInc;
        y += yInc;
        pointSet->size++;
    }
    return pointSet;
}