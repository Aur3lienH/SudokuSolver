#include "Point.h"
#include <math.h>
#include "../deepLearning/Matrix.h"

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

void P_DrawSDL(SDL_Surface* surface, Point* p, Uint32 color)
{
    //Lock surface
    if( SDL_MUSTLOCK( surface ) )
    {
        SDL_LockSurface( surface );
    }
    Uint32* pixels = (Uint32*)surface->pixels;
    
    for (int x = p->x - POINT_RADIUS; x < p->x + POINT_RADIUS; x++)
    {
        for (int y = p->y - POINT_RADIUS; y < p->y + POINT_RADIUS; y++)
        {
            if(x >= 0 && x < surface->w && y >= 0 && y < surface->h)
            {
                pixels[y * surface->w + x] = color;
            }
        }
    }

    //Unlock surface
    if( SDL_MUSTLOCK( surface ) )
    {
        SDL_UnlockSurface( surface );
    }
}


void P_Free(Point* point)
{
    free(point);
}