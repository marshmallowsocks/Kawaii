#ifndef _linkedlist
#define _linkedlist
#include<iostream>
#include<string.h>
#include<gtk/gtk.h>

extern gpointer ptr_buff; //Link to pointer
extern unsigned long undo_counter; //Keep track of number of undos

using namespace std;

class LinkedList
{
	GtkTextBuffer *undo_buffer_state;
	
	LinkedList *llink,*rlink;

	public:
		int x;		

		LinkedList()
		{
			GtkTextIter start,end,ustart;
			gchar *data;			
			
			undo_buffer_state=gtk_text_buffer_new(NULL);

			if(ptr_buff==NULL); //First call,to be ignored
			
			else
			{
				gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(ptr_buff),&start,&end);
				data=gtk_text_buffer_get_text(GTK_TEXT_BUFFER(ptr_buff),&start,&end,TRUE);

				gtk_text_buffer_get_start_iter(undo_buffer_state,&ustart);
				gtk_text_buffer_insert(undo_buffer_state,&ustart,data,-1);	
			} //Now the current buffer snapshot is present in the list
			
			llink=NULL;
			rlink=NULL;
		}

		friend LinkedList* insert_rear(LinkedList*);
		friend void delete_rear(LinkedList*);		
		friend void display(LinkedList*);
		friend void undo(LinkedList*);
};

LinkedList* insert_rear(LinkedList* head)
{
	LinkedList *cur,*temp;

	cur=head;

	while(cur->rlink!=NULL)
		cur=cur->rlink;

	temp=new LinkedList();
		
	temp->x=head->x;	
	
	cur->rlink=temp;
	temp->llink=cur;

	return head;
}

void delete_rear(LinkedList* head)
{
	LinkedList *cur;
		
	cur=head;

	while(cur->rlink!=NULL)
		cur=cur->rlink;
		
	cur->llink->rlink=NULL;
	delete(cur);
}

void display(LinkedList *head)
{
	LinkedList *cur;

	cur=head;
	
	while(cur!=NULL)
	{
		g_print("%d->",cur->x);
		cur=cur->rlink;
	}
	
	g_print("\n");
}

void undo(LinkedList *head) //This function must replace the current buffer with the topmost snapshot,and then delete the snapshot
{
	if(head==NULL) return;

	else
	{	
		LinkedList *cur;
		GtkTextIter start,end,buff_start;	
		gchar *undo_data;		
		cur=head;
	
		while(cur->rlink!=NULL)
			cur=cur->rlink;
		
		gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(ptr_buff),&start,&end);
		gtk_text_buffer_delete(GTK_TEXT_BUFFER(ptr_buff),&start,&end);

		gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(cur->undo_buffer_state),&start,&end);
		undo_data=gtk_text_buffer_get_text(GTK_TEXT_BUFFER(cur->undo_buffer_state),&start,&end,FALSE);		
		
		gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(ptr_buff),&start);		
		gtk_text_buffer_insert(GTK_TEXT_BUFFER(ptr_buff),&start,undo_data,-1);
			
		if(head->rlink!=NULL)
			delete_rear(head);
	}

}

#endif

	
	
