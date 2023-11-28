#include <gtk/gtk.h>
#include <string.h>
#include "solver_ui.h"
#include "../sudokuSolver/Solver.h"
#include "../Sudoku.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Global variables
GtkWidget *window;

GtkWidget *hbox;

GtkWidget *vbox;

GtkWidget *image;

GtkWidget
*btn_open,
*btn_preproc,
*btn_treatment,
*btn_nn,
*btn_solve,
*btn_next_image,
*btn_previous_image,
*btn_quit;

int part = 0;
int i = 0;

GtkWidget *sudoku_labels[9][9];

int sudoku_grid[9][9];

char *filename;

/*
*   Function: solver_is_valid_image_file
*   -----------------------------
*   Checks if the file is a valid image file.
*/
gboolean solver_is_valid_image_file(const char *filename)
{
    if (filename == NULL) return FALSE;

    const char *extension = strrchr(filename, '.');
    if (extension == NULL) return FALSE;

    return g_ascii_strcasecmp(extension, ".png") == 0 ||
           g_ascii_strcasecmp(extension, ".jpg") == 0 ||
           g_ascii_strcasecmp(extension, ".jpeg") == 0 ||
           g_ascii_strcasecmp(extension, ".bmp") == 0;
}

/*
*   Function: solver_show_sudoku_image
*   ---------------------------
*   Shows the image of the sudoku puzzle.
*/
void solver_show_sudoku_image(char *filename)
{
    image = gtk_image_new();
    GdkPixbuf *pixbuf, *scaled_pixbuf;
    GError *error = NULL;
    pixbuf = gdk_pixbuf_new_from_file(filename, &error);

    if (error)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
    }

    // Scale the image to fit the window
    // The height has to be 500px 
    // The width is scaled proportionally
    
    // Get the width and height of the image
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);

    // Scale the image
    scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, 500, 500 * height / width,
    GDK_INTERP_BILINEAR); 
    gtk_image_set_from_pixbuf(GTK_IMAGE(image), scaled_pixbuf);

    // Free the memory
    g_object_unref(pixbuf);
    g_object_unref(scaled_pixbuf);
    
    gtk_box_pack_start(GTK_BOX(hbox), image, TRUE, TRUE, 0);
    gtk_widget_set_name(image, "sudoku_image");
    gtk_widget_show(image);
    gtk_widget_queue_draw(GTK_WIDGET(hbox));

    // Center the image
    gtk_widget_set_halign(image, GTK_ALIGN_CENTER);
}

/*
*   Function: on_delete_event
*   -------------------------
*   Called when the user tries to close the window.
*/
static gboolean on_delete_event(GtkWidget *widget)
{
    // Ask users if they are sure they want to quit
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new(GTK_WINDOW(widget),
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_QUESTION,
                                    GTK_BUTTONS_YES_NO,
                                    "Are you sure you want to quit?");
    gtk_window_set_title(GTK_WINDOW(dialog), "Confirm");
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));

    gboolean should_close = FALSE;
    switch (result)
    {
        case GTK_RESPONSE_YES:
            should_close = TRUE; // Permit the window to close
            break;
        default:
            should_close = FALSE; // Prevent the window from closing
            break;
    }

    gtk_widget_destroy(dialog);

    if (should_close) // Free the memory
    {
        g_free(filename);
        filename = NULL;
    }
    

    // The function must return FALSE to allow the window to be destroyed,
    // or TRUE to prevent it.
    return !should_close; 
}

/*
*   Function: solver_button_open
*   ---------------------
*   Opens a file chooser dialog and shows the image of the sudoku puzzle.
*/
void* thread(void* arg)
{
    char* filename = (char*)arg;
    int** sudokuValues = ImageToSudoku(filename);
    for (size_t i = 0; i < 9; i++)
    {
        for (size_t j = 0; j < 9; j++)
        {
            sudoku_grid[i][j] = sudokuValues[i][j];
        }
    }
    free(sudokuValues);
    return NULL;
}

