#include "linkedlist.h"
//#define g_print(...) g_print("")
extern gchararray str;
extern gchararray data_save;
extern gchararray data_load;
extern gpointer ptr_clip;
extern gpointer ptr_win;
extern gpointer ptr_quit;
extern gpointer ptr_view;
extern gpointer ptr_buff;
extern gpointer ptr_fg;
extern gpointer ptr_bg;
extern gpointer ptr_status;
extern gpointer ptr_note;
extern gulong win_handler;
extern gulong quit_handler;
extern gulong context_switch;
extern float edit_counter;
extern bool exit_mode;
extern bool save_state[20];
extern bool tab_exit_mode[20];
extern bool tab_edit_flag[20];
extern bool sin_edit_flag;
extern bool sin_exit_mode;
extern bool edit_flag;
extern bool highlight_mode;
extern char *global_filename[20];
extern LinkedList* undo_array[20];
extern LinkedList* redo_array[20];
extern GtkWidget* page_label[20];
extern GtkWidget* label;
extern unsigned long undo_counter;
extern LinkedList *ptr_undo;
extern LinkedList *ptr_redo;
extern gint page_no;

extern gint delete_event(GtkWidget*,GdkEvent*,gpointer);
extern void update_statusbar(GtkTextBuffer*,GtkStatusbar*);
extern void mset(GtkTextBuffer*,const GtkTextIter*,GtkTextMark*,gpointer);
extern gboolean mask_events(GtkWidget*,GdkEventKey*,LinkedList*);

bool replace_flag=false;
gchar *replace_data;
gint page_counter=0;

GtkTextMark *mark;

void rehash_context(int);

void quit_tab_widget(GtkWidget*);

void kill_page(GtkButton *button,GtkNotebook *notebook) //Function to kill a page
{
	GtkWidget *page;
	int page_num;

	page=GTK_WIDGET(g_object_get_data(G_OBJECT(button),"page"));
	page_num=gtk_notebook_page_num(notebook,page);
	
	if(tab_exit_mode[page_num]==false)	
		quit_tab_widget(GTK_WIDGET(button));
	
	g_print("Killing page:%d\n",page_num);	

	gtk_notebook_remove_page(notebook,page_num);

	page_counter--;	
			
	rehash_context(page_no);
		
	if(gtk_notebook_get_n_pages(GTK_NOTEBOOK(ptr_note))==1)
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(ptr_note),FALSE);	
}

void file_ok_sel_open(char *data)
{
	g_file_get_contents(data,&data_load,NULL,NULL);
}

void print_screen()
{
	GtkTextIter start,end;	

	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(ptr_buff),&start,&end);
	
	gtk_text_buffer_delete(GTK_TEXT_BUFFER(ptr_buff),&start,&end);
	
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(ptr_buff),&start,data_load,-1);
	g_free(data_load);
}

void file_open_widget(GtkWidget *widget)
{
	GtkWidget *dialog;
	GList *list;    

	char str[50],name[50];
	int j=0,slashmark;
  
     	dialog = gtk_file_chooser_dialog_new ("Open File",NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,NULL);
     
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
       	{
		list=gtk_container_get_children(GTK_CONTAINER(page_label[page_no]));            	

		global_filename[page_no]=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (dialog));
         
		file_ok_sel_open(global_filename[page_no]);		
	
		print_screen();
         
		strcpy(str,global_filename[page_no]);

		g_print("%s\n",str);
		
		g_print("Length:%d\n",strlen(str));		

		for(int i=strlen(str)-1;i>-1;i--)
		{
			g_print("%c",str[i]);			

			if(str[i]=='/') 
			{
				slashmark=i;				
				break;
			}
		}

		g_print("\n");

		j=strlen(str)-slashmark;		
		
		g_print("name:%d\n",j);		

		name[j]='\0';
		
		j--;		

		for(int i=strlen(str)-1;i>slashmark;i--)
		{			
			g_print("%d ",i);
			name[--j]=str[i]; 				
		
			if(j==strlen(str)) break;
		}
		
		g_print("%s\n",name); 		

		gtk_label_set_text(GTK_LABEL(list->data),name);		

		save_state[page_no]=true;		
	}
     
     	gtk_widget_destroy(dialog);
}

