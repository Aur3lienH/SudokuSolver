#include <gtk/gtk.h>
#include <string.h>
#include "UI/training_ui.h"
#include "deepLearning/Network.h"
#include "deepLearning/applications/Mnist.h"
#include "deepLearning/Loss.h"
#include "UI/DrawDigit.h"

// Global variables
GtkWidget *window_train;

GtkWidget *hbox_train;

GtkWidget *vbox_train;

GtkWidget *progress_bar;

GtkWidget *progress_container;

GtkWidget *progress_text_label;

GtkWidget *accuracy_label;

GtkWidget
*btn_training,
*btn_open_model,
*btn_testing,
*btn_drawdigit,
*btn_quit_train;

char *model;


/*
*   Function: is_valid_model_file
*   -----------------------------
*   Checks if the file is a valid model file.
*/
gboolean is_valid_model_file(const char *model)
{
    if (model == NULL) return FALSE;

    const char *extension = strrchr(model, '.');
    if (extension == NULL) return FALSE;

    return g_ascii_strcasecmp(extension, ".model") == 0;
}

/*
*   Function: training_on_delete_event
*   -------------------------
*   Called when the user tries to close the window.
*/
static gboolean training_on_delete_event(GtkWidget *widget)
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
            g_print("User chose to close the window\n");
            should_close = TRUE; // Permit the window to close
            break;
        default:
            g_print("User chose not to close the window\n");
            should_close = FALSE; // Prevent the window from closing
            break;
    }

    gtk_widget_destroy(dialog);

    if (should_close) // Free the memory
    {
        g_free(model);
        model = NULL;
    }
    

    // The function must return FALSE to allow the window to be destroyed,
    // or TRUE to prevent it.
    return !should_close;
}

/*
*   Function: training_init_progress_bar
*   ---------------------
*   Initializes the progress bar.
*/
void training_init_progress_bar(GtkWidget *vbox)
{
    progress_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(progress_container, "progress_container");

    // Create a label for progress text
    progress_text_label = gtk_label_new("Training Progress");
    gtk_box_pack_start(GTK_BOX(progress_container), progress_text_label,
    FALSE, FALSE, 0);
    gtk_widget_set_halign(progress_text_label, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(progress_text_label, GTK_ALIGN_START);

    // Create the progress bar
    progress_bar = gtk_progress_bar_new();
    gtk_widget_set_name(progress_bar, "progressbar");
    gtk_box_pack_start(GTK_BOX(progress_container), progress_bar,
    TRUE, TRUE, 0);

    // Pack the progress container in the parent vbox
    gtk_box_pack_start(GTK_BOX(vbox), progress_container, FALSE, FALSE, 5);
}


/*
*   Function: training_button_open
*   ---------------------
*   Opens a file chooser dialog.
*/
void training_button_open(GtkButton *button)
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
                                        "./models");

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) 
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        model = gtk_file_chooser_get_filename(chooser);

        if (!is_valid_model_file(model))
        {
            gtk_widget_destroy(dialog);

            GtkWidget *error_dialog = gtk_message_dialog_new(
                NULL,
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_CLOSE,
                "Invalid file type. Please select a valid model file."
            );
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);

            gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
        }
        else
        {
            gtk_widget_destroy(dialog);
            gtk_widget_set_sensitive(btn_training, FALSE);
            gtk_widget_set_sensitive(btn_testing, TRUE);
            gtk_widget_set_sensitive(btn_drawdigit, TRUE);
        }
    }
    else 
    {
        gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
        gtk_widget_destroy(dialog);
    }

    // Destroy progress bar, container, and text
    gtk_widget_destroy(progress_bar);
    gtk_widget_destroy(progress_container);
    gtk_widget_destroy(progress_text_label);
}


/*
*   Function: training_update_progress
*   ---------------------
*   Updates the progress bar text.
*/
void training_update_progress(float value)
{
    gchar *progress_text = NULL;
    if (value < 1.0)
    {
        progress_text = g_strdup_printf("Training: %.0f%%", value * 100);
    }
    else
    {
        progress_text = g_strdup_printf("Training done, model saved");
    }

    // Update progress bar fraction
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), value);

    // Update label text
    gtk_label_set_text(GTK_LABEL(progress_text_label), progress_text);
    gtk_widget_show(progress_text_label); // Make sure the label is visible

    g_free(progress_text);
}

/*
*   Function: training_button_training
*   ---------------------
*   Trains the model.
*/

void training_button_training(GtkButton *button)
{
    // Change the sensitivity of the buttons
    gtk_widget_set_sensitive(btn_training, FALSE);

    // Show progress container
    gtk_widget_show(progress_container);

    // Show progress bar
    gtk_widget_show(progress_bar);

    // Initialize the progress bar
    training_update_progress(0.0);

    // Wait for gtk to update the progress bar
    while (gtk_events_pending())
    {
        gtk_main_iteration_do(FALSE);
    }
    g_usleep(5000);

    // Call training function
    Mnist_Train();

    // Update the progress bar to show completion text
    training_update_progress(1.0);
}

