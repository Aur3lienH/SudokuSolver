#include <stdlib.h>
#include <math.h>
#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "../geometry/Square.h"
#include "../deepLearning/Matrix.h"

#define DIST(x1,y1,x2,y2) sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1))

float normalize(float min, float max, float val)
{
    return (val-min)/(max-min);
}

SDL_Surface* load_image(const char* path)
{
    /*
    LOADS AN IMAGE ONTO A SURFACE
    */
	SDL_Surface* surface = IMG_Load(path);
	SDL_Surface* converted = 
                SDL_ConvertSurfaceFormat(surface,SDL_PIXELFORMAT_RGB888,0);
	SDL_FreeSurface(surface);
	return converted;
}

Uint32 get_pixel(SDL_Surface *surface, int x, int y) 
{
    /*
    GET THE PIXEL OF A SURFACE USING COORDS (X,Y)
    */

    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    switch (bpp) {
        case 1:
            return *p;
        case 2:
            return *(Uint16 *)p;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
        case 4:
            return *(Uint32 *)p;
        default:
            return 0;
    }
}

int save_image(SDL_Surface *surface, const char *file)
{
    /*
    SAVE SURFACE AS A .JPG
    */

    if(IMG_SaveJPG(surface, file, 100))
    {
        return 1;
    }
    return 0; //fail to save
}

void setPixel(SDL_Surface *surface, 
            Uint8 r, Uint8 g, Uint8 b, Uint8 a, size_t x, size_t y)
{
    /*
    SET THE COLOR OF A PIXEL ON A SURFACE USING (r,g,b) & (x,y)
    */

    Uint32 *pixels = surface->pixels; 
    Uint32 color = SDL_MapRGBA(surface->format, r, g, b, a);
    pixels[y * surface->w + x] = color;
}

void surface_DrawLine(SDL_Surface *surface, int x1, int y1, int x2, int y2)
{
    /*
    MANUALY RENDER A LINE ON A SURFACE
    */

    SDL_LockSurface(surface);

    int w = surface->w, h = surface->h;

    //DRAW LINE BETWEEN TWO POINTS USING BRENSENHAM LINE ALGORITHM
    int dx = abs(x2-x1), sx = x1<x2 ? 1 : -1;
    int dy = abs(y2-y1), sy = y1<y2 ? 1 : -1;

    int err = (dx>dy ? dx : -dy)/2, e2;

    int i = 987654;
    int j = 0;
    //I AND J ARE TO AVOID ANY INFINITE LOOP BUT SHOULD NEVER OCCUR
    for(j =0; j<i;j++)
    {
        //CHECK IF POSITION IS INSIDE SURFACE
        if(x1 >= 0 && x1 < w && y1 >= 0 && y1 < h)
            setPixel(surface, 255, 0, 0, 255, x1,y1);
        if (x1==x2 && y1==y2) break;
        e2 = err;
        if (e2 >-dx) { err -= dy; x1 += sx; }
        if (e2 < dy) { err += dx; y1 += sy; }
    }

    //if(j==i){printf("Line drawing overflow\n");}

    SDL_UnlockSurface(surface);
}

void render_accumulator(unsigned int **accumulator_array, 
                        double width, double height, double min, double max)
{
    /*
    EXPORT THE ACCUMULATOR ON A NEW SURFACE AND EXPORT THE SURFACE
    */

    SDL_Surface* surface = SDL_CreateRGBSurface
                                (0, width, height, 32,
                                0, 0, 0, 0);
    SDL_LockSurface(surface); 
    int w_w = surface->w, w_h = surface->h;

    //SET ALL PIXELS TO BLACK
    for(int y = 0; y<w_h; y++)
	{
		for(int x = 0; x<w_w; x++)
		{
            setPixel(surface, 0, 0, 0, 255, x, y);
        }
    }

    //LOOP TO DRAW THE ACCUMULATOR
    for(size_t y = 0; y<height; y++)
	{
		for(size_t x = 0; x<width/2; x++)
		{
            // SET THE COLOR OF THE DRAWING OPERATION 
            // TO NORMALIZED VALUE OF ACCUMULATOR 
            // (SHADE OF GREY BASED OF VALUE)
            Uint8 col = 
                    (Uint8)(255*normalize(min,max, accumulator_array[y][x]));

            float norm_w = normalize(0, width/2, x);
            float norm_h = normalize(0, height, y);

            setPixel(surface, col, col, col, 255, w_w*norm_w, w_h*norm_h);
        }
    }

    //AVERAGE ALL REMAINING PIXELS TO FILL GAPS (GET A BETTER IMAGE)
    for(int y = 1; y<w_h-1; y++)
	{
		for(int x = 1; x<w_w-1; x++)
		{
            Uint32 inputPix = get_pixel(surface, x, y);
            Uint8 r, g, b;
            SDL_GetRGB(inputPix, surface->format, &r, &g, &b);
            if(r==0 && g==0 && b==0)
            {
                Uint8 r1, g1, b1;
                Uint32 p1 = get_pixel(surface, x-1, y);
                SDL_GetRGB(p1, surface->format, &r1, &g1, &b1);

                Uint8 r2, g2, b2;
                Uint32 p2 = get_pixel(surface, x+1, y);
                SDL_GetRGB(p2, surface->format, &r2, &g2, &b2);

                Uint8 ra = r1/2 + r2/2, ga = g1/2 + g2/2, ba = b1/2 + b2/2;

                setPixel(surface, ra, ga, ba, 255, x, y);
            }
        }
    } 

    SDL_UnlockSurface(surface);

    save_image(surface, "images/export/step_1.jpg");
}

