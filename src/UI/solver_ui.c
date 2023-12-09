#include <gtk/gtk.h>
#include <string.h>
#include "solver_ui.h"
#include "../sudokuSolver/Solver.h"
#include "../Sudoku.h"
#include "../imageProcessing/Hough.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <cairo.h>

// Global variables
GtkWidget 
*window,
*hbox,
*vbox,
*nav_hbox,
*image,
*grid;

GtkWidget
*btn_open,
*btn_preproc,
*btn_treatment,
*btn_nn,
*btn_solve,
*btn_next_image,
*btn_previous_image,
*btn_original_image,
*btn_quit;

int part = 0;
int i = 0;

GtkWidget *sudoku_labels[9][9];

int original_values[9][9];
int sudoku_grid[9][9];

char 
*original_filename,
*filename;

const size_t preproc_count = 4;
const size_t treatment_count = 245;

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
        g_free(original_filename);
        filename = NULL;
        original_filename = NULL;
    }
    

    // The function must return FALSE to allow the window to be destroyed,
    // or TRUE to prevent it.
    return !should_close; 
}

/*
*   Function: save_sudoku_grid_to_image
*   ---------------------
*   Saves the solved sudoku grid to an image.
*/
void save_sudoku_grid_to_image()
{
    int grid_size = 9;
    int cell_size = 50; // Size of each cell in the grid
    int image_size = grid_size * cell_size;

    // Create a Cairo surface to draw on
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, image_size, image_size);
    cairo_t *cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 1, 1, 1); // White color
    cairo_paint(cr);

    // Set font size
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 30);

    // Draw 3x3 subgrid rectangles
    int subgrid_size = 3 * cell_size; // Size of each 3x3 subgrid rectangle
    cairo_set_source_rgb(cr, 0, 0, 0); // Black color for subgrid borders
    cairo_set_line_width(cr, 3);
    for (int i = 0; i < grid_size; i += 3)
    {
        for (int j = 0; j < grid_size; j += 3)
        {
            cairo_rectangle(cr, j * cell_size, i * cell_size, subgrid_size, subgrid_size);
            cairo_stroke(cr);
        }
    }

    // Draw the Sudoku grid and numbers
    for (int i = 0; i < grid_size; i++)
    {
        for (int j = 0; j < grid_size; j++)
        {
            // Calculate cell position
            int x = j * cell_size;
            int y = i * cell_size;

            // Draw cell border
            cairo_set_source_rgb(cr, 0, 0, 0);
            cairo_set_line_width(cr, 1);

            cairo_rectangle(cr, x, y, cell_size, cell_size);
            cairo_stroke(cr);

            if (sudoku_grid[i][j] == original_values[i][j])
            {
                cairo_set_source_rgb(cr, 0, 0, 0);
            }
            else
            {
                cairo_set_source_rgb(cr, 0, 0, 1);
            }

            // Draw number
            char num_text[2];
            snprintf(num_text, sizeof(num_text), "%d", sudoku_grid[i][j]);
            cairo_move_to(cr, x + (cell_size / 2 - 10), 
            y + (cell_size / 2 + 10));
            cairo_show_text(cr, num_text);
        }
    }

    // Save the surface to a PNG file
    cairo_surface_write_to_png(surface, "solved_sudoku_grid.png");

    // Clean up
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}


/*
*   Function: thread
*   ---------------------
*   Thread to execute the image processing.
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

    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(hbox));
    for(iter = children; iter != NULL; iter = g_list_next(iter))
    {
        GtkWidget *widget = GTK_WIDGET(iter->data);
        if (GTK_IS_GRID(widget))
        {
            gtk_widget_destroy(widget);
        }
    }
    g_list_free(children);

    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

    dialog = gtk_file_chooser_dialog_new("Open File", NULL,
                                         action,
                                         "_Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         "_Open",
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);

    gtk_widget_set_name(dialog, "solver_error_dialog");
    // Set a specific folder to show when dialog opens
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),
                                        "./images/SudokuImages");

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) 
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        original_filename = g_strdup(filename);
        
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

            gtk_widget_set_name(error_dialog, "solver_error_dialog");
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
*   Function: on_original_image_window_destroy
*   ---------------------
*   Called when the user closes the original image window.
*/
void on_original_image_window_destroy(GtkWidget *widget, gpointer data)
{
    GtkButton *button = GTK_BUTTON(data);
    gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
}