/*
*   Function: solver_button_open
*   ---------------------
*   Opens a file chooser dialog and shows the image of the sudoku puzzle.
*/
void solver_button_open(GtkButton *button)
{
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

    dialog = gtk_file_chooser_dialog_new("Open File", NULL,
                                         action,
                                         "_Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         "_Open",
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);
        // Set a specific folder to show when dialog opens
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),
                                        "./images/SudokuImages");

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) 
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        
        // Check the file type here before proceeding
        if (!solver_is_valid_image_file(filename))
        {
            gtk_widget_destroy(dialog);

            // If it's not a valid image, show an error dialog
            GtkWidget *error_dialog = gtk_message_dialog_new(
                NULL,
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_CLOSE,
                "Invalid file type. Please select a valid image file.");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);

            // Let the user try again
            gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
        }
        else
        {
            // If it's a valid image, proceed with your original logic
            gtk_widget_destroy(dialog);

            // Show the image
            solver_show_sudoku_image(filename);

            // Change the sensitivity of the buttons
            gtk_widget_set_sensitive(btn_preproc, TRUE);

            //Execute the thread
            pthread_t tid;
            pthread_create(&tid,NULL,thread,filename);
        }
    }
    else 
    {
        gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
        gtk_widget_destroy(dialog);
    }
}

/*
*   Function: solver_button_next_image
*   ---------------------------
*   Shows the next image of the sudoku puzzle.
*/
void solver_button_next_image(GtkButton* button)
{
    i++;

    if (part == 0)
    {
        if (i == 1)
        {
            gtk_widget_set_sensitive(GTK_BUTTON(btn_previous_image), TRUE);
        }
        if (i == 7)
        {
            gtk_widget_set_sensitive(GTK_BUTTON(button), FALSE);
        }
        // Show next image
        gtk_widget_destroy(image);
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "./images/export/preproc_%d.jpg", i);
        while (gtk_events_pending())
        {
            gtk_main_iteration();
        }
        solver_show_sudoku_image(buffer);
    }
    else if (part == 1)
    {
        if (i == 2)
        {
            gtk_widget_set_sensitive(GTK_BUTTON(btn_previous_image), TRUE);
        }
        if (i == 6)
        {
            gtk_widget_set_sensitive(GTK_BUTTON(button), FALSE);
        }
        // Show next image
        gtk_widget_destroy(image);
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "./images/export/step_%d.jpg", i);
        while (gtk_events_pending())
        {
            gtk_main_iteration();
        }
        solver_show_sudoku_image(buffer);
    }
}

/*
*   Function: solver_button_previous_image
*   ---------------------------
*   Shows the previous image of the sudoku puzzle.
*/
void solver_button_previous_image(GtkButton* button)
{
    i--;

    if (part == 0)
    {
        if (i == 0)
        {
            gtk_widget_set_sensitive(GTK_BUTTON(button), FALSE);
        }
        if (i == 6)
        {
            gtk_widget_set_sensitive(GTK_BUTTON(btn_next_image), TRUE);
        }
        // Show previous image
        gtk_widget_destroy(image);
        while (gtk_events_pending())
        {
            gtk_main_iteration();
        }
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "./images/export/preproc_%d.jpg", i);
        while (gtk_events_pending())
        {
            gtk_main_iteration();
        }
        solver_show_sudoku_image(buffer);
        while (gtk_events_pending())
        {
            gtk_main_iteration();
        }
    }
    else if (part == 1)
    {
        if (i == 1)
        {
            gtk_widget_set_sensitive(GTK_BUTTON(button), FALSE);
        }
        if (i == 5)
        {
            gtk_widget_set_sensitive(GTK_BUTTON(btn_next_image), TRUE);
        }
        // Show previous image
        gtk_widget_destroy(image);
        while (gtk_events_pending())
        {
            gtk_main_iteration();
        }
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "./images/export/step_%d.jpg", i);
        while (gtk_events_pending())
        {
            gtk_main_iteration();
        }
        solver_show_sudoku_image(buffer);
        while (gtk_events_pending())
        {
            gtk_main_iteration();
        }
    }
}

