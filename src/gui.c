#include "gui.h"

gdouble fraction;
gchar label_string[64];
gint format;
static GtkWidget *outfile_btn;
static GtkWidget *infile_btn;
static GtkWidget *render_btn;
static GtkWidget *format_combobox;
static GtkWidget *outfile_entry;
static GtkWidget *infile_entry;
static GtkWidget *progress_label;
static gint rendering;

int main (int argc, char *argv[])
{   
    GtkWidget *window;
    GtkWidget *format_frame;
    GtkWidget *grid, *format_grid;
    GtkWidget *open_label, *save_label, *format_label;
    GtkFileFilter *filter_audio, *filter_all;
    GtkListStore *format_list;
    GtkCellRenderer *format_column;
    ProgressData *prog_data;

    fraction = 0.0;

    gtk_init (&argc, &argv);
    g_object_set (gtk_settings_get_default (),
                    "gtk-application-prefer-dark-theme", TRUE, NULL);


    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    infile_entry = gtk_entry_new ();
    outfile_entry = gtk_entry_new ();

    outfile_btn = gtk_button_new_with_label ("Browse");
    infile_btn = gtk_file_chooser_button_new ("Browse", GTK_FILE_CHOOSER_ACTION_OPEN);
    render_btn = gtk_button_new_with_label ("Render");

    prog_data = g_malloc (sizeof (struct ProgressData));
    prog_data->bar = gtk_progress_bar_new ();

    /* Main window */
    gtk_window_set_icon_name (GTK_WINDOW (window), "de-interleaver");
    gtk_window_set_title (GTK_WINDOW (window), "De-Interleaver");
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_set_size_request (window, 600, -1);
    
    /* Entries */
    gtk_widget_set_hexpand (infile_entry, TRUE);
    gtk_widget_set_hexpand (outfile_entry, TRUE);

    /* Labels */
    save_label = gtk_label_new ("Path to save file as: ");
    gtk_widget_set_halign (save_label, GTK_ALIGN_START);
    open_label = gtk_label_new ("Path to input file: ");
    gtk_widget_set_halign (open_label, GTK_ALIGN_START);
    progress_label = gtk_label_new ("");
    gtk_widget_set_halign (progress_label, GTK_ALIGN_START);
    format_label = gtk_label_new ("Channel order (5.1):");
    gtk_widget_set_halign (format_label, GTK_ALIGN_START);
    gtk_widget_set_hexpand (format_label, TRUE);
    
    
    /* File chooser */
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (infile_btn),
                        g_get_home_dir ());
                        
    
    filter_audio = gtk_file_filter_new ();
    gtk_file_filter_set_name (filter_audio, "Audio Files");
    gtk_file_filter_add_pattern (filter_audio, "*.wav");
    gtk_file_filter_add_pattern (filter_audio, "*.WAV");
    gtk_file_filter_add_pattern (filter_audio, "*.aiff");
    gtk_file_filter_add_pattern (filter_audio, "*.AIFF");

    filter_all = gtk_file_filter_new ();
    gtk_file_filter_set_name (filter_all, "All Files");
    gtk_file_filter_add_pattern (filter_all, "*");

    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (infile_btn), filter_audio);
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (infile_btn), filter_all);

    /* Progres bar */
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (prog_data->bar), fraction);
    gtk_widget_set_hexpand (prog_data->bar, TRUE);
   
    /* Combo box */
    format_list = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);
    gtk_list_store_insert_with_values (format_list, NULL, -1, 0, "Film", -1);
    gtk_list_store_insert_with_values (format_list, NULL, -1, 0, "SMPTE/ITU", -1);
    gtk_list_store_insert_with_values (format_list, NULL, -1, 0, "Numeric", -1);
    
    format_combobox = gtk_combo_box_new_with_model (GTK_TREE_MODEL (format_list));
    g_object_unref (format_list);
    gtk_widget_set_hexpand (format_combobox, TRUE);

    format_column = gtk_cell_renderer_text_new ();
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (format_combobox), format_column, TRUE);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (format_combobox), format_column, "text", 0, NULL);
    gtk_combo_box_set_active (GTK_COMBO_BOX (format_combobox), 0);
    format = 0;

    /* Frames */
    format_frame = gtk_frame_new (NULL);
    gtk_frame_set_label (GTK_FRAME (format_frame), "Settings");
    gtk_container_set_border_width (GTK_CONTAINER (format_combobox), 10);
    gtk_widget_set_hexpand (format_frame, TRUE);
    format_grid = gtk_grid_new ();

    gtk_grid_attach (GTK_GRID (format_grid), format_label, 0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (format_grid), format_combobox, 1, 0, 1, 1);
    gtk_container_set_border_width (GTK_CONTAINER (format_grid), 10);
    gtk_container_add (GTK_CONTAINER (format_frame), format_grid);


    /* Signals */
    g_signal_connect (G_OBJECT (window), "destroy",
                        G_CALLBACK (destroy), NULL);
    g_signal_connect (G_OBJECT (window), "delete_event",
                        G_CALLBACK (delete_event), NULL);

    g_signal_connect (G_OBJECT (outfile_btn), "clicked",
                        G_CALLBACK (outfile_btn_clicked), window);
    
    g_signal_connect (G_OBJECT (infile_btn), "selection_changed",
                        G_CALLBACK (file_changed), NULL);

    g_signal_connect (G_OBJECT (render_btn), "clicked",
                        G_CALLBACK (render), (gpointer) prog_data);

    g_signal_connect (G_OBJECT (format_combobox), "changed",
                        G_CALLBACK (format_changed), NULL);


    /* Layout */
    grid = gtk_grid_new ();
    gtk_grid_set_row_spacing (GTK_GRID (grid), 10);
    gtk_grid_set_column_spacing (GTK_GRID (grid), 10);
    
    gtk_grid_attach (GTK_GRID (grid), open_label, 0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (grid), infile_entry, 0, 1, 6, 1);
    gtk_grid_attach (GTK_GRID (grid), infile_btn, 7, 1, 1, 1);
    gtk_grid_attach (GTK_GRID (grid), save_label, 0, 2, 1, 1);
    gtk_grid_attach (GTK_GRID (grid), outfile_entry, 0, 3, 6, 1);
    gtk_grid_attach (GTK_GRID (grid), outfile_btn, 7, 3, 1, 1);
    gtk_grid_attach (GTK_GRID (grid), format_frame, 0, 4, 8, 1);
    gtk_grid_attach (GTK_GRID (grid), progress_label, 0, 5, 10, 1);
    gtk_grid_attach (GTK_GRID (grid), prog_data->bar, 0, 5, 6, 1);
    gtk_grid_attach (GTK_GRID (grid), render_btn, 7, 5, 1, 1);

    gtk_container_add (GTK_CONTAINER (window), grid);
    gtk_widget_show_all (window);

    gtk_main ();
    g_free (prog_data);
    return 0;
}


