#include "UI/App.h"
#include "UI/selection_ui.h"
#include "UI/solver_ui.h"
#include "UI/training_ui.h"
#include <gtk/gtk.h>

void LaunchApp(int argc, char** argv) 
{
    GtkApplication* app;
    int status;

    app = gtk_application_new("org.gtk.sudoku", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(show_selection_window), 
    NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
}
