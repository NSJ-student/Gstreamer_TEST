/*
 * GtkMainWindow.cpp
 *
 *  Created on: 2022. 9. 16.
 *      Author: Sujin
 */

#include "GtkMainWindow.h"

static void on_playbin_clicked(GtkButton *btn, gpointer user_data);
static void on_camerabin_clicked(GtkButton *btn, gpointer user_data);
static void on_file_open_clicked(GtkButton *btn, gpointer user_data);
static void on_shortcut_key_pressed(GtkWidget *widget, GdkEventKey *event, gpointer user_data);

/**********************************************/
//	Class Methods
/**********************************************/

GtkMainWindow::GtkMainWindow(GApplication *app, GstPlayer * player) :
		gst_player(player),
		current_uri(NULL)
{
    //////////////////////////////
    /////  main window
    //////////////////////////////
	gtk_window = gtk_application_window_new(GTK_APPLICATION(app));
	gtk_hpaned_container = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_viewer_notebook  = gtk_notebook_new();
	gtk_video_viewer = gst_player->get_video_widget();
	gtk_camera_viewer = gst_player->get_camera_widget();
	gtk_control_notebook  = gtk_notebook_new();

	// set viewer notebook
	gtk_notebook_set_show_tabs (GTK_NOTEBOOK(gtk_viewer_notebook), false);
	gtk_notebook_set_show_border (GTK_NOTEBOOK(gtk_viewer_notebook), false);
    gtk_notebook_append_page (GTK_NOTEBOOK(gtk_viewer_notebook), gtk_video_viewer, NULL);
    gtk_notebook_append_page (GTK_NOTEBOOK(gtk_viewer_notebook), gtk_camera_viewer, NULL);
    // set control notebook
    gtk_notebook_set_tab_pos (GTK_NOTEBOOK(gtk_control_notebook), GTK_POS_TOP);

    // add to container
    gtk_container_add (GTK_CONTAINER (GTK_WIDGET(gtk_window)), GTK_WIDGET(gtk_hpaned_container));
	gtk_paned_pack1 (GTK_PANED(gtk_hpaned_container), gtk_viewer_notebook, true, true);
    gtk_paned_pack2 (GTK_PANED(gtk_hpaned_container), gtk_control_notebook, true, true);

    g_signal_connect (GTK_WIDGET(gtk_window), "key-press-event", G_CALLBACK(on_shortcut_key_pressed), this);

    //////////////////////////////
    /////  page 1 (video)
    //////////////////////////////
    gtk_notebook_page1 = gtk_fixed_new();
	GtkWidget * label = gtk_label_new ("Video");
    gtk_notebook_append_page (GTK_NOTEBOOK(gtk_control_notebook), gtk_notebook_page1, label);

    gtk_txt_videopath = gtk_entry_new();
    gtk_btn_select_video = gtk_button_new_with_label("Open");
    gtk_btn_play_video = gtk_button_new_with_label("Play");

    g_signal_connect(gtk_btn_select_video, "clicked", G_CALLBACK(on_file_open_clicked), this);
    g_signal_connect(gtk_btn_play_video, "clicked", G_CALLBACK(on_playbin_clicked), this);

    gtk_fixed_put(GTK_FIXED(gtk_notebook_page1), GTK_WIDGET(gtk_txt_videopath), 10, 10);
    gtk_fixed_put(GTK_FIXED(gtk_notebook_page1), GTK_WIDGET(gtk_btn_select_video), 10, 50);
    gtk_fixed_put(GTK_FIXED(gtk_notebook_page1), GTK_WIDGET(gtk_btn_play_video), 80, 50);

    //////////////////////////////
    /////  page 2 (camera)
    //////////////////////////////
    gtk_notebook_page2 = gtk_fixed_new();
	GtkWidget * label2 = gtk_label_new ("Camera");
    gtk_notebook_append_page (GTK_NOTEBOOK(gtk_control_notebook), gtk_notebook_page2, label2);

    gtk_btn_play_camera = gtk_button_new_with_label("Play");
    g_signal_connect(gtk_btn_play_camera, "clicked", G_CALLBACK(on_camerabin_clicked), this);

    gtk_fixed_put(GTK_FIXED(gtk_notebook_page2), GTK_WIDGET(gtk_btn_play_camera), 10, 10);

    //////////////////////////////
    /////  display the gtk_window
    //////////////////////////////
    gtk_widget_show_all(GTK_WIDGET(gtk_window));
    gtk_widget_set_size_request(GTK_WIDGET(gtk_window), 940, 480);
    gtk_widget_queue_resize (GTK_WIDGET(gtk_window));
}

GtkMainWindow::~GtkMainWindow()
{
	// TODO Auto-generated destructor stub
}

GdkWindow* GtkMainWindow::get_video_sink()
{
	return gtk_widget_get_window (gtk_video_viewer);
}

GtkWidget* GtkMainWindow::get_video_widget()
{
	return gtk_video_viewer;
}

void GtkMainWindow::set_video_widget(GtkWidget *widget)
{
	gtk_video_viewer = widget;
	gtk_paned_pack1 (GTK_PANED(gtk_hpaned_container), gtk_video_viewer, true, true);
}

