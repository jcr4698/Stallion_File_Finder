#include <gtk/gtk.h>
#include "find_file_gui.h"

void load_css(void);

void lookup(GtkWidget* widget, gpointer data) {
    const gchar *key_words = gtk_entry_get_text(GTK_ENTRY(data));
    
    /* Store variables */
    char *str, *token;
    int tok = 0;
    int char_count = 3;

	/* Parse string */
    str = malloc((strlen((char*)key_words) + 1) * sizeof(char));
    strcpy(str, (char*)key_words);
    while(token = strtok_r(str, " ", &str)) {
		tok++;
        char_count += strlen(token) + 6;
    }

    /* Print found key words */
    search(tok, (char*)key_words, char_count);

    /* Delete user entry to prepare next input */
    gtk_entry_set_text(GTK_ENTRY(data), "");
}

void destroy(GtkWidget* widget, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char* argv[]) {

    /* Interface elements */
    GtkWidget* window;
    GtkWidget* grid;
    GtkWidget* key_words_label;
    GtkWidget* key_words_field;
    GtkWidget* space;
    GtkWidget* find_button;

    /* Initial message */
    g_print("Find a file by keywords:\n");

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
    key_words_label = gtk_label_new("Lookup:    ");
    gtk_widget_set_name(key_words_label, "label");
    key_words_field = gtk_entry_new();
    gtk_widget_set_name(key_words_field, "field");
    gtk_entry_set_placeholder_text(GTK_ENTRY(key_words_field), "i.e. keyword_1 keyword_2 keyword_3 ...");
    gtk_grid_attach(GTK_GRID(grid), key_words_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), key_words_field, 1, 0, 4, 1);

    /* Add an empty space*/
    space = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), space, 1, 1, 1, 1); // Add button to grid

    /* Set up ADD button properties */
    find_button = gtk_button_new_with_label("Find File");    // Create button of name "Find"
    gtk_widget_set_name(find_button, "black-button");
    g_signal_connect(G_OBJECT(find_button), "clicked", G_CALLBACK(lookup), key_words_field);  // Set up button
    gtk_grid_attach(GTK_GRID(grid), find_button, 2, 2, 2, 1); // Add button to grid

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