static void destroy (GtkWidget *widget, gpointer data)
{
    gtk_main_quit ();
}

static gboolean delete_event (GtkWidget *window, GdkEvent *event, gpointer data)
{
    GtkWidget *dialog;
    int answer;

    if (rendering) {
        dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_QUESTION,
                                        GTK_BUTTONS_YES_NO,
                                        "Rendering in progress. Exit?");
        gtk_window_set_title (GTK_WINDOW (dialog), "Exit");

        answer = gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);

        if (answer == GTK_RESPONSE_NO)
            return TRUE;
    }
    return FALSE;
}


static void file_changed (GtkWidget *chooser, gpointer data)
{
    gchar *file;

    file = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
    gtk_entry_set_text (GTK_ENTRY (infile_entry), file);
    gtk_entry_set_text (GTK_ENTRY (outfile_entry), file);
}

static void format_changed (GtkWidget *format_box, gpointer data)
{
    format = gtk_combo_box_get_active (GTK_COMBO_BOX (format_box));
}

static void outfile_btn_clicked (GtkWidget *btn, GtkWidget *window)
{
    GtkWidget *dialog;
    gchar *filename;
    gint result;

    dialog = gtk_file_chooser_dialog_new ("Save File As ...", GTK_WINDOW (window), 
                                GTK_FILE_CHOOSER_ACTION_SAVE,
                                "_Cancel", GTK_RESPONSE_CANCEL,
                                "_Save", GTK_RESPONSE_ACCEPT,
                                NULL);

    result = gtk_dialog_run (GTK_DIALOG (dialog));
    if (result == GTK_RESPONSE_ACCEPT) {
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        gtk_entry_set_text (GTK_ENTRY (outfile_entry), filename);
    }

    gtk_widget_destroy (dialog);

}

static void render (GtkWidget *button, ProgressData *prog_data)
{
    GThread *thread;

    gtk_widget_set_sensitive (outfile_btn, FALSE);
    gtk_widget_set_sensitive (infile_btn, FALSE);
    gtk_widget_set_sensitive (render_btn , FALSE);

    prog_data->id = g_timeout_add (100, update_progress_in_timeout, prog_data->bar);    

    thread = g_thread_new ("worker", worker, prog_data);
    g_thread_unref (thread);

}

static gboolean update_progress_in_timeout (gpointer progress)
{
    GtkWidget *pbar = progress;
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (pbar), fraction);
    gtk_label_set_text (GTK_LABEL (progress_label), label_string);

    return TRUE;
}

static gpointer worker (gpointer data)
{

    /* de-interleave */
    rendering = 1;
    de_interleave (infile_entry, outfile_entry, gtk_combo_box_get_active (GTK_COMBO_BOX (format_combobox)));

    g_idle_add (worker_finish_in_idle, data);
    rendering = 0;
    return NULL;
}

static gboolean worker_finish_in_idle (gpointer data)
{
    ProgressData *prog_data = data;
    fraction = 0.0;

    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (prog_data->bar), 0.0);
    gtk_label_set_text (GTK_LABEL (progress_label), "");
    gtk_widget_set_sensitive (outfile_btn, TRUE);
    gtk_widget_set_sensitive (infile_btn, TRUE);
    gtk_widget_set_sensitive (render_btn , TRUE);
    

    g_source_remove (prog_data->id);

    return FALSE;
}