void GtkMainWindow::resize_video_viewer()
{
	int width = gtk_paned_get_position(GTK_PANED(gtk_hpaned_container));
	int org_w, org_h;
	int window_w, window_h;
	gtk_widget_get_size_request(GTK_WIDGET(gtk_video_viewer), &org_w, &org_h);
	gtk_window_get_size(GTK_WINDOW(gtk_window), &window_w, &window_h);

	g_print("paned resize: (%dx%d) -> (%dx%d)\n", org_w, org_h, width, window_h);
    gtk_widget_set_size_request(GTK_WIDGET(gtk_video_viewer), width, window_h);
    gtk_widget_queue_resize (GTK_WIDGET(gtk_video_viewer));
//	gst_video_overlay_set_render_rectangle(GST_VIDEO_OVERLAY(video_sink), 0, 0, width, window_h);
}

void GtkMainWindow::select_video_file()
{
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;

    if(gst_player->is_video_playing())
    {
		g_printerr ("video is playing\n");
    	return;
    }

	dialog = gtk_file_chooser_dialog_new ("Open File",
	                                      GTK_WINDOW(gtk_window),
	                                      action,
	                                      "Cancel",
	                                      GTK_RESPONSE_CANCEL,
	                                      "Open",
	                                      GTK_RESPONSE_ACCEPT,
	                                      NULL);

	res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	{
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		char *filename = gtk_file_chooser_get_filename (chooser);
		char *uri = gtk_file_chooser_get_uri (chooser);

		gtk_entry_set_text(GTK_ENTRY(gtk_txt_videopath), filename);
		if(current_uri)
		{
			g_free (current_uri);
		}
		current_uri = uri;
		g_free (filename);
	}

	gtk_widget_destroy (dialog);
}

void GtkMainWindow::start_stop_camera()
{
	if(gst_player->is_camera_playing())
	{
		if(gst_player->stop_camera())
		{
	        gtk_button_set_label(GTK_BUTTON(gtk_btn_play_camera), "Start");
		}
	}
	else
	{
		if(gst_player->is_video_playing())
		{
			if(gst_player->stop_video())
			{
		        gtk_button_set_label(GTK_BUTTON(gtk_btn_play_video), "Start");
			}
		}

		if(gst_player->play_camera())
		{
			gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_viewer_notebook), 1);
	        gtk_button_set_label(GTK_BUTTON(gtk_btn_play_camera), "Stop");
		}
	}
}

void GtkMainWindow::start_stop_video()
{
	if(gst_player->is_video_playing())
	{
		if(gst_player->stop_video())
		{
	        gtk_button_set_label(GTK_BUTTON(gtk_btn_play_video), "Start");
		}
	}
	else
	{
		if(gst_player->is_camera_playing())
		{
			if(gst_player->stop_camera())
			{
		        gtk_button_set_label(GTK_BUTTON(gtk_btn_play_camera), "Start");
			}
		}

		if(gst_player->play_video(current_uri))
		{
			gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_viewer_notebook), 0);
	        gtk_button_set_label(GTK_BUTTON(gtk_btn_play_video), "Stop");
		}
	}
}

gboolean GtkMainWindow::is_fullscreen()
{
    GdkWindow * window = gtk_widget_get_window(GTK_WIDGET(gtk_window));
    if(window == 0)
    {
        return false;
    }

    GdkWindowState state = gdk_window_get_state(window);
    if((state & GDK_WINDOW_STATE_FULLSCREEN) == 0)
    {
        return false;
    }

    return true;
}

void GtkMainWindow::shortcut_handler(GdkEventKey *event)
{
    switch (event->keyval){
        case GDK_KEY_F5: {
        	if(!is_fullscreen())
        	{
                gtk_window_fullscreen(GTK_WINDOW(gtk_window));
                gtk_widget_queue_resize (GTK_WIDGET(gtk_window));
        	}
        	else
        	{
                gtk_window_unfullscreen(GTK_WINDOW(gtk_window));
                gtk_widget_queue_resize (GTK_WIDGET(gtk_window));
        	}
            break;
        }
        case GDK_KEY_q:
        case GDK_KEY_Q:
            break;
        case GDK_KEY_F1:
            break;
        case GDK_KEY_F3:
            break;
        case GDK_KEY_Right:
            if(event->state & GDK_CONTROL_MASK)
            {
            }
            break;
        case GDK_KEY_Left:
            if(event->state & GDK_CONTROL_MASK)
            {
            }
            break;
        case GDK_KEY_t:
        case GDK_KEY_T:
            if(event->state & GDK_CONTROL_MASK)
            {
            }
            break;
        case GDK_KEY_m:
        case GDK_KEY_M:
            if(event->state & GDK_CONTROL_MASK)
            {
            }
            break;
        default:
            break;
    }
}




/**********************************************/
//	callback functions
/**********************************************/

static void on_playbin_clicked(GtkButton *btn, gpointer user_data)
{
	GtkMainWindow * p_main = (GtkMainWindow *)user_data;

	if(p_main == NULL)
	{
		return;
	}

	p_main->start_stop_video();
}

static void on_camerabin_clicked(GtkButton *btn, gpointer user_data)
{
	GtkMainWindow * p_main = (GtkMainWindow *)user_data;

	if(p_main == NULL)
	{
		return;
	}

	p_main->start_stop_camera();
}

static void on_file_open_clicked(GtkButton *btn, gpointer user_data)
{
	GtkMainWindow * p_main = (GtkMainWindow *)user_data;

	if(p_main == NULL)
	{
		return;
	}

	p_main->select_video_file();
}

static void on_shortcut_key_pressed(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	GtkMainWindow * p_main = (GtkMainWindow *)user_data;

	if(p_main == NULL)
	{
		return;
	}

	p_main->shortcut_handler(event);
}