/*
*   Function: solver_button_preproc
*   ---------------------
*   Shows the image after pre-processing.
*/
void solver_button_preproc(GtkButton *button)
{
    part = 0;
    i = 0;

    gtk_widget_set_sensitive(btn_open, FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

    gtk_widget_set_sensitive(btn_next_image, TRUE);
    gtk_widget_set_sensitive(btn_previous_image, FALSE);

    // Show first image
    gtk_widget_destroy(image);
    while (gtk_events_pending())
    {
        gtk_main_iteration();
    }
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "./images/export/preproc_%d.jpg", i);
    while (gtk_events_pending())
    {
        gtk_main_iteration();
    }
    solver_show_sudoku_image(buffer);
    while (gtk_events_pending())
    {
        gtk_main_iteration();
    }

    // Change the sensitivity of the button
    gtk_widget_set_sensitive(btn_treatment, TRUE);
}

/*
*   Function: solver_button_treatment
*   ---------------------
*   Shows the image after treatment.
*/
void solver_button_treatment(GtkButton *button)
{
    part = 1;
    i = 1;

    // Show first image
    gtk_widget_destroy(image);
    while (gtk_events_pending())
    {
        gtk_main_iteration();
    }
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "./images/export/step_%d.jpg", i);
    while (gtk_events_pending())
    {
        gtk_main_iteration();
    }
    solver_show_sudoku_image(buffer);
    while (gtk_events_pending())
    {
        gtk_main_iteration();
    }

    gtk_widget_set_sensitive(btn_open, FALSE);
    gtk_widget_set_sensitive(btn_preproc, FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

    gtk_widget_set_sensitive(btn_next_image, TRUE);
    gtk_widget_set_sensitive(btn_previous_image, FALSE);

    // Change the sensitivity of the button
    gtk_widget_set_sensitive(btn_nn, TRUE);
}

/*
*   Function: solver_button_nn
*   ---------------------
*   Shows the numbers of the sudoku puzzle.
*/
void solver_button_nn(GtkButton *button)
{
    gtk_widget_set_sensitive(btn_next_image, FALSE);
    gtk_widget_set_sensitive(btn_previous_image, FALSE);

    GtkWidget *grid = gtk_grid_new();

    gtk_grid_set_row_spacing(GTK_GRID(grid), 3);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 3);

    // Load grid
    //load_grid("grid_00", sudoku_grid);

    // Remove the image
    gtk_widget_destroy(image);

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            char buffer[3];
            snprintf(buffer, sizeof(buffer), "%d", sudoku_grid[i][j]);
            
            GtkWidget *btn = gtk_button_new_with_label(buffer);
            gtk_widget_set_sensitive(btn, FALSE); // For a static look
            gtk_grid_attach(GTK_GRID(grid), btn, j, i, 1, 1);
            
            if (sudoku_grid[i][j] != 0)
            {
                gtk_widget_set_name(btn, "original_number");
            }
            else
            {
                gtk_widget_set_name(btn, "empty_cell");
            }
            
            sudoku_labels[i][j] = btn; // Storing the button reference
        }
    }

    gtk_box_pack_start(GTK_BOX(hbox), grid, TRUE, TRUE, 0);
    
    // Center the grid
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    gtk_widget_show_all(hbox);

    // Change the sensitivity of the buttons
    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
    gtk_widget_set_sensitive(btn_solve, TRUE);
}

/*
*   Function: solver_button_solve
*   ---------------------
*   Solves the sudoku puzzle.
*/
void solver_button_solve(GtkButton *button)
{
    solver(sudoku_grid);

    for (int i = 0; i < 9; i++) 
    {
        for (int j = 0; j < 9; j++)
        {
            char buffer[3];
            snprintf(buffer, sizeof(buffer), "%d", sudoku_grid[i][j]);

            // Update the numbers after solving
            if (sudoku_grid[i][j] != 0 && 
            strcmp(gtk_widget_get_name(sudoku_labels[i][j]), 
            "original_number"))
            {
                gtk_button_set_label(GTK_BUTTON(sudoku_labels[i][j]), buffer);
                gtk_widget_set_name(sudoku_labels[i][j], "new_number");
            }
        }
    }

    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
    gtk_widget_show_all(hbox);
}