/*
*   Function: training_init_accuracy_label
*   ---------------------
*   Initializes the accuracy label.
*/
void training_init_accuracy_label(GtkWidget *vbox, float accuracy)
{
    // Create the accuracy label
    accuracy_label = gtk_label_new("Accuracy: ");
    gtk_box_pack_start(GTK_BOX(vbox), accuracy_label, FALSE, FALSE, 0);
    gtk_widget_set_halign(accuracy_label, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(accuracy_label, GTK_ALIGN_START);

    // Set the accuracy label text
    gchar *accuracy_text = g_strdup_printf("Accuracy: %.2f%%", accuracy);
    gtk_label_set_text(GTK_LABEL(accuracy_label), accuracy_text);

    // Style the text
    PangoFontDescription *font_desc = 
    pango_font_description_from_string("Sans 20");


    //Deprecated function
    //gtk_widget_override_font(accuracy_label, font_desc);

    
    pango_font_description_free(font_desc);

    // Show the accuracy label
    gtk_widget_show(accuracy_label);

    // Free the memory
    g_free(accuracy_text);
}

/*
*   Function: training_button_testing
*   ---------------------
*   Tests the model.
*/
void training_button_testing(GtkButton *button)
{
    Network* n = N_Load(model);
    N_Compile(n,CE_Create());
    Dataset* testDataset = LoadTestMnist();
    float accuracy = TestAccuracy(n, testDataset);

    // Show the accuracy label
    training_init_accuracy_label(vbox_train, accuracy);
}

/*
*   Function: training_button_drawdigit
*   ---------------------
*   Calls drawdigit.
*/
void training_button_drawdigit(GtkButton *button)
{
    Network* n = N_Load(model);
    //It will free the network
    DrawDigit(0, NULL, n);
}

/*
*   Function: training_button_quit
*   ---------------------
*   Quits the program.
*/
void training_button_quit()
{
    g_free(model); // Free the memory
    model = NULL; // Reset the model file name

    // Quit the program
    gtk_widget_destroy(window_train);
}

/*
*   Function: training_buttons_init
*   ----------------------
*   Initializes the buttons.
*/
void training_buttons_init(GtkWidget *vbox)
{
    // Add buttons to vbox
    btn_training = gtk_button_new_with_label("Train model");
    btn_open_model = gtk_button_new_with_label("Open model");
    btn_testing = gtk_button_new_with_label("Testing");
    btn_drawdigit = gtk_button_new_with_label("Draw digit");
    btn_quit_train = gtk_button_new_with_label("Quit");
    gtk_box_pack_start(GTK_BOX(vbox), btn_training, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), btn_open_model, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), btn_testing, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), btn_drawdigit, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), btn_quit_train, FALSE, FALSE, 5);

    // Set sensitivity of buttons
    gtk_widget_set_sensitive(btn_training, TRUE);
    gtk_widget_set_sensitive(btn_open_model, TRUE);
    gtk_widget_set_sensitive(btn_testing, FALSE);
    gtk_widget_set_sensitive(btn_drawdigit, FALSE);

    // Connect the buttons to the execution of the files
    g_signal_connect(btn_training, "clicked",
    G_CALLBACK(training_button_training), NULL);
    g_signal_connect(btn_open_model, "clicked",
    G_CALLBACK(training_button_open), NULL);
    g_signal_connect(btn_testing, "clicked",
    G_CALLBACK(training_button_testing), NULL);
    g_signal_connect(btn_drawdigit, "clicked",
    G_CALLBACK(training_button_drawdigit), NULL);
    g_signal_connect(btn_quit_train, "clicked",
    G_CALLBACK(training_button_quit), NULL);
}

/*
*   Function: training_load_css
*   ------------------
*   Loads the CSS file.
*/
void training_load_css()
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
*   Function: show_trainingload_css
*   -----------------
*   Initializes the main window and calls the other functions.
*/
void show_training_window(GtkApplication *app)
{
    window_train = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window_train), "Training model");
    gtk_window_set_default_size(GTK_WINDOW(window_train), 600, 600);

    // Connect the delete-event signal to the handler
    g_signal_connect(window_train, "delete-event", 
    G_CALLBACK(training_on_delete_event), NULL);

    // Create the main vertical box instead of horizontal
    vbox_train = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window_train), vbox_train);
    gtk_widget_set_name(vbox_train, "vbox_train_machin");

    // Create the horizontal box for buttons and add to vbox_train
    hbox_train = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox_train), hbox_train, FALSE, FALSE, 0);
    gtk_widget_set_name(hbox_train, "hbox_train_buttons");

    // Initialize the buttons in the horizontal box at the top.
    training_buttons_init(hbox_train);

    // Load the CSS file
    training_load_css();

    // Initialize the progress bar
    training_init_progress_bar(vbox_train);

    // Show all widgets inside the main window_train.
    gtk_widget_show_all(window_train);

    // Hide progress bar
    gtk_widget_hide(progress_bar);

    // Hide progress container
    gtk_widget_hide(progress_container);
}