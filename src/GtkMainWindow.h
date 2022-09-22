/*
 * GtkMainWindow.h
 *
 *  Created on: 2022. 9. 16.
 *      Author: Sujin
 */

#ifndef SRC_GTKMAINWINDOW_H_
#define SRC_GTKMAINWINDOW_H_

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#ifdef WIN32
#include <gdk/gdkwin32.h>
#else
#include <gdk/gdkx.h>
#endif
#include "GstPlayer.h"

typedef gboolean (*def_callback_t)(void);
typedef gboolean (*check_callback_t)(void);

class GtkMainWindow {
public:
	GtkMainWindow(GApplication *app, GstPlayer * player);
	virtual ~GtkMainWindow();

	GdkWindow * get_video_sink();
	GtkWidget * get_video_widget();;
	void set_video_widget(GtkWidget * widget);
	void resize_video_viewer();
	void select_video_file();

	void start_stop_camera();
	void start_stop_video();

	gboolean is_fullscreen();
	void shortcut_handler(GdkEventKey *event);

private:
	GtkWidget * gtk_window;
	GtkWidget * gtk_vpaned_container;
		GtkWidget * gtk_viewer_notebook;
			GtkWidget * gtk_video_viewer;
			GtkWidget * gtk_camera_viewer;
		GtkWidget * gtk_control_notebook;
			GtkWidget * gtk_notebook_page1;
				GtkWidget * gtk_txt_videopath;
				GtkWidget * gtk_btn_select_video;
				GtkWidget * gtk_btn_play_video;
			GtkWidget * gtk_notebook_page2;
				GtkWidget * gtk_btn_play_camera;

	GstPlayer * gst_player;
	char * current_uri;
};

#endif /* SRC_GTKMAINWINDOW_H_ */
