#include <cairo.h>
#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include "deepLearning/applications/Mnist.h"
#include "matrix/Matrix.h"
#include "deepLearning/Network.h"
#include "deepLearning/Dataset.h"
#include "imageProcessing/ImageProcessing.h"
#include "imageProcessing/ImageTransformation.h"
#include "tools/StringTools.h"
#include "imageProcessing/DigitProcess.h"
#include "imageProcessing/SdlConverter.h"
#include "imageProcessing/ImageProcessing.h"
#include "tools/FileTools.h"

GtkWidget *darea;
const char* READING_DIGIT_PATH = "datasets/unsure.data";
const char* PADDING = "datasets/padding.p";
float* Pixels = NULL;
const size_t PixelsWidth = 28;
const size_t PixelsCount = PixelsWidth * PixelsWidth;
const float FillingSpeed = 0.5;
Network* network = NULL;
Matrix* input = NULL;
int testingDigits = 0;
FILE* outputFile = NULL;
FILE* readerFile = NULL;
FILE* paddingFILE = NULL;
size_t padding = 0;

/*
*   Function: dd_load_css
*   ------------------
*   Loads the CSS file.
*/
void dd_load_css()
{
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;
    GError *error = NULL;

    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    
    gtk_style_context_add_provider_for_screen(screen, 
    GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    gtk_css_provider_load_from_path(provider, "./src/UI/style.css", &error);

    if (error)
    {
        g_printerr("Error loading CSS: %s\n", error->message);
        g_clear_error(&error);
    }

    g_object_unref(provider);
}


static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    if (event->keyval == GDK_KEY_r || event->keyval == GDK_KEY_R)
    {
        for (size_t i = 0; i < 784; i++)
        {
            Pixels[i] = 0;
        }
        gtk_widget_queue_draw(widget);
    }  
    return FALSE; 
}




static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    guint windowWidth, windowHeight;
    // Get the dimensions of the window.
    windowWidth = gtk_widget_get_allocated_width(widget);
    windowHeight = gtk_widget_get_allocated_height(widget);

    // Calculate the size of the square and position to center it.
    double square_size = (windowWidth / (double)PixelsWidth);
    // The size of the square (e.g., 100x100 pixels).
    
    double x = 0;
    double y = 0;
    for (size_t i = 0; i < PixelsWidth; i++)
    {
        for (size_t j = 0; j < PixelsWidth; j++)
        {
            // Set the color of the square (red in this case).
            double value = Pixels[j + i * PixelsWidth];
            cairo_set_source_rgb(cr, value, value, value); 
            // RGB values are in the range [0, 1].

            // Position the square and set its size.
            cairo_rectangle(cr, x, y, square_size, square_size);

            // Apply the color to the square.
            cairo_fill(cr);
            x += square_size;
        }
        y += square_size;
        x = 0;
    }

    y += 50;
    x = 50;
    char* text = (char*)malloc(sizeof(char) * 2);
    text[1] = '\0';
    const Matrix* res = N_Process(network,input);
        
    for (size_t i = 0; i < 10; i++)
    {
        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_set_font_size(cr, 20.0);
        cairo_move_to(cr, x-6, y+30);
        text[0] = i + '0';
        cairo_show_text(cr, text);

        cairo_set_source_rgb(cr, 1, 1, 1);
        double height = res->data[i] * 40;
        cairo_rectangle(cr, x - square_size / 2, y - height, 
        square_size, height);
        cairo_fill(cr);
        x += 50;
    }
    
    return FALSE; 
    // Returning FALSE means event propagation continues if necessary.
}

void AddColorToPixel(size_t index, double value)
{
    if(index < PixelsCount)
    {
        Pixels[index] = fmin(Pixels[index] + value,1);
        printf("%f\n",Pixels[index]);
    }
}

void AddColorToSurroundingPixels(size_t index)
{
    AddColorToPixel(index,FillingSpeed);
    AddColorToPixel(index + 1,FillingSpeed/2);
    AddColorToPixel(index - 1,FillingSpeed/2);
    AddColorToPixel(index + PixelsWidth,FillingSpeed/2);
    AddColorToPixel(index - PixelsWidth,FillingSpeed/2);
    AddColorToPixel(index + PixelsWidth + 1,FillingSpeed/4);
    AddColorToPixel(index + PixelsWidth - 1,FillingSpeed/4);
    AddColorToPixel(index - PixelsWidth + 1,FillingSpeed/4);
    AddColorToPixel(index - PixelsWidth - 1,FillingSpeed/4);
}

void RemoveColorToPixel(size_t index, double value)
{
    if(index < PixelsCount)
    {
        Pixels[index] = 0;
    }
}

