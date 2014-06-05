#include<iostream>
#include<gtk/gtk.h>
#include<glib/gstdio.h>
#include <gdk/gdkkeysyms.h>
#include<string.h>
#include "instance.cpp"

using namespace std;

int main(int argc,char *argv[])
{
	gtk_init(&argc,&argv);
	
	Instance instance;
 	
	return 0;
}



