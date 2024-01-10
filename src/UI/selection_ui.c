#include "UI/selection_ui.h"
#include "UI/solver_ui.h"
#include "UI/training_ui.h"

/*
*   Function: load_css
*   ------------------
*   Loads the CSS file.
*/
void load_css()
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

/*
*   Function: on_solver_button_clicked
*   ------------------
*   Callback function for the solver button.
*/
static void on_solver_button_clicked(GtkWidget *widget, gpointer data)
{
    // Destroy the selection window
    gtk_widget_destroy(GTK_WIDGET(data));

    // Launch the Solver window
    show_solver_window((GtkApplication *)g_application_get_default());
}

/*
*   Function: on_training_button_clicked
*   ------------------
*   Callback function for the training button.
*/
static void on_training_button_clicked(GtkWidget *widget, gpointer data)
{
    // Destroy the selection window
    gtk_widget_destroy(GTK_WIDGET(data));

    // Launch the Training window
    show_training_window((GtkApplication *)g_application_get_default());
}

/*
*   Function: on_window_delete_event
*   ------------------
*   Callback function for the delete-event signal.
*/
static gboolean on_window_delete_event(GtkWidget *widget,
GdkEvent *event, gpointer data)
{
    // Destroy the selection window
    gtk_widget_destroy(GTK_WIDGET(widget));

    // Exit the program
    return FALSE;
}

/*
*   Function: show_selection_window
*   ------------------
*   Initializes the main window and calls the other functions.
*/
void show_selection_window(GtkApplication *app)
{
    GtkWidget *window;
    GtkWidget *button_box;
    GtkWidget *solver_button;
    GtkWidget *training_button;

    // Load CSS
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "src/UI/style.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
    GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Create a new window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Selection Window");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 100);

    // Connect the delete-event signal to the handler
    g_signal_connect(window, "delete-event", 
    G_CALLBACK(on_window_delete_event), NULL);

    // Create a container
    button_box = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
    gtk_container_add(GTK_CONTAINER(window), button_box);

    // Solver button
    solver_button = gtk_button_new_with_label("Solver");
    g_signal_connect(solver_button, "clicked", 
    G_CALLBACK(on_solver_button_clicked), window);
    gtk_container_add(GTK_CONTAINER(button_box), solver_button);

    // Training button
    training_button = gtk_button_new_with_label("Model Training");
    g_signal_connect(training_button, "clicked", 
    G_CALLBACK(on_training_button_clicked), window);
    gtk_container_add(GTK_CONTAINER(button_box), training_button);

    gtk_widget_show_all(window);
}
