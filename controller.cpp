#include<string.h>
#include "linkedlist.h"
//#define g_print(...) g_print("")

extern gchararray data_save;
extern gchararray data_load;
extern gchar *compiler;
extern gchar *output;
extern gpointer ptr_clip;
extern gpointer ptr_win;
extern gpointer ptr_quit;
extern gpointer ptr_buff;
extern gpointer ptr_note;
extern gpointer ptr_view;
extern gpointer ptr_bundo;
extern gpointer ptr_bredo;
extern gpointer ptr_status;
extern gulong win_handler;
extern gulong quit_handler;
extern gulong context_switch;
extern gulong kill_handler[20];
extern bool exit_mode;
extern bool save_state[20];
extern bool tab_exit_mode[20];
extern bool recognition_mode;
extern bool sin_edit_flag;
extern bool sin_exit_mode;
extern char *global_filename[20];
extern LinkedList *ptr_undo;
extern LinkedList *ptr_redo;
extern LinkedList* undo_array[20];
extern LinkedList* redo_array[20];
extern GtkWidget* page_label[20];
extern GtkWidget* tab_label[20];
extern GtkWidget *label;
extern void quit_widget(GtkWidget*);
extern void file_save(GtkWidget*);

bool edit_flag=true;
bool rec_flag=false;
bool tab_edit_flag[20];
bool consec_undo=false;
unsigned long undo_counter=0;
gint page_no=0;

//void check_sensitivity();

gint delete_event(GtkWidget *widget,GdkEvent *event,gpointer data)
{
	gtk_main_quit();
	return(FALSE);
}

void destroy_event(GtkWidget *w,gpointer data)
{
	gtk_widget_destroy(GTK_WIDGET(data));
}

void display()
{
	for(int i=0;i<gtk_notebook_get_n_pages(GTK_NOTEBOOK(ptr_note));i++)
	{
		g_print("\n");		
		g_print("%d:Undo State Head:%x\n",i,undo_array[i]);	
		g_print("%d:Redo State Head:%x\n",i,redo_array[i]);	
		g_print("%d:Save State:%d\n",i,save_state[i]);	
		g_print("%d:Tab Edit State:%d\n",i,tab_edit_flag[i]);
		g_print("%d:Tab Exit Mode:%d\n",i,tab_exit_mode[i]);	
		g_print("\n");	
	}		

}
void set_exit_mode(GtkWidget *widget,gpointer window)
{
	if(gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(ptr_buff))==TRUE)
	{
		if(edit_flag==true)		
		{			
			g_signal_handler_disconnect(window,win_handler);
			g_signal_handler_disconnect(ptr_quit,quit_handler);
			
			exit_mode=false;			
			edit_flag=false;
					
			win_handler=g_signal_connect(window,"delete_event",G_CALLBACK(quit_widget),NULL);
			quit_handler=g_signal_connect((ptr_quit),"activate",G_CALLBACK(quit_widget),NULL);	
		}	
	}
}

void set_tab_exit_mode(GtkWidget *widget)
{
	if(tab_edit_flag[page_no]==true)	
	{
		if(gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(ptr_buff))==TRUE)		
		{			
			g_print("Page no %d edited\n",page_no);			

			tab_exit_mode[page_no]=false;			
			tab_edit_flag[page_no]=false;
					
		}
	}
}


