#include "menu.cpp"
#include "controller.cpp"

gchararray data_save;
gchararray data_load;
gchar *compiler;
gchar *output;
gpointer ptr_clip;
gpointer ptr_win;
gpointer ptr_quit;
gpointer ptr_buff;
gpointer ptr_view;
gpointer ptr_note;
gpointer ptr_bundo;
gpointer ptr_bredo;
gpointer ptr_fg;
gpointer ptr_bg;
gpointer ptr_status;
gulong win_handler;
gulong quit_handler;
gulong context_switch;
bool exit_mode=true;
bool tab_exit_mode[20];
bool recognition_mode=false;
bool highlight_mode=false;
bool save_state[20];
bool sin_exit_mode;
bool sin_edit_flag;
LinkedList *ptr_undo;
LinkedList *ptr_redo;
LinkedList* undo_array[20];
LinkedList* redo_array[20];
GtkWidget* page_label[20];
GtkWidget* label;
char *global_filename[20];

class Instance
{
	private:
		GtkWidget *window;
		GtkWidget *notebook;	
		GtkWidget *text;		
		GtkWidget *box;
		GtkWidget *vscroll;	
		GtkWidget *toolbar;		
		GtkWidget *menubar;
  		GtkWidget *filemenu;
  		GtkWidget *file;
		GtkWidget *New;		
		GtkWidget *open;  		
		GtkWidget *save;	
		GtkWidget *save_as;		
		GtkWidget *quit;
		GtkWidget *helpmenu;
		GtkWidget *help;
		GtkWidget *formatmenu;		
		GtkWidget *format;
		GtkWidget *editmenu;
		GtkWidget *edit;		
		GtkWidget *cut;
		GtkWidget *copy;
		GtkWidget *paste;
		GtkWidget *undo;		
		GtkWidget *redo;		
		GtkWidget *searchmenu;
		GtkWidget *search;	
		GtkWidget *find;
		GtkWidget *replace;		
		GtkWidget *font;
		GtkWidget *wrap;				
		GtkWidget *mode;
		GtkWidget *about;
		GtkWidget *controls;
		GtkWidget *viewmenu;
		GtkWidget *view;		
		GtkWidget *toolview;
		GtkWidget *statusview;
		GtkWidget *status;		
		GtkWidget *bg_color;
		GtkWidget *fg_color;		
		
		GtkTextBuffer *buffer;				
		GtkClipboard *board;
		
		GtkToolItem *t_new;
		GtkToolItem *t_open;
		GtkToolItem *t_save;		
		GtkToolItem *t_undo;		
		GtkToolItem *t_redo;
		GtkToolItem *t_cut;
		GtkToolItem *t_copy;
		GtkToolItem *t_paste;		
		GtkToolItem *t_sep1;
		GtkToolItem *t_sep2;
		GtkToolItem *t_sep3;				
		
		GtkAccelGroup *accels;		

		LinkedList *head;
		LinkedList *l_redo;		
		
		void Engine();
	
		void Init();				
		
		void Window();

		void Add();
	
		void Pack();	
		
		void Menu();
			
		void NoteBook();		
	
		void ToolBar();		
		
		void Color();		
		
		void Text();
		
		void Status();		

		void ShowAll();

	public:
		Instance()
		{
			Engine();
		}
};		

void Instance::Status()
{
	g_signal_connect(ptr_buff,"changed",G_CALLBACK(update_statusbar),status);
	g_signal_connect_object(ptr_buff,"mark_set",G_CALLBACK(mset),status,G_CONNECT_AFTER);
}