void RemoveColorToSurroundingPixels(size_t index)
{
    RemoveColorToPixel(index,FillingSpeed);
    RemoveColorToPixel(index + 1,FillingSpeed/2);
    RemoveColorToPixel(index - 1,FillingSpeed/2);
    RemoveColorToPixel(index + PixelsWidth,FillingSpeed/2);
    RemoveColorToPixel(index - PixelsWidth,FillingSpeed/2);
    RemoveColorToPixel(index + PixelsWidth + 1,FillingSpeed/4);
    RemoveColorToPixel(index + PixelsWidth - 1,FillingSpeed/4);
    RemoveColorToPixel(index - PixelsWidth + 1,FillingSpeed/4);
    RemoveColorToPixel(index - PixelsWidth - 1,FillingSpeed/4);
}



static gboolean on_mouse_move(GtkWidget *widget, GdkEventMotion *event, gpointer user_data) {
    int x, y;
    GdkModifierType state;

    // In newer versions of GTK, you can use event->x and event->y directly.
    // The following method is maintained for compatibility and detailed state checking.
    gdk_window_get_device_position(event->window, event->device, &x, &y, &state);

    // Check if the left mouse button is pressed
    if (state & GDK_BUTTON1_MASK) {


        guint windowWidth, windowHeight;
        // Get the dimensions of the window.
        windowWidth = gtk_widget_get_allocated_width(widget);
        windowHeight = gtk_widget_get_allocated_height(widget);

        // Calculate the size of the square and position to center it.
        size_t pos = (size_t)(event->x / (windowWidth / (double)PixelsWidth)) + (size_t)(event->y / (windowWidth / (double)PixelsWidth)) * PixelsWidth;
        if(pos < PixelsCount)
        {
            AddColorToSurroundingPixels(pos);
            gtk_widget_queue_draw(widget);
        }
    } 
    
    if(state & GDK_BUTTON3_MASK)
    {
        guint windowWidth, windowHeight;
        // Get the dimensions of the window.
        windowWidth = gtk_widget_get_allocated_width(widget);
        windowHeight = gtk_widget_get_allocated_height(widget);

        // Calculate the size of the square and position to center it.
        size_t pos = (size_t)(event->x / (windowWidth / (double)PixelsWidth)) + (size_t)(event->y / (windowWidth / (double)PixelsWidth)) * PixelsWidth;
        if(pos < PixelsCount)
        {
            RemoveColorToSurroundingPixels(pos);
            gtk_widget_queue_draw(widget);
        }
    }

    // Return FALSE to propagate the event further (if necessary)
    return FALSE;
}

void NextDigit()
{
    if(readerFile == NULL)
    {
        printf("Error opening the file !\n");
        return;
    }
    else
    {
        Matrix* temp = M_Load(readerFile);
        if(temp != NULL)
        {
            M_Copy(temp,input);
            M_Free(temp);
            gtk_widget_queue_draw(darea);
        }
        else
        {
            printf("Error loading the digit !\n");
        }
    }
    padding++;
}

void SetTheNumber(GtkWidget* widget, gpointer data)
{
    size_t number = (size_t)data;
    SaveDigit(input, number, outputFile);
    NextDigit();
}

void validate_button(GtkWidget *widget, gpointer data) {
    //Get the label given by the network
    const Matrix* res = N_Process(network,input);
    size_t maxIndex = 0;
    for (size_t i = 0; i < 10; i++)
    {
        if(res->data[i] > res->data[maxIndex])
        {
            maxIndex = i;
        }
    }
    long byteIndex = (long)(maxIndex);
    SetTheNumber(widget, (gpointer)byteIndex);
}