/*
*   Function: solver_button_original_image
*   ---------------------
*   Shows the original image of the sudoku puzzle.
*/
void solver_button_original_image(GtkButton *button, gpointer user_data)
{
    GtkWidget *original_image_window;
    GtkWidget *image_widget;
    GdkPixbuf *pixbuf, *scaled_pixbuf;
    GError *error = NULL;

    // Create a new top-level window
    original_image_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(original_image_window), 
    "Original Sudoku Image");

    // Load the original image into a GdkPixbuf
    pixbuf = gdk_pixbuf_new_from_file(original_filename, &error);

    if (error)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return; // Exit the function if there's an error
    }

    // Change sensitivity of the buttons
    gtk_widget_set_sensitive(btn_original_image, FALSE);

    // Connect the destroy signal to the window
    g_signal_connect(G_OBJECT(original_image_window), "destroy",
    G_CALLBACK(on_original_image_window_destroy), button);

    // Scale the image to a width of 500 pixels
    // while maintaining the aspect ratio
    int original_width = gdk_pixbuf_get_width(pixbuf);
    int original_height = gdk_pixbuf_get_height(pixbuf);
    int scaled_height = (500 * original_height) / original_width;
    scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, 500, scaled_height, 
    GDK_INTERP_BILINEAR);

    // Create an image widget from the scaled GdkPixbuf
    image_widget = gtk_image_new_from_pixbuf(scaled_pixbuf);

    // Add the image to the window
    gtk_container_add(GTK_CONTAINER(original_image_window), image_widget);

    // Show the window with the image
    gtk_widget_show_all(original_image_window);

    // Free the memory
    g_object_unref(pixbuf);
    g_object_unref(scaled_pixbuf);
}

