#include <gtk/gtk.h>
#include "add_file_gui.h"

void load_css(void);

static int counter = 0;
struct gpointers {
    char* new_name;
    char* old_name;
    char* desc;
};
  
void find_for(GtkWidget* widget, struct gpointers* data) {
    const gchar *new_name = gtk_entry_get_text(GTK_ENTRY(data->new_name));
    const gchar *old_name = gtk_entry_get_text(GTK_ENTRY(data->old_name));
    const gchar *desc = gtk_entry_get_text(GTK_ENTRY(data->desc));
    int result = add_file((char*)new_name, (char*)old_name, (char*)desc);
    if(result == -2)
        gtk_entry_set_text(GTK_ENTRY(data->old_name), "");
    else if(result == -3)
        gtk_entry_set_text(GTK_ENTRY(data->new_name), "");
    else if(result == -5) {
        gtk_entry_set_text(GTK_ENTRY(data->old_name), "");
        gtk_entry_set_text(GTK_ENTRY(data->new_name), "");
    }
    else if(result == -6) {
        gtk_entry_set_text(GTK_ENTRY(data->old_name), "");
        gtk_entry_set_text(GTK_ENTRY(data->new_name), "");
        gtk_entry_set_text(GTK_ENTRY(data->desc), "");
    }
}
  
void destroy(GtkWidget* widget, gpointer data) {
    gtk_main_quit();
}
  
int main(int argc, char* argv[]) {

    /* Interface elements */
    GtkWidget* window;
    GtkWidget* grid;
    GtkWidget* new_name_label;
    GtkWidget* new_name_field;
    GtkWidget* old_name_label;
    GtkWidget* old_name_field;
    GtkWidget* desc_label;
    GtkWidget* desc_field;
    GtkWidget* add_button;

    /* Initial message */
    g_print("Add a Symbolic Link:\n");

    /* Initialize the GTK+ libraries */
    gtk_init(&argc, &argv);

    /* Interface style */
    load_css();

    /* Set up window properties */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 100, 100);
    gtk_container_set_border_width(GTK_CONTAINER(window), 50);
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);
    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL); // Exit button terminates window

    /* Set up new name fields */
    new_name_label = gtk_label_new("New Name:   ");
    gtk_widget_set_name(new_name_label, "label");
    new_name_field = gtk_entry_new();
    gtk_widget_set_name(new_name_field, "field");
    gtk_entry_set_placeholder_text(GTK_ENTRY(new_name_field), "i.e. new_name.txt");
    gtk_grid_attach(GTK_GRID(grid), new_name_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), new_name_field, 1, 0, 1, 1);

    /* Set up old name fields */
    old_name_label = gtk_label_new("Old Name:   ");
    gtk_widget_set_name(old_name_label, "label");
    old_name_field = gtk_entry_new();
    gtk_widget_set_name(old_name_field, "field");
    gtk_entry_set_placeholder_text(GTK_ENTRY(old_name_field), "i.e. old_name.txt ");
    gtk_grid_attach(GTK_GRID(grid), old_name_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), old_name_field, 1, 1, 1, 1);

    /* Set up description fields */
    desc_label = gtk_label_new("Description:    ");
    gtk_widget_set_name(desc_label, "label");
    desc_field = gtk_entry_new();
    gtk_widget_set_name(desc_field, "field");
    gtk_entry_set_placeholder_text(GTK_ENTRY(desc_field), "i.e. example description, jan, dec. 2022");
    gtk_grid_attach(GTK_GRID(grid), desc_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), desc_field, 1, 2, 1, 1);

    /* Set up ADD button properties */
    add_button = gtk_button_new_with_label("Add File");    // Create button of name "Find"
    gtk_widget_set_name(add_button, "black-button");
    struct gpointers* new_sym_link = malloc(3*sizeof(gpointer));
    new_sym_link->new_name = (gpointer) new_name_field;
    new_sym_link->old_name = (gpointer) old_name_field;
    new_sym_link->desc = (gpointer) desc_field;
    g_signal_connect(G_OBJECT(add_button), "clicked", G_CALLBACK(find_for), new_sym_link);  // Set up button
    gtk_grid_attach(GTK_GRID(grid), add_button, 1, 3, 1, 1); // Add button to grid

    /* Display/Run window and its elements */
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), FALSE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}

void load_css(void) {
    GtkCssProvider* provider;
    GdkDisplay* display;
    GdkScreen* screen;
    const gchar* css_style_file = "gui/style_sheet.css";
    GFile* css_fp = g_file_new_for_path(css_style_file);
    GError* error = 0;
    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_css_provider_load_from_file( provider, css_fp, &error);
    g_object_unref(provider);
}
