#include "snd_tools.h"

extern gdouble fraction;
extern gchar label_string[64];


int de_interleave (GtkWidget *infile_entry, GtkWidget *outfile_entry, gint format)
{
    STATE state;
    SF_INFO sfinfo;
    SF_EMBED_FILE_INFO sfeinfo;
    char infile_path [512], outfile_path [512], ext [32], *cptr;
    int ch, double_split;    
    const gchar *in_path, *out_path;

    char *format_itu[6] = { "L", "C", "R", "Ls", "Rs", "LFE" };
    char *format_film[6] = { "L", "R", "C", "LFE", "Ls", "Rs" };
    char *format_numeric[8] = { "01", "02", "03", "04", "05", "06", "07", "08" };
    
    //if (signal(SIGINT, signal_handler) == SIG_ERR)
    //    g_error ("Error: Can not catch SIGKILL");

    in_path = gtk_entry_get_text (GTK_ENTRY (infile_entry));
    out_path = gtk_entry_get_text (GTK_ENTRY (outfile_entry));


    memset (&state, 0, sizeof (state));
    memset (&sfinfo, 0 ,sizeof (sfinfo));

    if ((state.infile = sf_open (in_path, SFM_READ, &sfinfo)) == NULL) {
        g_error ("Error: Could not open input file '%s' \n%s \n", infile_path, sf_strerror (NULL));
        return 1;
    }

    if (sfinfo.channels < 2) {
        g_error ("Error: Input file '%s' has only one channel. \n", infile_path);
        return 1;
    }

    state.channels = sfinfo.channels;
    sfinfo.channels = 1;


    if (snprintf (infile_path, sizeof (infile_path), "%s", (char *) in_path) > sizeof (infile_path)) {
        g_error ("Error: Length of input file path exceeds MAX_PATH (%d)\n", (int) sizeof (infile_path));
        return 1;
    }
    if (snprintf (outfile_path, sizeof (outfile_path), "%s",(char *) out_path) > sizeof (outfile_path)) {
        g_error ("Error: Length of input file path exceeds MAX_PATH (%d)\n", (int) sizeof (outfile_path));
        return 1;
    }


    if ((cptr = strrchr (infile_path, '.')) == NULL) {
        ext[0] = 0;
    } else {
        snprintf (ext, sizeof (ext), "%s", cptr);
        cptr[0] = 0;
        if ((cptr = strrchr (outfile_path, '.')) != NULL)
            cptr [0] = 0;
    }

    for (ch = 0; ch < state.channels; ch++) {
        char filename [555];
        char *nptr;

        if (state.channels == 6) {
            switch (format) {
                case FILM:
                    snprintf (filename, sizeof (filename), "%s_%s%s", outfile_path, format_film[ch], ext);
                    break;
                case ITU:
                    snprintf (filename, sizeof (filename), "%s_%s%s", outfile_path, format_itu[ch], ext);
                    break;
                case NUMERIC:
                default:
                    snprintf (filename, sizeof (filename), "%s_%s%s", outfile_path, format_numeric[ch], ext);
            }
        } else {
            snprintf (filename, sizeof (filename), "%s_%s%s", outfile_path, format_numeric[ch], ext);
        }
        

        if ((state.outfile [ch] = sf_open (filename, SFM_WRITE, &sfinfo)) == NULL) {
            g_error ("Error: Could not open output file '%s' \n%s \n", filename, sf_strerror (NULL));
            return 1;
        }
        
        if ((nptr = strrchr (filename, '/')) == NULL) {
            strcpy (state.filename [ch], filename);
        } else {
            strcpy (state.filename [ch], ++nptr);
        }
    }

    switch (sfinfo.format & SF_FORMAT_SUBMASK) {
        case SF_FORMAT_FLOAT:
        case SF_FORMAT_DOUBLE:
        case SF_FORMAT_VORBIS:
            double_split = 1;
            break;
        default:
            double_split = 0;
            break;
    }

    if (double_split)
        deinterleave_double (&state, &sfinfo);
    else
        deinterleave_int (&state, &sfinfo);

    sf_close (state.infile);

    for (ch = 0; ch < MAX_CHANNELS; ch++) {
        if (state.outfile [ch] != NULL)
            sf_close (state.outfile [ch]);
    }

    return 0;
}
/*
static void signal_handler (int signo) {
    if (signo == SIGKILL)
        g_print ("SIGKILL");
}
*/
static void deinterleave_int (STATE *state, SF_INFO *sfinfo) 
{
    int read_len;
    int ch, k, frames_chan, frames;
    
    for (ch = 0; ch < state->channels; ch++) {
        frames = 0;
        g_snprintf (label_string, sizeof (label_string), "%s%s%s", "Writing file '", state->filename [ch], "' ...");
        sf_seek (state->infile, 0, SEEK_SET);
        
        do {	
            read_len = sf_readf_int (state->infile, state->din.i, BUFFER_LEN);
            
            for (k = 0 ; k < read_len ; k++) {
				state->dout.i [k] = state->din.i [k * state->channels + ch];
            }
			
            sf_write_int (state->outfile [ch], state->dout.i, read_len);
            frames += read_len;
            usleep(300); 
            if (ch > 0)
                fraction = (1.0 * frames) / (1.0 * frames_chan);
	    } while (read_len > 0);
        if (ch == 0)
            frames_chan = frames;
    }
}



static void deinterleave_double (STATE *state, SF_INFO *sfinfo)
{
    int read_len;
    int ch, k;

    do {
        read_len = sf_readf_double (state->infile, state->din.d, BUFFER_LEN);

        for (ch = 0; ch < state->channels; ch++) {
            for (k = 0; k < read_len; k++) {
                state->dout.d [k] = state->din.d [k * state->channels + ch];
                sf_write_double (state->outfile [ch], state->dout.d, read_len);
            }
        }
    } while (read_len > 0);

}
