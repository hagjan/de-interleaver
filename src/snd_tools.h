#include <sndfile.h>
#include <gtk/gtk.h>

#define BUFFER_LEN 4096
#define MAX_CHANNELS 16

enum {
    FILM,
    ITU,
    NUMERIC
};

typedef struct {
    SNDFILE *infile;
    SNDFILE *outfile [MAX_CHANNELS];
    gchar filename [MAX_CHANNELS][64];

    union {
        double d [MAX_CHANNELS * BUFFER_LEN];
        int i [MAX_CHANNELS * BUFFER_LEN];
    } din;

    union {
        double d [BUFFER_LEN];
        int i [BUFFER_LEN];
    } dout;

    int channels;
} STATE;


static void deinterleave_int (STATE *state, SF_INFO *sfinfo);
static void deinterleave_double (STATE *state, SF_INFO *sfinfo);
int de_interleave (GtkWidget*, GtkWidget*, gint);