SDL_Surface** extract_Squares(SDL_Surface *surface, Square s)
{
    /*
    GIVEN A SURFACE THAT IS JUST THE GRID, EXPORT ALL SquareS SEPARATLY
    */

    // FIRST, FIND LEFT-MOST CORNER
    // TO DO THIS, FIN THE PAIR OF COORDINATES WITH THE LOWEST RATIO x/y
    int xOff = s.points[0].x,yOff = s.points[0].x;
    if( DIST(0,0, s.points[1].x, s.points[1].y) < DIST(0,0, xOff, yOff))
    {xOff = s.points[1].x; yOff = s.points[1].y;}
    if( DIST(0,0, s.points[2].x, s.points[2].y) < DIST(0,0, xOff, yOff))
    {xOff = s.points[2].x; yOff = s.points[2].y;}
    if( DIST(0,0, s.points[3].x, s.points[3].y) < DIST(0,0, xOff, yOff))
    {xOff = s.points[3].x; yOff = s.points[3].y;}

    SDL_LockSurface(surface);

    SDL_PixelFormat *fmt = surface->format;

    // GET SIZE OF A SINGLE CELL
    int cell_size = DIST(s.points[0].x,s.points[0].y,
                        s.points[1].x,s.points[1].y)/9;

    //printf("%i\n", cell_size);

    int cpt = 0;

    SDL_Surface** Squares = (SDL_Surface**)malloc(81*sizeof(SDL_Surface*));

    // LOOP THROUGH ALL CELLS 
    for(int i = 0; i<9; i++)
    {
        for(int j = 0; j<9; j++)
        {
            SDL_Surface* cell = SDL_CreateRGBSurface
                                (0, cell_size, cell_size, 32,
                                0, 0, 0, 0);
            SDL_LockSurface(cell);
            //SET PIXELS
            for(int y = 0; y<cell_size; y++)
            {
                for(int x = 0; x<cell_size; x++)
                {
                    Uint32 pixel = get_pixel(surface, xOff + j*cell_size+x, 
                                                    yOff + i*cell_size+y);
                    Uint8 r, g, b;
                    SDL_GetRGB(pixel, fmt, &r, &g, &b);

                    setPixel(cell, r, g, b, 255, x, y);
                }
            }
            SDL_UnlockSurface(cell);

            Squares[cpt] = cell;

            char filename[100];
            snprintf(filename, sizeof(filename), "images/cells/cell_%d.jpg", cpt);
            cpt++;
            save_image(cell, filename);
        }
    }

    SDL_UnlockSurface(surface);
    return Squares;
} 

/*-------------------------------------------------------------------------//
                FROM THIS POINT, ALL FUNCTIONS ARE TO PERFORM 
                        PERSPECTIVE TRANSFORMATION
//-------------------------------------------------------------------------*/

void print_matrix(char s[], double m[], size_t rows, size_t cols)
{
	printf("%s = \n", s);
	for(size_t r = 0; r<rows; r++)
	{
		for(size_t c = 0; c<cols; c++)
		{
			size_t i = r * cols + c;
			printf("%14f", m[i]);
		}
		printf("\n");
	}	
}

int convert_matindex(int i,int j, int width)
{
    return i * width + j;
}
 
void copy_matrix(double m[], size_t rows, size_t cols, double r[])
{
	for(size_t _r = 0; _r<rows; _r++)
        {
            for(size_t _c = 0; _c<cols; _c++)
            {
                size_t i = _c * rows + _r;
			    r[i] = m[i];
            }
        }   
}