void save_buffer(gpointer buffer)
{
	GtkTextIter start,end;	
	
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(buffer),&start,&end);
	data_save=gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer),&start,&end,TRUE);
	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(buffer),FALSE);

	edit_flag=true;		
	
	tab_edit_flag[page_no]=true;	

	g_signal_handler_disconnect(ptr_win,win_handler);
	g_signal_handler_disconnect(ptr_quit,quit_handler);
	
	win_handler=g_signal_connect((ptr_win),"delete_event",G_CALLBACK(delete_event),NULL); 
	quit_handler=g_signal_connect((ptr_quit),"activate",G_CALLBACK(delete_event),NULL);
}

void file_ok_sel_save(char *data)
{
	g_file_set_contents(data,data_save,-1,NULL);
    	g_free(data_save);
}	

void file_save_widget(GtkWidget *widget,gpointer buffer)
{
	GtkWidget *dialog;
	GList *list;     
	char str[50],name[50];
	int j=0,slashmark;     	
	
	dialog = gtk_file_chooser_dialog_new ("Save File",NULL,GTK_FILE_CHOOSER_ACTION_SAVE,GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,NULL);
     	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);
     
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER (dialog),"noname.txt");
     
     
     	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
       	{
		list=gtk_container_get_children(GTK_CONTAINER(page_label[page_no]));         	

		global_filename[page_no]=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (dialog));
         	
		save_buffer(buffer);         	

		g_signal_connect((widget),"delete_event",G_CALLBACK(delete_event),NULL); //Set exit on close			
		exit_mode=true;		

		tab_exit_mode[page_no]=true;		
		
		file_ok_sel_save(global_filename[page_no]);		

		strcpy(str,global_filename[page_no]);

		g_print("%s\n",str);
		
		g_print("Length:%d\n",strlen(str));		

		for(int i=strlen(str)-1;i>-1;i--)
		{
			g_print("%c",str[i]);			

			if(str[i]=='/') 
			{
				slashmark=i;				
				break;
			}
		}

		g_print("\n");

		j=strlen(str)-slashmark;		
		
		g_print("name:%d\n",j);		

		name[j]='\0';
		
		j--;		

		for(int i=strlen(str)-1;i>slashmark;i--)
		{			
			g_print("%d ",i);
			name[--j]=str[i]; 				
		
			if(j==strlen(str)) break;
		}
		
		g_print("%s\n",name); 		

		gtk_label_set_text(GTK_LABEL(list->data),name);			
	}
     
     	gtk_widget_destroy(dialog);
}


void new_widget(GtkWidget *widget)
{
  	GtkWidget *dialog;
	gint response;  	
	
	dialog = gtk_message_dialog_new(NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"File not saved. Save?");
  	
	gtk_window_set_title(GTK_WINDOW(dialog),"File not saved");
	
	response=gtk_dialog_run(GTK_DIALOG(dialog));
  	
	if(response == -9) 
	{
		gtk_widget_destroy(dialog);		
		exit_mode=true;		
		tab_exit_mode[page_no]=true;		
		return;
	}
	
	if(response == -8)
	{	
		file_save_widget(widget,ptr_buff);
		gtk_widget_destroy(dialog);	
		return;	
	}

}