void Instance::Init()
{
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	box=gtk_box_new(GTK_ORIENTATION_VERTICAL,0); //Set box as non homogenous	
	vscroll=gtk_scrolled_window_new(NULL,NULL);	
	status=gtk_statusbar_new();	
	accels=gtk_accel_group_new();				
	bg_color=gtk_color_button_new();	
	fg_color=gtk_color_button_new();	
	
	menubar=gtk_menu_bar_new(); //Sets parent menubar
	filemenu=gtk_menu_new(); //Sets child menu 	
	helpmenu=gtk_menu_new(); //Sets child menu	
	formatmenu=gtk_menu_new(); //Sets child menu
	editmenu=gtk_menu_new();//sets child menu
	searchmenu=gtk_menu_new();//sets search menu	
	viewmenu=gtk_menu_new();//sets view menu	
	
	file=gtk_menu_item_new_with_label("File"); //Sets menu tab
	New=gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW,accels);
  	open=gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN,accels);
	save=gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE,accels);//set with accelerator
	save_as=gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE_AS,accels);	
	quit = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT,accels);//set with accelerator
	format=gtk_menu_item_new_with_label("Format");
	font=gtk_image_menu_item_new_from_stock(GTK_STOCK_SELECT_FONT,accels);
	wrap=gtk_check_menu_item_new_with_label("Word Wrap");	
	help=gtk_menu_item_new_with_label("Help"); //Sets menu tab
	about=gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT,accels);	
	controls=gtk_menu_item_new_with_label("Controls");	
	edit=gtk_menu_item_new_with_label("Edit");
	cut=gtk_image_menu_item_new_from_stock(GTK_STOCK_CUT,accels);
	copy=gtk_image_menu_item_new_from_stock(GTK_STOCK_COPY,accels);
	paste=gtk_image_menu_item_new_from_stock(GTK_STOCK_PASTE,accels);
	undo=gtk_image_menu_item_new_from_stock(GTK_STOCK_UNDO,accels);
	redo=gtk_image_menu_item_new_from_stock(GTK_STOCK_REDO,accels);	
	search=gtk_menu_item_new_with_label("Search");	
	find=gtk_image_menu_item_new_from_stock(GTK_STOCK_FIND,accels);
	replace=gtk_image_menu_item_new_from_stock(GTK_STOCK_FIND_AND_REPLACE,accels);
	view=gtk_menu_item_new_with_label("View");
	toolview=gtk_check_menu_item_new_with_label("Toolbar");
	statusview=gtk_check_menu_item_new_with_label("Statusbar");		
	mode=gtk_check_menu_item_new_with_label("C Language");
		
	t_new=gtk_tool_button_new_from_stock(GTK_STOCK_NEW);	
	t_open=gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);	
	t_save=gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);	
	t_undo=gtk_tool_button_new_from_stock(GTK_STOCK_UNDO);	
	t_redo=gtk_tool_button_new_from_stock(GTK_STOCK_REDO);	
	t_cut=gtk_tool_button_new_from_stock(GTK_STOCK_CUT);
	t_copy=gtk_tool_button_new_from_stock(GTK_STOCK_COPY);
	t_paste=gtk_tool_button_new_from_stock(GTK_STOCK_PASTE);	
	
	t_sep1=gtk_separator_tool_item_new();
	t_sep2=gtk_separator_tool_item_new();
	t_sep3=gtk_separator_tool_item_new(); 		

	text=gtk_text_view_new();
	notebook=gtk_notebook_new();	
	buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));	
	toolbar=gtk_toolbar_new();	
	board=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	
	head=new LinkedList();
	l_redo=new LinkedList();	
	
	head->x=0;	
	
	ptr_clip=board;
	ptr_win=window;
	ptr_quit=quit;
	ptr_buff=buffer;
	ptr_view=text;	
	ptr_note=notebook;	
	ptr_bundo=t_undo;
	ptr_bredo=t_redo;		
	ptr_undo=head;
	ptr_redo=l_redo;
	ptr_status=status;	
	ptr_fg=fg_color;
	ptr_bg=bg_color;
	ptr_status=status;
	
	undo_array[0]=head;
	redo_array[0]=l_redo;
	save_state[0]=false;
	tab_exit_mode[0]=true;	
	tab_edit_flag[0]=true;

	sin_edit_flag=tab_edit_flag[0];
	sin_exit_mode=tab_exit_mode[0];	
}

void Instance::Engine()
{
	Init();
	Window();			
	Status();	
	Text();		
	Menu();		
	NoteBook();	
	ToolBar();	
	Color();	
	Pack();
	Add();	
	ShowAll();
}

void Instance::Add()
{
	gtk_container_add(GTK_CONTAINER(status),fg_color);	
	gtk_container_add(GTK_CONTAINER(status),bg_color);	
	gtk_container_add(GTK_CONTAINER(vscroll),text);	
	gtk_container_add(GTK_CONTAINER(window),box); 
}

void Instance::Pack()
{
	gtk_box_pack_start(GTK_BOX(box),menubar, FALSE, FALSE, 0); //pack to box
	
	gtk_box_pack_start(GTK_BOX(box),toolbar,FALSE,FALSE,0);	

	gtk_box_pack_start(GTK_BOX(box),notebook,TRUE,TRUE,0);

	gtk_box_pack_start(GTK_BOX(box),status,FALSE,FALSE,0);
}

