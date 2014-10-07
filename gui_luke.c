/***********************************************
NOT MY CODE!!! THIS IS HERE FOR USABILITY'S SAKE
***********************************************/

#include "gui_luke.h"
#include <gtk/gtk.h>

/*
Based upon code retreived from:
http://www.gnu.org/software/guile-gnome/docs/gtk/html/GtkFileChooserDialog.html
*/
int fileOpenDialog(Window *window, char* dialogTitle, char* fName)
{
    int retval = 1;

    GtkWidget *dialog;

    dialog = gtk_file_chooser_dialog_new(dialogTitle, 
        (GtkWindow*)window->gtkWindow, GTK_FILE_CHOOSER_ACTION_OPEN, 
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

    if(gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {

        sprintf(fName, "%s", gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (dialog)));
    }
    else
    {
        retval = 0;
    }

    gtk_widget_destroy(dialog);


    return retval;
}

/*
Based upon code retreived from:
http://www.gnu.org/software/guile-gnome/docs/gtk/html/GtkFileChooserDialog.html
*/
int fileSaveDialog(Window *window, char* dialogTitle, char* fName)
{
    int pass = 1;

    GtkWidget* dialog;

    dialog = gtk_file_chooser_dialog_new(dialogTitle, (GtkWindow*)window->gtkWindow, GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER (dialog), TRUE);

    if(gtk_dialog_run (GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        sprintf(fName, "%s", gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog)));
    }
    else
    {
        pass = 0;
    }

    gtk_widget_destroy(dialog);


    return pass;
}