void transpose(double m[], size_t rows, size_t cols, double r[])
{
	for(size_t _r = 0; _r<rows; _r++)
        {
            for(size_t _c = 0; _c<cols; _c++)
            {
                size_t i = _r * cols + _c;
			    r[_c * rows + _r] = m[i];
            }
        }   
}

void invert(double m[], size_t rows, size_t cols, double r[]) 
{
    // First, let's check if the matrix is Square
    if (rows != cols) {
        return; // Inversion is not possible
    }

    size_t n = rows; // The order of the Square matrix
    double epsilon = 1e-10; // For checking if a value is effectively zero

    // Temporary matrix to perform operations, 
    //initializing it as the identity matrix
    double temp[n * n];
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            temp[i * n + j] = (i == j) ? 1.0 : 0.0; // Identity matrix
        }
    }

    // Temporary array to hold the current row during the calculations
    double scaleRow[n];

    // Begin the process of row reduction or Gauss-Jordan elimination
    for (size_t i = 0; i < n; ++i) {
        // Find the pivot element 
        //(we choose the absolute largest for stability)
        size_t pivot = i;
        for (size_t j = i; j < n; ++j) {
            if (fabs(m[j * n + i]) > fabs(m[pivot * n + i])) {
                pivot = j;
            }
        }

        // Check if the matrix is singular (non-invertible)
        if (fabs(m[pivot * n + i]) < epsilon) {
            return; // Cannot invert, would divide by zero in a step
        }

        // If the pivot is not the current row, swap them in both matrices
        if (pivot != i) {
            for (size_t j = 0; j < n; ++j) {
                double tmp = m[i * n + j];
                m[i * n + j] = m[pivot * n + j];
                m[pivot * n + j] = tmp;

                tmp = temp[i * n + j];
                temp[i * n + j] = temp[pivot * n + j];
                temp[pivot * n + j] = tmp;
            }
        }

        // Scale the pivot row to have a pivot of 1
        double pivotValue = m[i * n + i];
        for (size_t j = 0; j < n; ++j) {
            m[i * n + j] /= pivotValue;
            temp[i * n + j] /= pivotValue;
        }

        // Store the scaled pivot row before overwriting it
        for (size_t j = 0; j < n; ++j) {
            scaleRow[j] = m[i * n + j];
        }

        // Eliminate the current column in rows other than the pivot
        for (size_t j = 0; j < n; ++j) {
            if (j == i) continue; // Don't eliminate the pivot row

            double factor = m[j * n + i];
            for (size_t k = 0; k < n; ++k) {
                m[j * n + k] -= scaleRow[k] * factor;
                temp[j * n + k] -= temp[i * n + k] * factor;
            }
        }
    }

    // Copy the inverted matrix into the result
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            r[i * n + j] = temp[i * n + j];
        }
    }
}

void mul(double m1[], double m2[], size_t r1, size_t c1, size_t c2, double r[])
{
	for(size_t i = 0; i < r1; i++) {
        for(size_t j = 0; j < c2; j++) {
            r[i * c2 + j] = 0;
            for(size_t k = 0; k < c1; k++) {
                r[i * c2 + j] += m1[i * c1 + k] * m2[k * c2 + j];
            }
        }
    }
}

double tri_are(Point p1, Point p2, Point p3)
{
    double dist1 = DIST(p1.x, p1.y, p2.x, p2.y); //HAHAHAHAHHAHAHAHHAHAHAHAHHA
    double dist2 = DIST(p2.x, p2.y, p3.x, p3.y); // HUIHUIHOHEUUIIGHHH
    double dist3 = DIST(p3.x, p3.y, p1.x, p1.y);

    // HERON FORUMLA I LOVE HERON THANK YOU FOR THE FORMULA
    // *PIKACHU FACE*
    double semi_p = (dist1 + dist2 + dist3)/2;
    return sqrt(semi_p*(semi_p-dist1)*(semi_p-dist2)*(semi_p - dist3));
}
/*
int isInsideSquare(point p, Square s) 
{
    // GET AREA OF BIG Square
    double s_area = tri_are(s.points[0], s.points[1], s.points[2])
                    + tri_are(s.points[0], s.points[3], s.points[2]);
    
    //GET THE 4 TRIANGLES 
    double tr_area1 = tri_are(p, s.points[0], s.points[1]);
    double tr_area2 = tri_are(p, s.points[1], s.points[2]);
    double tr_area3 = tri_are(p, s.points[2], s.points[3]);
    double tr_area4 = tri_are(p, s.points[3], s.points[0]);

    double total = tr_area1 + tr_area2 + tr_area3 + tr_area4;

    return 1;
}*/

