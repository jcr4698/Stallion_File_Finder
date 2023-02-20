#include <gtk/gtk.h>
#include "add_file_gui.h"

#define DEFAULT "\033[0m"

/* Display variables */
GtkWidget* files_found;
GtkWidget* scrollwin_found;
GtkTextBuffer* tb;

void load_css(void);

static int counter = 0;
struct gpointers {
    char* new_name;
    char* old_name;
    char* desc;
};
  
void find_for(GtkWidget* widget, struct gpointers* data) {

    /* Store varibles */
    const gchar *new_name = gtk_entry_get_text(GTK_ENTRY(data->new_name));
    const gchar *old_name = gtk_entry_get_text(GTK_ENTRY(data->old_name));
    const gchar *desc = gtk_entry_get_text(GTK_ENTRY(data->desc));

    /* Attempt to add file to Gallery_Content.txt */
    int result = add_file((char*)new_name, (char*)old_name, (char*)desc);

    int output_size = 0;
    gchar* output_text;

    if(result == 0) {
        /* Store text to display */
        output_size = strlen(new_name) + strlen(old_name) + strlen(desc) + 6;
        output_text = malloc((output_size + 1) * sizeof(char));
        sprintf(output_text, "%s -> %s\n\n%s", new_name, old_name, desc);

        /* Clear information fields */
        gtk_entry_set_text(GTK_ENTRY(data->old_name), "");
        gtk_entry_set_text(GTK_ENTRY(data->new_name), "");
        gtk_entry_set_text(GTK_ENTRY(data->desc), "");
    }
    else if((result == -2) || (result == -5)) {

        /* Store text to display */
        output_size = strlen(old_name) + 86;
        output_text = malloc((output_size + 1) * sizeof(char));
        sprintf(output_text, "File name \"%s\" doesn't exist in current working directory.\nCan't create symbolic link\n", old_name);

        /* Clear information fields */
        gtk_entry_set_text(GTK_ENTRY(data->old_name), "");
    }
    else if(result == -3) {

        /* Store text to display */
        output_size = strlen(new_name) + 38;
        output_text = malloc((output_size + 1) * sizeof(char));
        sprintf(output_text, "Symbolic link name \"%s\" already exists.\n", new_name);
        
        /* Clear information fields */
        gtk_entry_set_text(GTK_ENTRY(data->new_name), "");
    }
    else if(result == -6) {

        /* Store text to display */
        output_size = strlen(new_name) + strlen(old_name) + strlen(desc) + 6;
        output_text = malloc((output_size + 1) * sizeof(char));
        sprintf(output_text, "File name \"%s\" already has a symbolic link", old_name);

        /* Clear information fields */
        gtk_entry_set_text(GTK_ENTRY(data->old_name), "");
        gtk_entry_set_text(GTK_ENTRY(data->new_name), "");
        gtk_entry_set_text(GTK_ENTRY(data->desc), "");
    }
    else if(result == -7) {

        /* Store text to display */
        output_size = 63;
        output_text = malloc((output_size + 1) * sizeof(char));
        sprintf(output_text, "ERROR: incorrect format. one or more fields is empty/missing.\n");
    }
    else if(result == -8) {

        /* Store text to display */
        output_size = strlen(old_name) + 89;
        output_text = malloc((output_size + 1) * sizeof(char));
        sprintf(output_text, "ERROR: \"%s\" isn't a proper file name. It must be type \".png\", \".jpg\", \".mov\", etc...\n", old_name);

        /* Clear information fields */
        gtk_entry_set_text(GTK_ENTRY(data->old_name), "");
    }
    else {
        printf("What the heck happened?! Please report to Jan C. Rubio (jcaj750@gmail.com)");
    }

    /* Delete user entry to prepare next input */
    free(data);

    /* Display text */
    gtk_text_buffer_set_text(tb, output_text, -1);
}
  
void destroy(GtkWidget* widget, struct gpointers* data) {
    free(data);
    printf("%s", DEFAULT);
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
    GtkWidget* space;
    GtkWidget* space1;

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
    gtk_grid_attach(GTK_GRID(grid), new_name_field, 1, 0, 4, 1);

    /* Set up old name fields */
    old_name_label = gtk_label_new("Old Name:   ");
    gtk_widget_set_name(old_name_label, "label");
    old_name_field = gtk_entry_new();
    gtk_widget_set_name(old_name_field, "field");
    gtk_entry_set_placeholder_text(GTK_ENTRY(old_name_field), "i.e. old_name.txt ");
    gtk_grid_attach(GTK_GRID(grid), old_name_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), old_name_field, 1, 1, 4, 1);

    /* Set up description fields */
    desc_label = gtk_label_new("Description:    ");
    gtk_widget_set_name(desc_label, "label");
    desc_field = gtk_entry_new();
    gtk_widget_set_name(desc_field, "field");
    gtk_entry_set_placeholder_text(GTK_ENTRY(desc_field), "i.e. example description, jan, dec. 2022");
    gtk_grid_attach(GTK_GRID(grid), desc_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), desc_field, 1, 2, 4, 1);

    /* Add an empty space*/
    space = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), space, 0, 3, 1, 1); // Add space to grid

    /* Set up ADD button properties */
    add_button = gtk_button_new_with_label("Add File");    // Create button of name "Find"
    gtk_widget_set_name(add_button, "black-button");
    struct gpointers* new_sym_link = malloc(3*sizeof(gpointer));
    new_sym_link->new_name = (gpointer) new_name_field;
    new_sym_link->old_name = (gpointer) old_name_field;
    new_sym_link->desc = (gpointer) desc_field;
    g_signal_connect(G_OBJECT(add_button), "clicked", G_CALLBACK(find_for), new_sym_link);  // Set up button
    gtk_grid_attach(GTK_GRID(grid), add_button, 2, 4, 1, 1); // Add button to grid

    /* Add an empty space*/
    space1 = gtk_label_new(""); 
    gtk_grid_attach(GTK_GRID(grid), space1, 0, 5, 1, 1); // Add space

    /* Set up scrolling bar for files_found */
    scrollwin_found = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_set_border_width(GTK_CONTAINER(scrollwin_found), 6);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollwin_found), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrollwin_found), GTK_SHADOW_IN);

    /* Set up text area for files_found */
    files_found = gtk_text_view_new();
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(files_found), 2);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(files_found), 2);
    gtk_widget_set_hexpand(scrollwin_found, TRUE);
    GdkColor black = {0, 0x0000, 0x0000, 0x0000};
    GdkColor white = {0, 0xffff, 0xffff, 0xffff};
    gtk_widget_modify_bg(GTK_WIDGET(files_found), GTK_STATE_NORMAL, &black);
    gtk_widget_modify_fg(GTK_WIDGET(files_found), GTK_STATE_NORMAL, &white);
    gtk_widget_show(files_found);

    /* Connect scrolling bar to text area */
    gtk_container_add(GTK_CONTAINER(scrollwin_found), files_found);
    tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(files_found));

    /* Start text area for files_found */
	gtk_text_buffer_set_text(tb, "", -1);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(files_found), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(files_found), FALSE);
    gtk_grid_attach(GTK_GRID(grid), scrollwin_found, 0, 6, 5, 1);
    gtk_widget_set_size_request(scrollwin_found, 25, 80);

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