/*
*   Function: solver_button_quit
*   ---------------------
*   Quits the program.
*/
void solver_button_quit()
{
    g_free(filename);
    filename = NULL; // Reset the filename

    // Quit the program
    gtk_widget_destroy(window);
}

/*
*   Function: solver_buttons_init
*   ----------------------
*   Initializes the buttons.
*/
void solver_buttons_init(GtkWidget *vbox)
{
    // Add buttons to vbox
    btn_open = gtk_button_new_with_label("Open File");
    btn_preproc = gtk_button_new_with_label("Pre-Process");
    btn_treatment = gtk_button_new_with_label("Treatment");
    btn_nn = gtk_button_new_with_label("Neural Network");
    btn_solve = gtk_button_new_with_label("Solve");
    btn_next_image = gtk_button_new_with_label("=>");
    btn_previous_image = gtk_button_new_with_label("<=");
    btn_quit = gtk_button_new_with_label("Quit");
    gtk_box_pack_start(GTK_BOX(vbox), btn_open, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), btn_preproc, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), btn_treatment, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), btn_nn, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), btn_solve, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), btn_next_image, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), btn_previous_image, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), btn_quit, FALSE, FALSE, 5);

    // Set sensitivity of buttons
    gtk_widget_set_sensitive(btn_open, TRUE);
    gtk_widget_set_sensitive(btn_preproc, FALSE);
    gtk_widget_set_sensitive(btn_treatment, FALSE);
    gtk_widget_set_sensitive(btn_nn, FALSE);
    gtk_widget_set_sensitive(btn_solve, FALSE);
    gtk_widget_set_sensitive(btn_next_image, FALSE);
    gtk_widget_set_sensitive(btn_previous_image, FALSE);

    // Connect the buttons to the execution of the files
    g_signal_connect(btn_open, "clicked",
    G_CALLBACK(solver_button_open), NULL);
    g_signal_connect(btn_preproc, "clicked",
    G_CALLBACK(solver_button_preproc), NULL);
    g_signal_connect(btn_treatment, "clicked",
    G_CALLBACK(solver_button_treatment), NULL);
    g_signal_connect(btn_nn, "clicked",
    G_CALLBACK(solver_button_nn), NULL);
    g_signal_connect(btn_solve, "clicked",
    G_CALLBACK(solver_button_solve), NULL);
    g_signal_connect(btn_quit, "clicked",
    G_CALLBACK(solver_button_quit), NULL);
    g_signal_connect(btn_next_image, "clicked",
    G_CALLBACK(solver_button_next_image), NULL);
    g_signal_connect(btn_previous_image, "clicked",
    G_CALLBACK(solver_button_previous_image), NULL);
}

/*
*   Function: solver_load_css
*   ------------------
*   Loads the CSS file.
*/
void solver_load_css()
{
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;
    GError *error = NULL;

    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    
    gtk_style_context_add_provider_for_screen(screen,
    GTK_STYLE_PROVIDER(provider),
    GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    gtk_css_provider_load_from_path(provider, "./src/UI/style.css", &error);

    if (error)
    {
        g_printerr("Error loading CSS: %s\n", error->message);
        g_clear_error(&error);
    }

    g_object_unref(provider);
}

/*
*   Function: base
*   --------------
*   Initializes the main window and calls the other functions.
*/
void show_solver_window(GtkApplication *app) 
{
    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Sudoku Solver");
    gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);

    // Connect the delete-event signal to the handler
    g_signal_connect(window, "delete-event", G_CALLBACK(on_delete_event),
    NULL);

    // Create the main horizontal box
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(window), hbox);
    gtk_widget_set_name(hbox, "hbox"); // Set the name of the widget for CSS

    // Create the vertical box for buttons and add to hbox
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);
    gtk_widget_set_size_request(vbox, 240, -1); // 3/10 of 800 is 240
    gtk_widget_set_name(vbox, "vbox"); // Set the name of the widget for CSS

    solver_buttons_init(vbox); // Initialize the buttons.

    // Load the CSS file
    solver_load_css();

    gtk_widget_show_all(window); // Show all widgets inside the main window.
}