void file_new(GtkWidget *widget,gpointer notebook)
{
	page_counter++;	

	g_print("New page:%d\n",page_counter);	

	PangoFontDescription *font;
	GdkRGBA rgba;
	GdkRGBA fore_color,back_color;	
	GtkWidget *text;
	GtkWidget *vscroll;
	GtkTextBuffer *buffer;
	GtkWidget *tabName;
	GtkWidget *tabContainer;
	GtkWidget *tabClose;		
	
	LinkedList *head;
	LinkedList *l_head;	

	gpointer counter;	

	text=gtk_text_view_new();
	vscroll=gtk_scrolled_window_new(NULL,NULL);	
	buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
	tabName=gtk_label_new("noname.txt");	
	tabContainer=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	tabClose=gtk_button_new_with_label("X");			
	
	ptr_view=text;
	ptr_buff=NULL;		

	head=new LinkedList();
	l_head=new LinkedList();	

	head->x=page_counter;

	ptr_buff=buffer;	

	ptr_undo=head;
	g_print("New tab::Ptr value:%d\n",ptr_undo->x);	
	ptr_redo=l_head;	

	label=tabContainer;	

	g_print("New tab::Ptr address:%x\n",ptr_undo);		

	undo_array[page_counter]=head;	
	redo_array[page_counter]=l_head;	

	save_state[page_counter]=false;	

	tab_edit_flag[page_counter]=true;	
	tab_exit_mode[page_counter]=true;
	
	sin_edit_flag=tab_edit_flag[page_counter];
	sin_exit_mode=tab_exit_mode[page_counter];	

	page_label[page_counter]=tabContainer;	

	font=pango_font_description_from_string("Monospace 15");
 	gtk_widget_modify_font(text,font);
  	pango_font_description_free(font);	
	
	gdk_rgba_parse(&rgba,"rgb(0,0,0)");
	gtk_widget_override_background_color(text,GTK_STATE_FLAG_NORMAL,&rgba);	
	
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer),"red_bg","background","red",NULL); 
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer),"data_type","foreground","green",NULL);
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer),"control","foreground","red",NULL);
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer),"string","foreground","purple",NULL);	
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer),"#pre","background","blue",NULL);	
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer),"misc","foreground","yellow",NULL);	
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer),"sin_comment","foreground","blue",NULL);	
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer),"highlight_bold","weight",PANGO_WEIGHT_BOLD,NULL);
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer),"highlight_italic","style",PANGO_STYLE_ITALIC,NULL);
	gtk_container_add(GTK_CONTAINER(vscroll),text);	

	gtk_box_pack_start(GTK_BOX(tabContainer),tabName,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(tabContainer),tabClose,FALSE,FALSE,0);
		
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),vscroll,tabContainer);

	gtk_widget_show_all(vscroll);
	gtk_widget_show_all(tabContainer);
	
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook),page_counter);

	gdk_rgba_parse(&fore_color,"rgb(255,255,255)");	
	gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(ptr_fg),&fore_color);

	gdk_rgba_parse(&back_color,"rgb(0,0,0)");
	gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(ptr_bg),&back_color);

	g_signal_connect(ptr_buff,"changed",G_CALLBACK(update_statusbar),ptr_status);
	g_signal_connect_object(ptr_buff,"mark_set",G_CALLBACK(mset),ptr_status,G_CONNECT_AFTER);
	
	g_signal_handler_disconnect(ptr_win,context_switch);	
	
	context_switch=g_signal_connect(ptr_win,"key-press-event",G_CALLBACK(mask_events),ptr_undo);

	if(gtk_notebook_get_n_pages(GTK_NOTEBOOK(ptr_note))>1)
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(ptr_note),TRUE);

	g_object_set_data(G_OBJECT(tabClose),"page",vscroll);	

	g_signal_connect(tabClose,"clicked",G_CALLBACK(kill_page),ptr_note);
}

void file_open(GtkWidget *widget)
{
	file_open_widget(widget);
}	

void file_save(GtkWidget *widget)
{
	save_buffer(ptr_buff);
	file_ok_sel_save((char*)global_filename[page_no]);
}

void file_tab_open(GtkWidget *widget)
{
	if(global_filename[page_no]==NULL)	
		file_open(widget);
	
	else
	{	
		file_new(widget,ptr_note);					
		file_open(widget);
	}
}	


