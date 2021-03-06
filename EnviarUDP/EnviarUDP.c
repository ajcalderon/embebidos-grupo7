#include <gst/gst.h>
#include <stdio.h>

int main(int argc, char *argv[]){
	
	GMainLoop *loop;

	GstElement *pipeline, *source, *mad, *audioconvert, *rtpL16pay, *udpsink;

	gst_init (&argc, &argv);

	loop = g_main_loop_new (NULL, FALSE);

	if (argc != 2) {
		g_printerr ("Usage: %s <Ogg/Vorbis filename>\n", argv[0]);
    	return -1;
	}

	//gst-launch filesrc location=get_lucky.mp3 ! mad ! audioconvert ! rtpL16pay ! udpsink host=192.168.0.100 port=5000

	pipeline    = gst_pipeline_new ("audio-player");
	source      = gst_element_factory_make ("filesrc","file-source");  
	mad         = gst_element_factory_make ("mad","mad");  //primero es le verdadero, el segundo es un sobrenombre
	audioconvert= gst_element_factory_make ("audioconvert","audioconvert");
	rtpL16pay   = gst_element_factory_make ("rtpL16pay","rtpL16pay");
	udpsink     = gst_element_factory_make ("udpsink","udpsink");

	g_object_set( udpsink, "host", "192.168.0.100", "port", 5000, NULL );

	if (!pipeline || !source || !mad || !audioconvert || !rtpL16pay || !udpsink) {
		g_printerr ("One element could not be created. Exiting.\n");
		return -1;
  	}

  	g_object_set (G_OBJECT (source), "location", argv[1], NULL);

	gst_bin_add_many (GST_BIN (pipeline), source, mad, audioconvert, rtpL16pay, udpsink, NULL);

	gst_element_link_many (source, mad, audioconvert, rtpL16pay, udpsink, NULL);
	
	g_print ("Now playing: %s\n", argv[1]);
	gst_element_set_state (pipeline, GST_STATE_PLAYING);

	g_print ("Running...\n");
	g_main_loop_run (loop);

	g_print ("Returned, stopping playback\n");
	gst_element_set_state (pipeline, GST_STATE_NULL);

	g_print ("Deleting pipeline\n");
	gst_object_unref (GST_OBJECT (pipeline));
	g_main_loop_unref (loop);

	return 0;

}