gboolean mask_events(GtkWidget *widget,GdkEventKey *key,LinkedList *ptr_undo)
{
	set_exit_mode(widget,ptr_win);	
	set_tab_exit_mode(widget);	
	
	if(!(key->state & GDK_CONTROL_MASK))
	{		
		switch(key->keyval)
		{
			case GDK_KEY_space:
			case GDK_KEY_period:
			case GDK_KEY_comma:
			case GDK_KEY_Return:
						if(recognition_mode==true)	
						{		
							tags_highlight(widget);								
							keyword_highlight(widget);	
							rec_flag=false;
						}							

						insert_rear(ptr_undo); 		
			break;			
			
			default:
					if(recognition_mode==true)	
					{		
							tags_highlight(widget);								
							keyword_highlight(widget);	
							rec_flag=false;
					}					
					
					consec_undo=false;
			break;
		}
	}		
	
	if((key->type == GDK_KEY_PRESS) && (key->state & GDK_CONTROL_MASK)) 
	{
		GtkTextBuffer *buffer;		
		buffer=gtk_text_buffer_new(NULL);
		
		switch (key->keyval)
    		{
     			case GDK_KEY_q: 
					if(!exit_mode) quit_widget(widget);
								      	
					else gtk_main_quit();			
			break;				
		
		
			case GDK_KEY_n:
					file_new(widget,ptr_note);
			break;

			case GDK_KEY_o:
					file_tab_open(widget);					
			break;
			
			case GDK_KEY_s:
					if(save_state[page_no]==false)					
					{
						file_save_widget(widget,ptr_buff);
						save_state[page_no]=true;					
					}			
					
					else 		
						file_save(widget);					
								
			break;
			
			case GDK_KEY_f:
					file_search_widget(widget);
			break;
					
			case GDK_KEY_r:
					file_replace_widget(widget,ptr_undo);
			break;		

			case GDK_KEY_p:
					font_widget(widget,ptr_view);	
			break;	

			case GDK_KEY_z:
					if(consec_undo==false)
					{
						insert_rear(ptr_redo);
						undo(ptr_undo);
						undo_counter--;

						consec_undo=true;
					}					

					insert_rear(ptr_redo);					
					g_print("Undo value::%d\n",ptr_undo->x);					
					g_print("Undo Address::%x\n",ptr_undo);					
					undo(ptr_undo);			
					undo_counter--;				
			break;				
		
			case GDK_KEY_v:
					insert_rear(ptr_undo);		
			break;			
	
			case GDK_KEY_m:
					if(undo_counter==0);
			
					else
					{
						insert_rear(ptr_undo);					
						undo(ptr_redo);
						undo_counter++;			
					}
			break;			

			case GDK_KEY_b:
					display(); //This is only for debugging purposes. Displays the current state flags
			break;
			
			default: break;		

		}
	}
	
	if(key->type==GDK_KEY_PRESS && (key->state & GDK_MOD1_MASK))
	{
		if(key->keyval==GDK_KEY_s)
			file_save_widget(widget,ptr_buff);
	}	

	if(recognition_mode==false && rec_flag==false)
	{
		GtkTextIter start,end;		

		gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(ptr_buff),&start,&end);		
		
		gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"data_type",&start,&end);
		gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"control",&start,&end);	
		gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"highlight_bold",&start,&end);	
		gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"highlight_italic",&start,&end);
		gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"#pre",&start,&end);
		gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"string",&start,&end);		
	
		rec_flag=true;	
	}
	
	return FALSE;
}

void set_wrap_mode(GtkWidget *widget,gpointer view)
{
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view),GTK_WRAP_CHAR);

	else gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view),GTK_WRAP_NONE); 
}

void update_statusbar(GtkTextBuffer *buffer,GtkStatusbar *status)
{
  	gchar *msg;
	gint row, col,chars;
	GtkTextIter iter;

  	gtk_statusbar_pop(status,0); 

  	gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(ptr_buff),&iter,gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(ptr_buff)));

	row=gtk_text_iter_get_line(&iter);
  	col=gtk_text_iter_get_line_offset(&iter);
	chars=gtk_text_buffer_get_char_count(GTK_TEXT_BUFFER(ptr_buff));	
	
  	msg=g_strdup_printf("Line %d Column %d Characters %d ", row+1, col+1,chars);

  	gtk_statusbar_push(status, 0, msg);

  	g_free(msg);
}

void mset(GtkTextBuffer *buffer,const GtkTextIter *new_location,GtkTextMark *mark,gpointer data)
{
  	update_statusbar(buffer, GTK_STATUSBAR(data));
}

void set_recognition_mode(GtkWidget *widget)
{
	if(recognition_mode==false)
		recognition_mode=true;
			
	else recognition_mode=false;
	
}

void set_background_color(GtkWidget *widget)
{
	GdkRGBA rgba;
	
	gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(widget),&rgba);
	
	gtk_widget_override_background_color(GTK_WIDGET(ptr_view),GTK_STATE_FLAG_NORMAL,&rgba);
}
	
void set_foreground_color(GtkWidget *widget)
{
	GdkRGBA rgba;

	gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(widget),&rgba);

	gtk_widget_override_color(GTK_WIDGET(ptr_view),GTK_STATE_FLAG_NORMAL,&rgba);
}

void switch_context(GtkNotebook *notebook,GtkWidget *page,guint page_num,gpointer data)
{
	GList *child;	

	child=gtk_container_get_children(GTK_CONTAINER(page));	

	ptr_view=child->data;
	ptr_buff=gtk_text_view_get_buffer(GTK_TEXT_VIEW(ptr_view));

	ptr_undo=undo_array[page_num];	
	ptr_redo=redo_array[page_num];
	
	label=page_label[page_num];	

	page_no=page_num;	

	sin_edit_flag=tab_edit_flag[page_num];
	sin_exit_mode=tab_exit_mode[page_num];	

	g_print("Context switch::Head address:%x\n",ptr_undo);	

	update_statusbar(GTK_TEXT_BUFFER(ptr_buff),GTK_STATUSBAR(ptr_status));	

	g_print("Context switch::Page number:%d\n",page_num);

	g_signal_handler_disconnect(ptr_win,context_switch);	

	context_switch=g_signal_connect(ptr_win,"key-press-event",G_CALLBACK(mask_events),ptr_undo);
}


	