void keyword_highlight(GtkWidget *widget)
{
	GtkTextIter start_search,end_search;
	GtkTextIter start_match,end_match;
	GtkTextIter apply_start,apply_end;	
	GtkTextMark *mark; 
	
	gint offset;
	
	const char data_type[10][10]=
	{
		"int ",
		"char ",
		"float ",	
		"double ",
		"void ",
		"short ",
		"long ",
		"bool ",
		"unsigned ",
		"signed ",
	};	
	
	const char control[11][10]=
	{
		"if",
		"else ",
		"switch",
		"case ",
		"do\n",
		"while",
		"for",
		"break;",
		"continue;",
		"return ",
		"do "	
	};
			
	const char misc[13][10]=
	{
		"auto",
		"const",
		"default",
		"enum",
		"extern",
		"goto",
		"register",
		"sizeof",
		"static",
		"struct",
		"typedef",
		"union",
		"volatile"
	};	

	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(ptr_buff),&start_search,&end_search);	
	
	for(int i=0;i<10;i++)
	{	
		while(gtk_text_iter_forward_search(&start_search,data_type[i],GTK_TEXT_SEARCH_TEXT_ONLY,&start_match,&end_match,&end_search)) 
		{
			gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"control",&start_match,&end_match);				
			gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"highlight_bold",&start_match,&end_match);	
			
			gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"data_type",&start_match,&end_match);
          		gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"highlight_bold",&start_match,&end_match);
			start_search=end_match;
		}
			
	}
	
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(ptr_buff),&start_search,&end_search);		
	
	for(int i=0;i<11;i++)
	{
		while(gtk_text_iter_forward_search(&start_search,control[i],GTK_TEXT_SEARCH_TEXT_ONLY,&start_match,&end_match,&end_search)) 
		{
			gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"data_type",&start_match,&end_match);
			gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"highlight_bold",&start_match,&end_match);			

			gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"control",&start_match,&end_match);
			gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"highlight_bold",&start_match,&end_match);          		
			start_search=end_match;
        	}
	}	

	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(ptr_buff),&start_search,&end_search);		
	
	for(int i=0;i<13;i++)
	{
		while(gtk_text_iter_forward_search(&start_search,misc[i],GTK_TEXT_SEARCH_TEXT_ONLY,&start_match,&end_match,&end_search)) 
		{
			gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"data_type",&start_match,&end_match);
			gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"highlight_bold",&start_match,&end_match);			

			gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"misc",&start_match,&end_match);
			gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"highlight_bold",&start_match,&end_match);          		
			start_search=end_match;
        	}
	}
}

void tags_highlight(GtkWidget *widget)
{
	GtkTextIter start_search,end_search;
	GtkTextIter start_match,end_match;
	GtkTextMark *apply_start,*apply_end; 	
	gint line_no;
	
	gboolean tag=false;	

	const char string[3]={"\""};	
	const char preprocessor_directive[2]={"#"};
	const char single_comment[3]={"//"};	

	for(int i=0;i<gtk_text_buffer_get_line_count(GTK_TEXT_BUFFER(ptr_buff));i++)
	{	
		gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(ptr_buff),&start_search,i);
		gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(ptr_buff),&end_search);		
		
		while(gtk_text_iter_forward_search(&start_search,preprocessor_directive,GTK_TEXT_SEARCH_TEXT_ONLY,&start_match,&end_match,&end_search)) 
		{
			gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(ptr_buff),&end_match,gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(ptr_buff)));
		
			if(gtk_text_iter_get_line(&end_match)==i)
				gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"#pre",&start_match,&end_match);          		
		
			start_search=end_match;
        	}
	}

	for(int i=0;i<gtk_text_buffer_get_line_count(GTK_TEXT_BUFFER(ptr_buff));i++)
	{	
		gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(ptr_buff),&start_search,i);
		gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(ptr_buff),&end_search);		
		
		while(gtk_text_iter_forward_search(&start_search,single_comment,GTK_TEXT_SEARCH_TEXT_ONLY,&start_match,&end_match,&end_search)) 
		{
			gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(ptr_buff),&end_match,gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(ptr_buff)));
		
			if(gtk_text_iter_get_line(&end_match)==i)
				gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"sin_comment",&start_match,&end_match);          		
		
			start_search=end_match;
        	}
	}	

	for(int i=0;i<gtk_text_buffer_get_line_count(GTK_TEXT_BUFFER(ptr_buff));i++)
	{	
		gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(ptr_buff),&start_search,i);
		gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(ptr_buff),&end_search);		
		
		while(gtk_text_iter_forward_search(&start_search,string,GTK_TEXT_SEARCH_TEXT_ONLY,&start_match,&end_match,&end_search)) 
		{
			switch(tag)
			{	
				case false:
						line_no=gtk_text_iter_get_line(&start_match);						

						g_print("Tag start:%d\n",line_no);						

						apply_start=gtk_text_mark_new(NULL,FALSE);
						apply_start=gtk_text_buffer_create_mark(GTK_TEXT_BUFFER(ptr_buff),NULL,&start_match,FALSE);
						tag=true;
				break;			
				
				case true:
						apply_end=gtk_text_mark_new(NULL,FALSE);
						apply_end=gtk_text_buffer_create_mark(GTK_TEXT_BUFFER(ptr_buff),NULL,&start_match,FALSE);
						
						gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(ptr_buff),&start_match,apply_start);
						gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(ptr_buff),&end_match,apply_end);
										
						gtk_text_iter_forward_char(&end_match);
						
						g_print("Tag end:%d\n",gtk_text_iter_get_line(&end_match));
						
						if(gtk_text_iter_get_line(&end_match) == line_no)						
						{							
							gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"string",&start_match,&end_match);
							gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"highlight_italic",&start_match,&end_match);
						}
				
						tag=false;
				break;			
			}

			start_search=end_match;
        	}
	}
}