void Instance::NoteBook()
{
	GtkWidget *tabName;
	GtkWidget *tabContainer;
	GtkWidget *tabClose;		
	
	tabName=gtk_label_new("noname.txt");	
	tabContainer=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	tabClose=gtk_button_new_with_label("X");	
	
	gtk_box_pack_start(GTK_BOX(tabContainer),tabName,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(tabContainer),tabClose,FALSE,FALSE,0);	
	
	gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));
	
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook),TRUE);	

	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook),FALSE);	

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),vscroll,tabContainer);

	g_signal_connect(notebook,"switch-page",G_CALLBACK(switch_context),NULL);

	gtk_widget_show_all(tabContainer);	

	label=tabContainer;

	page_label[0]=tabContainer;

	g_object_set_data(G_OBJECT(tabClose),"page",vscroll);	

	g_signal_connect(tabClose,"clicked",G_CALLBACK(kill_page),notebook);
}

void Instance::Window()
{
	GdkPixbuf *icon;	
	
	//Sets window to be topmost for DE
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);	
	gtk_window_set_default_size(GTK_WINDOW(window),600,600); //Set size
	gtk_window_set_title(GTK_WINDOW(window),"Kawaii"); //Set Title	
	
	icon=gdk_pixbuf_new_from_file("Kawaii.png",NULL);
	
	gtk_window_set_icon(GTK_WINDOW(window),icon);
	  
	win_handler=g_signal_connect((window),"delete_event",G_CALLBACK(delete_event),NULL); 	

	context_switch=g_signal_connect(window,"key-press-event",G_CALLBACK(mask_events),ptr_undo);
}

void Instance::Menu()
{
	g_signal_connect((open),"activate",G_CALLBACK(file_tab_open),NULL); //opens file widget	
	g_signal_connect((New),"activate",G_CALLBACK(file_new),ptr_buff);//opens empty text file
	g_signal_connect((save),"activate",G_CALLBACK(file_save),NULL); //opens file widget
	g_signal_connect((save_as),"activate",G_CALLBACK(file_save_widget),ptr_buff); //opens file widget	
	g_signal_connect((font),"activate",G_CALLBACK(font_widget),ptr_view); //opens font widget	
	g_signal_connect((wrap),"activate",G_CALLBACK(set_wrap_mode),ptr_view);	
	g_signal_connect((about),"activate",G_CALLBACK(about_widget),(window)); //opens about widget	
	g_signal_connect((cut),"activate",G_CALLBACK(cut_slice),ptr_buff);//cut implementation
	g_signal_connect((copy),"activate",G_CALLBACK(copy_slice),ptr_buff);//copy implementation
	g_signal_connect((paste),"activate",G_CALLBACK(paste_slice),ptr_buff);//paste implementation
	g_signal_connect((undo),"activate",G_CALLBACK(file_undo),NULL);	
	g_signal_connect((redo),"activate",G_CALLBACK(file_redo),NULL);	
	g_signal_connect((find),"activate",G_CALLBACK(file_search_widget),NULL);//find implementation
	g_signal_connect((replace),"activate",G_CALLBACK(file_replace_widget),ptr_undo);//replace implementation	
	g_signal_connect((toolview),"activate",G_CALLBACK(file_hide_toolbar),toolbar);	
	g_signal_connect((statusview),"activate",G_CALLBACK(file_hide_statusbar),status);	
	g_signal_connect((mode),"activate",G_CALLBACK(set_recognition_mode),NULL);	
	g_signal_connect((controls),"activate",G_CALLBACK(controls_widget),window);	
	
	quit_handler=g_signal_connect((quit),"activate",G_CALLBACK(delete_event),NULL); //Sets exit on quit
	
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file),filemenu);//sets submenu
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit),editmenu);//sets submenu	
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view),viewmenu);//sets submenu	
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(search),searchmenu);//sets submenu	
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(format),formatmenu);//sets submenu
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(help),helpmenu);//sets submenu
		
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), New);//append filemenu
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), open);//append filemenu  	
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), save);//append filemenu	
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), save_as);//append filemenu		
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);//append filemenu
	gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu), about);//append helpmenu
	gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu), controls);//append helpmenu
	gtk_menu_shell_append(GTK_MENU_SHELL(formatmenu), font);//append formatmenu
	gtk_menu_shell_append(GTK_MENU_SHELL(formatmenu), wrap);//append formatmenu	
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), cut);//append editmenu		
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), copy);//append editmenu	
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), paste);//append editmenu	
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), undo);	
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), redo);	
	gtk_menu_shell_append(GTK_MENU_SHELL(searchmenu), find);//append searchmenu
	gtk_menu_shell_append(GTK_MENU_SHELL(searchmenu), replace);//append searchmenu	
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), toolview);
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), statusview);	
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), mode);
		
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);//append filebar	
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), edit);//append editbar	
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), view);//append viewbar	
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), search);//append searchbar	
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), format);//append formatbar	
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), help);//format helpbar

	gtk_menu_item_activate(GTK_MENU_ITEM(wrap));
	gtk_menu_item_activate(GTK_MENU_ITEM(toolview));
	gtk_menu_item_activate(GTK_MENU_ITEM(statusview));
}

