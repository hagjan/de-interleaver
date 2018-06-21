//#include <gtk/gtk.h>
#include "snd_tools.h"


#define NUM_FORMATS 3


typedef struct ProgressData {
    GtkWidget *bar;
    guint id;
} ProgressData;


static void destroy (GtkWidget*, gpointer);
static gboolean delete_event (GtkWidget*, GdkEvent*, gpointer);
static void file_changed (GtkWidget*, gpointer);
static void format_changed (GtkWidget*, gpointer);
static void outfile_btn_clicked (GtkWidget*, GtkWidget*);
static void render (GtkWidget*, ProgressData*);
static gboolean update_progress_in_timeout (gpointer);
static gpointer worker (gpointer);
static gboolean worker_finish_in_idle (gpointer);