int DrawDigit(int argc, char *argv[], Network* n) {

    GtkWidget *window;
    Dataset* dataset = NULL;
    Pixels = (float*)malloc(sizeof(float) * PixelsCount);
    for (size_t i = 0; i < PixelsCount; i++)
    {
        Pixels[i] = 0;
    }
    printf("argc : %d\n",argc);
    if(argc >= 1)
    {
        if(CompareStrings(argv[0],"--example"))
        {
            dataset = LoadTestMnist();
            input = M_Create_2D_Data(PixelsCount,1,Pixels);
            M_Copy(dataset->data[0][61],input);
            argc--;
            argv++;
        }
        else if(CompareStrings(argv[0],"--example-sudoku"))
        {
            printf("example sudoku\n");
            dataset = LoadSudokuDigitDataset();
            input = M_Create_2D_Data(PixelsCount,1,Pixels);
            M_Copy(dataset->data[0][1000],input);
            argc--;
            argv++;
        }
        else if (CompareStrings(argv[0],"--test"))
        {
            printf("test\n");
            argc--;
            argv++;
            Matrix* temp;
            SDL_Surface* surface;
            if(argc >= 1)
            {
                surface = IMG_Load(argv[0]);
            }
            else
            {
                surface = IMG_Load("images/cells/cell_0.jpg");
            }
            int isBlank = 0;
            Matrix* matrix = SurfaceTo3DImage(surface);
            temp = MatrixToDigit(matrix,&isBlank);
            input = M_Create_2D_Data(PixelsCount,1,Pixels);
            M_Copy(temp,input);
            M_Free(temp);

        }
        else if(CompareStrings(argv[0], "--filter"))
        {
            testingDigits = 1;
            argc--;
            argv++;
            if(argc >= 1)
            {
                network = N_Load(argv[0]);
            }
            else
            {
                printf("loading best model\n");
                network = LoadBestRecognitionModel();
            }
            input = M_Create_2D_Data(PixelsCount,1,Pixels);

        }
        else{
            input = M_Create_2D_Data(PixelsCount,1,Pixels); 
        }
    }
    else
    {
        input = M_Create_2D_Data(PixelsCount,1,Pixels); 
    }





    if(argc >= 2)
    {
        if(CompareStrings(argv[0],"--rotate"))
        {
            Matrix* temp = M_RotateImage_1DI(input,atof(argv[1]),PixelsWidth);
            M_Copy(temp,input);
            M_Free(temp);
        }
    }
    if(n == NULL)
    {
        network = LoadBestRecognitionModel();
        N_Compile(network,CE_Create());
    }
    else{
        network = n;
        N_Compile(network,CE_Create());
    }



    // Initialize GTK.
    gtk_init(&argc, &argv);

    dd_load_css();

    // Create a new window.
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    // Create a container box.
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box);


    GtkWidget* drawingAreaBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(drawingAreaBox, -1, PixelsWidth * 20 + 250); // Height as padding
    gtk_box_pack_start(GTK_BOX(box), drawingAreaBox, FALSE, FALSE, 0);

    GtkWidget* buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(box), buttonBox, FALSE, FALSE, 0);


    // Create a drawing area.
    darea = gtk_drawing_area_new();
    gtk_box_pack_start(GTK_BOX(drawingAreaBox), darea, TRUE, TRUE,0);

    // Connect the "draw" event to a signal handler.
    g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(on_draw_event), NULL);

    // Connect the "motion-notify-event" event to a signal handler.
    g_signal_connect(G_OBJECT(window), "motion-notify-event", G_CALLBACK(on_mouse_move), NULL);

    // Connect the "destroy" event to exit the application.
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), NULL);

    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    // Set the default size of the window.
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    size_t height = testingDigits ? PixelsWidth * 20 + 200 : PixelsWidth * 20 + 100;
    gtk_window_set_default_size(GTK_WINDOW(window), PixelsWidth * 20, height); // Window size
    gtk_window_set_title(GTK_WINDOW(window), "Digit Drawing");

    if(testingDigits)
    {
        GtkWidget* button = gtk_button_new_with_label("valid");
        g_signal_connect(button, "clicked", G_CALLBACK(validate_button), NULL);
        gtk_box_pack_start(GTK_BOX(buttonBox), button, FALSE, FALSE, 0);
        for (size_t i = 0; i < 10; i++)
        {
            char* label = (char*)malloc(sizeof(char) * 2);
            label[1] = '\0';
            label[0] = i + '0';
            GtkWidget* digitButton = gtk_button_new_with_label(label);

            g_signal_connect(digitButton, "clicked", G_CALLBACK(SetTheNumber), (gpointer)i);
            gtk_box_pack_start(GTK_BOX(buttonBox), digitButton, FALSE,FALSE, 0);


        }

        GtkWidget* skipButton = gtk_button_new_with_label("skip");
        g_signal_connect(skipButton, "clicked", G_CALLBACK(NextDigit), NULL);
        gtk_box_pack_start(GTK_BOX(buttonBox), skipButton, FALSE, FALSE, 0);
        

        outputFile = fopen("datasets/digits.data","awb");
        readerFile = fopen(READING_DIGIT_PATH,"rb");
        if(readerFile == NULL)
        {
            exit(EXIT_FAILURE);
        }
        if(outputFile == NULL)
        {
            exit(EXIT_FAILURE);
        }

        paddingFILE = fopen(PADDING,"rb");
        if(paddingFILE != NULL)
        {
            CheckRead(fread(&padding,sizeof(size_t),1,paddingFILE));
            fclose(paddingFILE);

            size_t index = (M_SaveSizeDim(28,28,1)) * padding;
            fseek(readerFile,index,SEEK_SET);
        }
        
    }

    gtk_widget_set_events(window, gtk_widget_get_events(window) | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);

    // Display the widgets.
    gtk_widget_show_all(window);
    

    // GTK main loop.
    gtk_main();
    if(outputFile != NULL)
    {
        fclose(outputFile);
    }
    paddingFILE = fopen(PADDING,"wb");
    if(paddingFILE != NULL)
    {
        fwrite(&padding,sizeof(size_t),1,paddingFILE);
        fclose(paddingFILE);
    }
    free(Pixels);
    N_Free(network);
    if(dataset != NULL)
    {
        Dataset_Free(dataset);
    }

    return 0;
}