void search_highlight(GtkWidget *widget,gpointer entry_buff)
{
	GtkTextIter start_search,end_search,start_match,end_match; 	
	const char *text;	
	gboolean selection;	
	
	text=gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(entry_buff));	
	
	selection=gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(ptr_buff),&start_search,&end_search);

	if(selection==FALSE)
		gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(ptr_buff),&start_search,&end_search);	

	gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"red_bg",&start_search,&end_search);	

	while(gtk_text_iter_forward_search(&start_search,(char*)text,GTK_TEXT_SEARCH_CASE_INSENSITIVE,&start_match,&end_match,&end_search)) 
	{
		gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"red_bg",&start_match,&end_match);
          	start_search=end_match;
        }
}

void file_search_widget(GtkWidget *widget)
{
	GtkWidget *dialog;
	GtkWidget *content_area;	
	GtkWidget *search;
	GtkWidget *label;
	GtkEntryBuffer *buffer;		
	gboolean value;	
	
	g_print("Segfault!\n");		
	dialog=gtk_dialog_new_with_buttons("Find",NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_STOCK_OK,GTK_RESPONSE_ACCEPT,GTK_STOCK_CANCEL,GTK_RESPONSE_REJECT);	
	g_print("Segfault!\n");		
	content_area=gtk_dialog_get_content_area(GTK_DIALOG(dialog));	
	label=gtk_label_new("Search for:");
	buffer=gtk_entry_buffer_new(NULL,-1);	
	search=gtk_entry_new_with_buffer(buffer);
	
	gtk_container_add(GTK_CONTAINER(content_area),label);	
	gtk_container_add(GTK_CONTAINER(content_area),search);

	g_signal_connect_swapped(dialog,"response",G_CALLBACK(gtk_widget_destroy),dialog);	

	gtk_widget_show_all(dialog);

	value=gtk_dialog_run(GTK_DIALOG(dialog));

	g_print("%d\n",value);	
	
	if(value == -1)
	{
		search_highlight(dialog,buffer);
	}
}