void Instance::ToolBar()
{
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar),GTK_TOOLBAR_BOTH);
	gtk_container_set_border_width(GTK_CONTAINER(toolbar),5);  	
	
	gtk_tool_button_set_label(GTK_TOOL_BUTTON(t_new),"New tab");

	gtk_tool_item_set_tooltip_text(t_new,"Create an empty tab");
	gtk_tool_item_set_tooltip_text(t_open,"Open a file");
	gtk_tool_item_set_tooltip_text(t_save,"Save current file");
	gtk_tool_item_set_tooltip_text(t_undo,"Undo previous action");	
	gtk_tool_item_set_tooltip_text(t_redo,"Redo undone action");
	gtk_tool_item_set_tooltip_text(t_cut,"Cut selection");
	gtk_tool_item_set_tooltip_text(t_copy,"Copy selection");
	gtk_tool_item_set_tooltip_text(t_paste,"Paste clipboard");		
	
	g_signal_connect(t_new,"clicked",G_CALLBACK(file_new),notebook);	
	g_signal_connect(t_open,"clicked",G_CALLBACK(file_open),NULL);
	g_signal_connect(t_save,"clicked",G_CALLBACK(file_save_widget),NULL);	
	g_signal_connect(t_undo,"clicked",G_CALLBACK(file_undo),NULL);
	g_signal_connect(t_redo,"clicked",G_CALLBACK(file_redo),NULL);
	g_signal_connect(t_cut,"clicked",G_CALLBACK(cut_slice),buffer);
	g_signal_connect(t_copy,"clicked",G_CALLBACK(copy_slice),buffer);
	g_signal_connect(t_paste,"clicked",G_CALLBACK(paste_slice),buffer);
	
	
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),t_new,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),t_open,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),t_save, -1);	
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),t_sep1, -1); 	
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),t_undo,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),t_redo,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),t_sep2,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),t_cut,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),t_copy,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),t_paste,-1);	
}

void Instance::Color()
{
	GdkRGBA fore_color;

	gdk_rgba_parse(&fore_color,"rgb(255,255,255)");	
	gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(fg_color),&fore_color);

	g_signal_connect(bg_color,"color-set",G_CALLBACK(set_background_color),NULL);	
	g_signal_connect(fg_color,"color-set",G_CALLBACK(set_foreground_color),NULL);
}

void Instance::Text()
{
	PangoFontDescription *font;
	GdkRGBA rgba;
	
	font=pango_font_description_from_string("Monospace 15");
 	gtk_widget_override_font(text,font);
  	pango_font_description_free(font);

	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer),"red_bg","background","red",NULL); 
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer),"data_type","foreground","green",NULL);
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer),"control","foreground","red",NULL);
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer),"string","foreground","purple",NULL);	
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer),"#pre","background","blue",NULL);	
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer),"misc","foreground","yellow",NULL);	
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer),"sin_comment","foreground","blue",NULL);	
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer),"highlight_bold","weight",PANGO_WEIGHT_BOLD,NULL);
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer),"highlight_italic","style",PANGO_STYLE_ITALIC,NULL);
	
	gdk_rgba_parse(&rgba,"rgb(0,0,0)");
	gtk_widget_override_background_color(text,GTK_STATE_FLAG_NORMAL,&rgba);

}

void Instance::ShowAll() 
{
	gtk_widget_add_events(GTK_WIDGET(ptr_view),GDK_BUTTON_PRESS_MASK);	

	gtk_widget_show_all(window);
	
	update_statusbar(GTK_TEXT_BUFFER(ptr_buff),GTK_STATUSBAR(status));	
		
	gtk_main();		
}