Matrix* TransformPerspective(Matrix* in, Square grid, size_t newWidth)
{
    Matrix* out = M_Create_2D(newWidth, newWidth);
    printf("Transforming perspective...\n");
    printf("new width: %ld\n", newWidth);
    Square from;
    //Top left corner
    from.points[0].x = grid.points[0].x;
    from.points[0].y = grid.points[0].y;
    //Top right corner
    from.points[1].x = grid.points[1].x;
    from.points[1].y = grid.points[1].y;
    //Bottom left corner
    from.points[2].x = grid.points[2].x;
    from.points[2].y = grid.points[2].y;
    //Bottom right corner
    from.points[3].x = grid.points[3].x;
    from.points[3].y = grid.points[3].y;

    // DEFINE OUTPUT CORNER COORDINATES
    Square to;
        //Top left corner
        to.points[0].x = 0;
        to.points[0].y = 0;
        //Top right corner
        to.points[1].x = newWidth;
        to.points[1].y = 0;
        //Bottom left corner
        to.points[2].x = 0;
        to.points[2].y = newWidth;
        //Bottom right corner
        to.points[3].x = newWidth;
        to.points[3].y = newWidth;

    // OTHER WAY TO REPRESENT THE COORDS
    int x1 = from.points[0].x, y1 = from.points[0].y;
    int x2 = from.points[1].x, y2 = from.points[1].y; 
    int x3 = from.points[2].x, y3 = from.points[2].y; 
    int x4 = from.points[3].x, y4 = from.points[3].y;

    int _x1 = to.points[0].x, _y1 = to.points[0].y;
    int _x2 = to.points[1].x, _y2 = to.points[1].y; 
    int _x3 = to.points[2].x, _y3 = to.points[2].y; 
    int _x4 = to.points[3].x, _y4 = to.points[3].y;  


    size_t p_col = 9, p_row = 9; 

    double P[] = 
    {
        -x1, -y1, -1, 0, 0, 0, x1 * _x1, y1 * _x1, _x1,
        0, 0, 0, -x1, -y1, -1, x1 * _y1, y1 * _y1, _y1,

        -x2, -y2, -1, 0, 0, 0, x2 * _x2, y2 * _x2, _x2,
        0, 0, 0, -x2, -y2, -1, x2 * _y2, y2 * _y2, _y2,

        -x3, -y3, -1, 0, 0, 0, x3 * _x3, y3 * _x3, _x3,
        0, 0, 0, -x3, -y3, -1, x3 * _y3, y3 * _y3, _y3,

        -x4, -y4, -1, 0, 0, 0, x4 * _x4, y4 * _x4, _x4,
        0, 0, 0, -x4, -y4, -1, x4 * _y4, y4 * _y4, _y4,

        0, 0, 0, 0, 0, 0, 0, 0, 1
    };

    double H[9];
    double R[] = {0, 0, 0, 0, 0, 0, 0, 0, 1};

    double p_invert[9*9];
    double P_ID[9*9];
    copy_matrix(P, p_row, p_col, P_ID);
    copy_matrix(P, p_row, p_col, p_invert);
    invert(P_ID, p_row, p_col, p_invert);

    mul(p_invert, R, p_row, p_col, 1, H);

    // WE NOW CONSIDER H IS A 3X3 MATRIX
    double h_invert[9];
    double H_ID[3*3];
    copy_matrix(H, 3, 3, H_ID);
    invert(H_ID, 3, 3, h_invert);

    // GET THE AREA OF THE Square BIM BIM BAM BAM
    // I AM GOING INSANE
    // <_>

    for(int y = 0; y<newWidth; y++)
    {
        for(int x = 0; x<newWidth; x++)
        {
            double OUT[] = {0, 0, 1};
            double IN[] = {x, y, 1};
            mul(h_invert, IN, 3, 3, 1, OUT);
            OUT[0] /= OUT[2];
            OUT[1] /= OUT[2];
            size_t x_int = (int)OUT[0];
            size_t y_int = (int)OUT[1];
            printf("x_int: %ld, y_int: %ld\n", x_int, y_int);
            if(x_int >= 0 && x_int < in->cols && y_int>= 0 && y_int < in->rows)
            {
                float val = in->data[y_int * in->cols + x_int];
                out->data[y * out->cols + x] = val;
            }
            else
            {
                M_SetValue(out, x, y, 0);
            }
        }
    }
    return out;
}