/*
*   Function: on_sudoku_button_clicked
*   ---------------------
*   Called when the user clicks on a button in the sudoku grid.
*/
void on_sudoku_button_clicked(GtkButton *button, gpointer user_data)
{
    int position = GPOINTER_TO_INT(user_data);
    int row = position / 9;
    int col = position % 9;

    // Create a dialog to ask the user for a new number
    GtkWidget *dialog, *content_area, *entry;
    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_dialog_new_with_buttons("Enter new number",
                                         GTK_WINDOW(window),
                                         flags,
                                         "_OK",
                                         GTK_RESPONSE_ACCEPT,
                                         "_Cancel",
                                         GTK_RESPONSE_REJECT,
                                         NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    entry = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(entry), 1);
    gtk_box_pack_start(GTK_BOX(content_area), entry, TRUE, TRUE, 0);
    gtk_widget_show_all(dialog);

    // Run the dialog and wait for the user response
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));

    if (result == GTK_RESPONSE_ACCEPT)
    {
        const char *text = gtk_entry_get_text(GTK_ENTRY(entry));
        if (text[0] >= '1' && text[0] <= '9')
        {
            // Update the button label and sudoku grid
            gtk_button_set_label(button, text);
            sudoku_grid[row][col] = text[0] - '0'; // Convert char to int

            // Change the name of the button
            gtk_widget_set_name(GTK_WIDGET(button), "original_number");
        }
        else if (text[0] == '0') // Check if input is 0
        {
            // Update the button label and sudoku grid
            gtk_button_set_label(button, text);
            sudoku_grid[row][col] = 0;

            // Change the name of the button
            gtk_widget_set_name(GTK_WIDGET(button), "empty_cell");
        }
    }

    gtk_widget_destroy(dialog);
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
            gtk_widget_set_sensitive(GTK_WIDGET(btn_previous_image), TRUE);
        }
        if (i == preproc_count)
        {
            gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
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
            gtk_widget_set_sensitive(GTK_WIDGET(btn_previous_image), TRUE);
        }
        if (i == treatment_count)
        {
            gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
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
            gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
        }
        if (i == preproc_count - 1)
        {
            gtk_widget_set_sensitive(GTK_WIDGET(btn_next_image), TRUE);
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
            gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
        }
        if (i == treatment_count - 1)
        {
            gtk_widget_set_sensitive(GTK_WIDGET(btn_next_image), TRUE);
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
    gtk_widget_set_sensitive(btn_original_image, TRUE);

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

    // Remove the image
    gtk_widget_destroy(image);

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            char buffer[3];
            snprintf(buffer, sizeof(buffer), "%d", sudoku_grid[i][j]);
            
            GtkWidget *btn = gtk_button_new_with_label(buffer);
            gtk_widget_set_sensitive(btn, TRUE);
            gtk_grid_attach(GTK_GRID(grid), btn, j, i, 1, 1);

            g_signal_connect(btn, "clicked", 
            G_CALLBACK(on_sudoku_button_clicked),
            GINT_TO_POINTER(i * 9 + j));
            
            if (sudoku_grid[i][j] != 0)
            {
                gtk_widget_set_name(btn, "original_number");
                original_values[i][j] = sudoku_grid[i][j];
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
    if (solver(sudoku_grid) == 0)
    {
        // Show a dialog to inform the user 
        // that the sudoku puzzle is unsolvable
        GtkWidget *dialog = gtk_message_dialog_new(
            NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_CLOSE,
            "Sudoku puzzle is unsolvable in the actual state.");
        
        gtk_widget_set_name(dialog, "solver_error_dialog");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        return;
    }

    for (int i = 0; i < 9; i++) 
    {
        for (int j = 0; j < 9; j++)
        {
            char buffer[3];
            snprintf(buffer, sizeof(buffer), "%d", sudoku_grid[i][j]);

            // Set the button to non clickable
            gtk_widget_set_sensitive(sudoku_labels[i][j], FALSE);

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

    gtk_widget_set_sensitive(btn_open, TRUE);

    save_sudoku_grid_to_image();
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
    btn_next_image = gtk_button_new_with_label("→");
    btn_previous_image = gtk_button_new_with_label("←");
    btn_original_image = gtk_button_new_with_label("Original Image");
    btn_quit = gtk_button_new_with_label("Quit");
    gtk_box_pack_start(GTK_BOX(vbox), btn_open, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), btn_preproc, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), btn_treatment, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), btn_nn, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), btn_solve, FALSE, FALSE, 5);

    // Create a horizontal box for the navigation buttons
    nav_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), nav_hbox, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(nav_hbox), btn_previous_image, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(nav_hbox), btn_next_image, TRUE, TRUE, 5);

    gtk_box_pack_start(GTK_BOX(vbox), btn_original_image, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), btn_quit, FALSE, FALSE, 5);

    // Set sensitivity of buttons
    gtk_widget_set_sensitive(btn_open, TRUE);
    gtk_widget_set_sensitive(btn_preproc, FALSE);
    gtk_widget_set_sensitive(btn_treatment, FALSE);
    gtk_widget_set_sensitive(btn_nn, FALSE);
    gtk_widget_set_sensitive(btn_solve, FALSE);
    gtk_widget_set_sensitive(btn_next_image, FALSE);
    gtk_widget_set_sensitive(btn_original_image, FALSE);
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
    g_signal_connect(btn_original_image, "clicked",
    G_CALLBACK(solver_button_original_image), NULL);
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
*   Function: show_solver_window
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