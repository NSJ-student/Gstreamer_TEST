/*
 * main.cpp
 *
 *  Created on: 2022. 9. 14.
 *      Author: Sujin
 */

#include "GtkMainWindow.h"
#include "GstPlayer.h"

GtkMainWindow * g_mainwindow;
GstPlayer * g_player;

// callback function which is called when application is first started
static void on_app_activate(GApplication *app, gpointer data) {
	g_print("App activated\n");

    g_player = new GstPlayer();
    g_mainwindow = new GtkMainWindow(app, g_player);

	g_print("App activated finish\n");
}

int main(int argc, char *argv[]) {
    // create new GtkApplication with an unique application ID
    GtkApplication *app = gtk_application_new(
        "org.gtkmm.example.libVLC_TEST",
        G_APPLICATION_FLAGS_NONE
    );

    // Initialize GStreamer
    gst_init(NULL, NULL);

    // connect the event-handler for "activate" signal of GApplication
    // G_CALLBACK() macro is used to cast the callback function pointer
    // to generic void pointer
    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);
    // start the application, terminate by closing the window
    // GtkApplication* is upcast to GApplication* with G_APPLICATION() macro
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    // deallocate the application object
    g_object_unref(app);
    return status;
}