void file_replace_widget(GtkWidget *widget,LinkedList *head)
{
	GtkWidget *dialog;
	GtkWidget *content_area;	
	GtkWidget *search;
	GtkWidget *replace;	
	GtkWidget *search_label;
	GtkWidget *replace_label;
	GtkEntryBuffer *search_buffer;		
	GtkEntryBuffer *replace_buffer;	
	GtkTextIter start_search,end_search,start_match,end_match; 		
	GtkTextMark *start_mark;
	GtkTextMark *end_mark;	
	GtkTextMark *buff_start_mark;
	GtkTextMark *buff_end_mark;
	const char *text_search;
	const char *text_replace;	
	gboolean value;	
	
	g_print("Segfault!\n");
	dialog=gtk_dialog_new_with_buttons("Replace",NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_STOCK_OK,GTK_RESPONSE_ACCEPT,GTK_STOCK_CANCEL,GTK_RESPONSE_REJECT);
	g_print("Segfault!\n");	
	content_area=gtk_dialog_get_content_area(GTK_DIALOG(dialog));	
	search_label=gtk_label_new("Search for:");
	replace_label=gtk_label_new("Replace with:");
	search_buffer=gtk_entry_buffer_new(NULL,-1);	
	search=gtk_entry_new_with_buffer(search_buffer);
	replace_buffer=gtk_entry_buffer_new(NULL,-1);
	replace=gtk_entry_new_with_buffer(replace_buffer);	
	
	gtk_container_add(GTK_CONTAINER(content_area),search_label);	
	gtk_container_add(GTK_CONTAINER(content_area),search);
	gtk_container_add(GTK_CONTAINER(content_area),replace_label);	
	gtk_container_add(GTK_CONTAINER(content_area),replace);
	
	g_signal_connect_swapped(dialog,"response",G_CALLBACK(gtk_widget_destroy),dialog);	

	gtk_widget_show_all(dialog);

	value=gtk_dialog_run(GTK_DIALOG(dialog));

	if(value == -1)
	{
		undo_counter++;			
		insert_rear(ptr_undo);		

		gboolean selection;		
		
		text_search=gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(search_buffer));	
		text_replace=gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(replace_buffer));		
		
		selection=gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(ptr_buff),&start_search,&end_search);

		if(selection==FALSE)
			gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(ptr_buff),&start_search,&end_search);
			
		gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(ptr_buff),"red_bg",&start_search,&end_search);	

		start_mark=gtk_text_mark_new(NULL,TRUE);
		end_mark=gtk_text_mark_new(NULL,TRUE);		

		buff_start_mark=gtk_text_mark_new(NULL,TRUE);
		buff_end_mark=gtk_text_mark_new(NULL,TRUE);

		gtk_text_buffer_add_mark(GTK_TEXT_BUFFER(ptr_buff),buff_start_mark,&start_search);
		gtk_text_buffer_add_mark(GTK_TEXT_BUFFER(ptr_buff),buff_end_mark,&end_search);		

		while(gtk_text_iter_forward_search(&start_search,(char*)text_search,GTK_TEXT_SEARCH_CASE_INSENSITIVE,&start_match,&end_match,&end_search)) 
		{
			gtk_text_buffer_add_mark(GTK_TEXT_BUFFER(ptr_buff),start_mark,&start_match);			
			gtk_text_buffer_add_mark(GTK_TEXT_BUFFER(ptr_buff),end_mark,&end_match);			
			
			gtk_text_buffer_delete(GTK_TEXT_BUFFER(ptr_buff),&start_match,&end_match);
				
			gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(ptr_buff),&start_match,start_mark);			
			gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(ptr_buff),&end_match,end_mark);			
			gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(ptr_buff),&start_search,buff_start_mark);
			gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(ptr_buff),&end_search,buff_end_mark);			
					
			gtk_text_buffer_insert(GTK_TEXT_BUFFER(ptr_buff),&start_match,text_replace,-1);
          		
			gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(ptr_buff),&start_match,start_mark);			
			gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(ptr_buff),&end_match,end_mark);			
			gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(ptr_buff),&start_search,buff_start_mark);
			gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(ptr_buff),&end_search,buff_end_mark);			
					
			gtk_text_buffer_delete_mark(GTK_TEXT_BUFFER(ptr_buff),start_mark);
			gtk_text_buffer_delete_mark(GTK_TEXT_BUFFER(ptr_buff),end_mark);			
		
			start_search=end_match;
        	}			
	

	}
}

void font_widget(GtkWidget *widget,gpointer text)
{
	GtkWidget *font_dialog;
	gint value;	
	gboolean selected;	
	GtkTextIter start,end;	
	GtkTextTag *tag;	
	PangoFontDescription *font;
	
	font_dialog=gtk_font_chooser_dialog_new("Font",NULL);
	
	gtk_font_chooser_set_preview_text(GTK_FONT_CHOOSER(font_dialog),"Kawaii Font Chooser");		

	value=gtk_dialog_run(GTK_DIALOG(font_dialog));
	
	if(value== -5)
	{
		font=gtk_font_chooser_get_font_desc(GTK_FONT_CHOOSER(font_dialog));
				
		selected=gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(ptr_buff),&start,&end);		

		
		if(selected == FALSE)
			gtk_widget_override_font(GTK_WIDGET(text),font);		

		else
		{
			tag=gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(ptr_buff),NULL,"font-desc",font,NULL);
		
			gtk_text_buffer_remove_tag(GTK_TEXT_BUFFER(ptr_buff),tag,&start,&end);			

			gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(ptr_buff),tag,&start,&end);		
		}
		
		gtk_widget_destroy(GTK_WIDGET(font_dialog));
		
	}

	if(value== -6)
	{
		gtk_widget_destroy(font_dialog);
	}
}

void cut_slice(GtkWidget *widget,gpointer buffer)
{
	gtk_text_buffer_cut_clipboard(GTK_TEXT_BUFFER(buffer),GTK_CLIPBOARD(ptr_clip),TRUE);	
}

void copy_slice(GtkWidget *widget,gpointer buffer)
{
	gtk_text_buffer_copy_clipboard(GTK_TEXT_BUFFER(buffer),GTK_CLIPBOARD(ptr_clip));	
}

void paste_slice(GtkWidget *widget,gpointer buffer)
{
	gtk_text_buffer_paste_clipboard(GTK_TEXT_BUFFER(buffer),GTK_CLIPBOARD(ptr_clip),NULL,TRUE);	
}

void quit_widget(GtkWidget *widget)
{
  	GtkWidget *dialog;
	gint response;  	
	
	dialog = gtk_message_dialog_new(NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"You have unsaved file(s).\nAre you sure you want to quit?");
  	
	gtk_window_set_title(GTK_WINDOW(dialog),"Unsaved File(s)");
	
	response=gtk_dialog_run(GTK_DIALOG(dialog));
  	
	if(response == -8) gtk_main_quit();
	
	if(response == -9) gtk_widget_destroy(dialog);
}

void quit_tab_widget(GtkWidget *widget)
{
	GtkWidget *dialog;
	gint response;  	
	
	dialog=gtk_message_dialog_new(NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"File not saved. Save?");
  	
	gtk_window_set_title(GTK_WINDOW(dialog),"File not saved");
	
	response=gtk_dialog_run(GTK_DIALOG(dialog));
 
 	if(response == -9);
	
	if(response == -8)
	{	
		if(save_state[page_no]==false)		
			file_save_widget(GTK_WIDGET(widget),ptr_buff);
		
		else file_save(GTK_WIDGET(widget));	
	}
	
	gtk_widget_destroy(dialog);
}

void about_widget(GtkWidget *widget,gpointer window)
{
	GtkWidget *dialog;
	GdkPixbuf *pixbuf;  	

	dialog = gtk_about_dialog_new();
	pixbuf = gdk_pixbuf_new_from_file("Kawaii_help.png",NULL);
	
	gtk_show_about_dialog(NULL,"program-name", "Kawaii v4.0.\nSystem Software project of: \nSrivathsan J,1BM11CS043\nJagriti Pandey,1BM11CS044\n","logo", pixbuf,"title",NULL);
}

void controls_widget(GtkWidget *widget,gpointer window)
{
	GtkWidget *dialog;
	
	dialog=gtk_about_dialog_new();
	
	gtk_show_about_dialog(NULL,"program-name","Ctrl+q Quit Kawaii\nCtrl+n New tab\nCtrl+o Open file\nCtrl+s Save file\nCtrl+s Search\nCtrl+r Search and Replace\nCtrl+p Fonts\nCtrl+z Undo\nCtrl+m Redo\nCtrl+x Cut\nCtrl+c Copy\nCtrl+v Paste\nCtrl+a Select All\n","logo",NULL,"title",NULL);
}

void file_undo(GtkWidget *widget)
{
	insert_rear(ptr_redo);
	undo(ptr_undo);
	undo_counter--;
}

void file_redo(GtkWidget *widget)
{
	if(undo_counter==0);

	else
	{	
		insert_rear(ptr_undo);
		undo(ptr_redo);
		undo_counter++;
	}
}

void file_hide_toolbar(GtkWidget *widget,gpointer toolbar)
{
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) gtk_widget_show(GTK_WIDGET(toolbar));

	else  gtk_widget_hide(GTK_WIDGET(toolbar)); 
}

void file_hide_statusbar(GtkWidget *widget,gpointer status)
{
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) gtk_widget_show(GTK_WIDGET(status));

	else  gtk_widget_hide(GTK_WIDGET(status)); 
}

void rehash_context(int page_num)
{
	g_print("REHASHING CONTEXT TABLE!\n");	

	undo_array[page_num]=NULL;
	redo_array[page_num]=NULL;
		
	g_print("Pages:%d\n",gtk_notebook_get_n_pages(GTK_NOTEBOOK(ptr_note)));	

	for(int i=page_num;i<gtk_notebook_get_n_pages(GTK_NOTEBOOK(ptr_note));i++)
	{
		undo_array[i]=undo_array[i+1];
		redo_array[i]=redo_array[i+1];	
		save_state[i]=save_state[i+1];
		tab_edit_flag[i]=tab_edit_flag[i+1];
		tab_exit_mode[i]=tab_exit_mode[i+1];	
	}
}

