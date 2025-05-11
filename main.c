#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef struct page{
	int ID;
	char url[50];
	char* description;
} page;

typedef struct node{
	struct node* next;
	page* holded_page;
} node;

typedef struct stack{
	node* top;
	int len;
} stack;

typedef struct tab{
	int ID;
	page* currentPage;
	stack* backwardStack;
	stack* forwardStack;
} tab;

typedef struct tab_node{
	struct tab_node* next;
	struct tab_node* prev;
	tab* holded_tab;
} tab_node;

typedef struct tabsList{
	tab_node* sentinel;
	int len;
} tabsList;

typedef struct browser{
	tab* current;
	tabsList list;
} browser;

void DISPLAY_ERROR_MESSAGE(FILE* out_file)
{
	fprintf(out_file, "403 Forbidden\n");
}

int IS_EMPTY_TABLIST(tabsList* tab_list)
{
	return tab_list->len == 0;
}

void INIT_STACKS(tab* current_tab)
{
	current_tab->backwardStack = (stack*)malloc(sizeof(stack));
	current_tab->forwardStack = (stack*)malloc(sizeof(stack));
	current_tab->backwardStack->top = NULL;
	current_tab->forwardStack->top = NULL;
	current_tab->backwardStack->len = 0;
	current_tab->forwardStack->len = 0;
}

page* CREATE_IMPLICIT_PAGE()
{
	page* implicit_page = (page*)malloc(sizeof(page));
	strcpy(implicit_page->url, "https://acs.pub.ro/");
	implicit_page->ID = 0;
	char* description = "Computer Science";
	implicit_page->description = (char*)malloc((strlen(description)+1)*
												sizeof(char));
	strcpy(implicit_page->description, description);
	return implicit_page;
}

void INIT_BROWSER(browser* Chrome_x)
{
	// Init implicit tab
	tab* current = (tab*)malloc(sizeof(tab));
	current->currentPage = CREATE_IMPLICIT_PAGE();
	current->ID = 0;
	Chrome_x->current = current;
	// Init implicit tab_node
	Chrome_x->list.sentinel = (tab_node*)malloc(sizeof(tab_node));
	Chrome_x->list.sentinel->holded_tab = NULL;
	tab_node* zero_tab = (tab_node*)malloc(sizeof(tab_node));
	zero_tab->next = Chrome_x->list.sentinel;
	zero_tab->prev = Chrome_x->list.sentinel;
	zero_tab->holded_tab = current;
	// Link circular nodes with implicit
	Chrome_x->list.sentinel->next = zero_tab;
	Chrome_x->list.sentinel->prev = zero_tab;
	INIT_STACKS(current);
	Chrome_x->list.len = 1;
}



void NEW_TAB(browser* Chrome_x, int* max_tabs)
{
	// Alloc new tab and create implicit page
	tab* recent_tab = (tab*)malloc(sizeof(tab));
	recent_tab->currentPage = CREATE_IMPLICIT_PAGE();
	recent_tab->ID = (*max_tabs)+1;
	*max_tabs = *max_tabs + 1;
	INIT_STACKS(recent_tab);
	// Link circurlar list and new tab_node
	tab_node* new_node_tab = (tab_node*)malloc(sizeof(tab_node));
	new_node_tab->holded_tab = recent_tab;
	new_node_tab->next = Chrome_x->list.sentinel;
	new_node_tab->prev = Chrome_x->list.sentinel->prev;
	Chrome_x->list.sentinel->prev->next = new_node_tab;
	Chrome_x->list.sentinel->prev = new_node_tab;
	Chrome_x->current = recent_tab;
	Chrome_x->list.len++;
}

void PRINT(browser* Chrome_x, FILE* out_file){
	// Find current tab node
	tab_node* current_tab_node = Chrome_x->list.sentinel;
	while (current_tab_node->holded_tab != Chrome_x->current)
	{
		current_tab_node = current_tab_node->next;
	}	
	// Display tab ID's starting from current tab to right
	tab_node* tmp = current_tab_node;
	if(Chrome_x->list.len == 1)
	{
		fprintf(out_file, "%d\n", current_tab_node->holded_tab->ID);
	} else {
		while (current_tab_node->next != tmp){
			if (current_tab_node != Chrome_x->list.sentinel){
				fprintf(out_file, "%d ", current_tab_node->holded_tab->ID);
			}
			current_tab_node = current_tab_node->next;
		}
		if (current_tab_node != Chrome_x->list.sentinel){
			fprintf(out_file, "%d\n", current_tab_node->holded_tab->ID);
		} else 
		{
			fprintf(out_file, "\n");
		}
	}
	// Show current tab description
	fprintf(out_file, "%s", Chrome_x->current->currentPage->description);
	fprintf(out_file, "\n");
}
void FREE_PAGE(page** page_erase){
	// Additional function to free pages
	free((*page_erase)->description);
	free(*page_erase);
	*page_erase = NULL;
}

void PUSH_STACK(stack* stack_push, page* page_push)
{
	// Additional function to push pages into forw/back stack.
	node* new_node = (node*)malloc(sizeof(node));
	new_node->holded_page = page_push;
	new_node->next = stack_push->top;
	stack_push->len++;
	stack_push->top = new_node;
}

page* POP_STACK(stack* stack_pop)
{
	// Get page from the top of the stack and erase it from stack, 
	// then returns it.
	if (stack_pop->top == NULL)
	{
		return NULL;
	}
	node* tmp_node = stack_pop->top;
	stack_pop->top = stack_pop->top->next;
	page* res_page = tmp_node->holded_page;
	free(tmp_node);
	stack_pop->len--;
	return res_page;
}

void DISPLAY_STACK(stack* stack_display, FILE* out_file)
{
	// Function to display entire stack.
	node* current_node = stack_display->top;
	while (current_node != NULL) {
		fprintf(out_file, "%s", current_node->holded_page->url);
		fprintf(out_file, "\n");
		current_node = current_node->next; 
	}
}

int IS_EMPTY_STACK(stack* check_stack)
{
	// Func to check if stack is empty
	return check_stack->top == NULL;
}

void FREE_STACK(stack** stack_erase, int absolute_free)
{
	node* cur = (*stack_erase)->top;
	while (cur != NULL)
	{
		node* tmp = cur->next; 
		// FREE only if page is implicit (id 0), other pages are static
		if (cur->holded_page->ID == 0) {
			FREE_PAGE(&(cur->holded_page));
		}
		free(cur);
		cur = tmp;
	}
	(*stack_erase)->top = NULL;
	(*stack_erase)->len = 0;

	// Free entire stack structure if flag is setted to 1
	if (absolute_free)
	{
		free(*stack_erase);
		*stack_erase = NULL;
	}
}
void FREE_TAB(tab_node* tab_node_erase)
{
	tab* tab_to_free = tab_node_erase->holded_tab;
	// Once we want we want to free a tab, we 
	// should fully free both stacks too

	FREE_STACK(&(tab_to_free->backwardStack), 1);
	FREE_STACK(&(tab_to_free->forwardStack), 1);

	
	if (tab_to_free->currentPage != NULL && 
		tab_to_free->currentPage->ID == 0)
	{
		FREE_PAGE(&(tab_to_free->currentPage));
		
	}
	free(tab_to_free);
	tab_node_erase->holded_tab = NULL;
}

void CLOSE(browser* Chrome_x, FILE* out_file)
{
	// Cannot close tab with ID 0 -> display error message
	if (Chrome_x->current->ID == 0)
	{
		DISPLAY_ERROR_MESSAGE(out_file);
		return;
	}
	// Close current tab, re-links nodes in circular list
	// then set up new current tab (the left one)
	tab_node* current_tab_node = Chrome_x->list.sentinel->next;
	while (current_tab_node->next->holded_tab != Chrome_x->current)
	{
		current_tab_node = current_tab_node->next;
	}
	tab_node* closed_tab = current_tab_node->next;
	Chrome_x->current = current_tab_node->holded_tab;
	current_tab_node->next = current_tab_node->next->next;
	if (closed_tab->next == Chrome_x->list.sentinel)
	{
		Chrome_x->list.sentinel->prev = current_tab_node;
	} else 
	{
		closed_tab->next->prev = current_tab_node;
	}
	Chrome_x->list.len--;
	FREE_TAB(closed_tab);
	free(closed_tab);
}
void OPEN(browser* Chrome_x, char* operation, FILE* out_file)
{
	// Func to open a specific tab from circular list
	int ID;
	char stock[256];
	sscanf(operation, "%s %d", stock, &ID);
	if(ID == Chrome_x->current->ID)
	{
		return;
	}
	// Check if specific tab ID exist
	tab_node* current_tab_node = Chrome_x->list.sentinel->next;
	while (current_tab_node->next != Chrome_x->list.sentinel)
	{
		if (current_tab_node->holded_tab->ID == ID)
		{
			Chrome_x->current = current_tab_node->holded_tab;
			return;
		}
		current_tab_node = current_tab_node->next;
	}
	// Specific not found -> display error message
	DISPLAY_ERROR_MESSAGE(out_file);
}
void NEXT(browser* Chrome_x)
{
	// Looking for our current tab node
	tab_node* current_tab_node = Chrome_x->list.sentinel->next;
	while (current_tab_node->next != Chrome_x->list.sentinel)
	{
		if (current_tab_node->holded_tab == Chrome_x->current)
		{
			break;
		}
		current_tab_node = current_tab_node->next;
	}
	// Check if next tab is santinel node, if true we jump + 1 node;
	if (current_tab_node->next == Chrome_x->list.sentinel)
	{
		Chrome_x->current = Chrome_x->list.sentinel->next->holded_tab;
	} else
	{
		Chrome_x->current = current_tab_node->next->holded_tab;
	}
}

void PREV(browser* Chrome_x)
{
	// Looking for our current tab node
	tab_node* current_tab_node = Chrome_x->list.sentinel->next;
	while (current_tab_node->next != Chrome_x->list.sentinel)
	{
		if (current_tab_node->holded_tab == Chrome_x->current)
		{
			break;
		}
		current_tab_node = current_tab_node->next;
	}
	// Check if prev tab is santinel node, if true we jump + 1 node;
	if (current_tab_node->prev == Chrome_x->list.sentinel)
	{
		Chrome_x->current = Chrome_x->list.sentinel->prev->holded_tab;
	} else
	{
		Chrome_x->current = current_tab_node->prev->holded_tab;
	}
}

void PARSE_INPUT(page* page_vector, char*** operation_vector,
				 int* page_count, int* operation_count) {
	FILE* p_infile = fopen("tema1.in", "r");
	// Function to parse all pages and operations from file
	char stock[256];
	int temp;
	
	fscanf(p_infile, "%d", page_count);
	while ((temp = fgetc(p_infile)) != '\n' && temp != EOF){}
	// Get all pages
	for (int i = 0; i < *page_count; i++) {
		fscanf(p_infile, "%d", &page_vector[i].ID);
		while ((temp = fgetc(p_infile)) != '\n' && temp != EOF){}

		fscanf(p_infile, "%s", page_vector[i].url);
		while ((temp = fgetc(p_infile)) != '\n' && temp != EOF){}

		fgets(stock, sizeof(stock), p_infile);
		page_vector[i].description = strdup(stock);
	}
	fscanf(p_infile, "%d", operation_count);
	while ((temp = fgetc(p_infile)) != '\n' && temp != EOF);
	// Get all operations
	*operation_vector = malloc(*operation_count * sizeof(char*));
	for (int i = 0; i < *operation_count; i++) {
		fgets(stock, sizeof(stock), p_infile);
		(*operation_vector)[i] = strdup(stock);
	}
	fclose(p_infile);
}
void PAGE(browser* Chrome_x, char* operation,
		  page* page_vector, int* page_count, FILE* out_file)
{
	int page_id;
	char stock[256];
	sscanf(operation, "%s %d", stock, &page_id);
	// Check if specific page_id exists
	int exist_flag = 0, i;
	for (i = 0; i < *page_count; i++)
	{
		if(page_vector[i].ID == page_id)
		{
			exist_flag = 1;
			break;
		}
	}
	if (exist_flag)
	{

		// If exists, push current page into backward
		// and set up new current page
		PUSH_STACK(Chrome_x->current->backwardStack,
				   Chrome_x->current->currentPage);
		Chrome_x->current->currentPage = &page_vector[i];
		FREE_STACK(&(Chrome_x->current->forwardStack), 0);
	} else
	{
		DISPLAY_ERROR_MESSAGE(out_file);
	}

}
void BACKWARD(browser* Chrome_x, FILE* out_file)
{
	// Check if backward stack is empty
	if (IS_EMPTY_STACK(Chrome_x->current->backwardStack))
	{
		DISPLAY_ERROR_MESSAGE(out_file);
	} else 
	{
		// If not empty we push current page into forward
		// then set up current as top bage from backward
		PUSH_STACK(Chrome_x->current->forwardStack,
				   Chrome_x->current->currentPage);
		page* back_page = POP_STACK(Chrome_x->current->backwardStack);
		Chrome_x->current->currentPage = back_page;
	}
}

void FORWARD(browser* Chrome_x, FILE* out_file)
{
	// Check if forward stack is empty
	if (IS_EMPTY_STACK(Chrome_x->current->forwardStack))
	{
		DISPLAY_ERROR_MESSAGE(out_file);
	} else 
	{
		// If not empty we push current page into backward
		// then set up current as top bage from forward
		PUSH_STACK(Chrome_x->current->backwardStack,
				   Chrome_x->current->currentPage);
		page* front_page = POP_STACK(Chrome_x->current->forwardStack);
		Chrome_x->current->currentPage = front_page;
	}
}

void PRINT_HISTORY(browser* Chrome_x, char* operation, FILE* out_file)
{
	//Func to display history for one specific tab.
	int tab_id;
	char stock[256];
	sscanf(operation, "%s %d", stock, &tab_id);
	// Check if tab with specific id exists
	int exist_flag=0;
	tab_node* current_tab_node = Chrome_x->list.sentinel->next;
	while (current_tab_node != Chrome_x->list.sentinel){
		if (current_tab_node->holded_tab->ID == tab_id){
			exist_flag=1;
			break;
		}
		current_tab_node = current_tab_node->next;
	}
	if (exist_flag)
	{
		// If specific tab exists, we print history in next order
		// Forward -> current -> backward
		DISPLAY_STACK(current_tab_node->holded_tab->forwardStack, out_file);
		fprintf(out_file, "%s\n", current_tab_node->holded_tab->currentPage->url);
		DISPLAY_STACK(current_tab_node->holded_tab->backwardStack, out_file);

	} else
	{
		DISPLAY_ERROR_MESSAGE(out_file);
	}
	

}


void PERFORM_OPERATIONS(char** operation_vector, browser* Chrome_x,
						int* operation_count, int* page_count,
						page* page_vector, FILE* out_file, int* max_tabs)
{
	// Possible operations declaration
	const char* operation_type[] = {
	 "NEW_TAB", "CLOSE", "OPEN", "NEXT", "PREV", "PAGE", 
		"BACKWARD", "FORWARD", "PRINT_HISTORY", "PRINT"
	};
	enum operation_enum {
		oNEW_TAB, oCLOSE, oOPEN, oNEXT, oPREV, oPAGE, oBACKWARD, oFORWARD,
		pPRINT_HISTORY, oPRINT
	};
	char operation[256];
	int ID;
	int len = sizeof(operation_type) / sizeof(operation_type[0]);

	// Read operation by operation from vector and execute them
	for (int i = 0; i < *operation_count; i++)
	{
		int type;
		sscanf(operation_vector[i], "%s %d", operation, &ID);
		for(int j = 0; j < len; j++)
		{
			if (strstr(operation_vector[i], operation_type[j]) != NULL)
			{
				type = j;
				break;
			}
		}
		switch (type)
		{
			case oNEW_TAB: NEW_TAB(Chrome_x, max_tabs); break;
			case oCLOSE: CLOSE(Chrome_x, out_file); break;
			case oOPEN: OPEN(Chrome_x, operation_vector[i], out_file); break;
			case oNEXT: NEXT(Chrome_x); break;
			case oPREV: PREV(Chrome_x); break;
			case oPAGE: {
				PAGE(Chrome_x, operation_vector[i], page_vector, page_count, out_file);
				break;
			}
			case oFORWARD: FORWARD(Chrome_x, out_file); break;
			case oBACKWARD: BACKWARD(Chrome_x, out_file); break;
			case oPRINT: PRINT(Chrome_x, out_file); break;
			case pPRINT_HISTORY: {
				PRINT_HISTORY(Chrome_x, operation_vector[i], out_file);
				break;
			}
		}
	}
}
void FREE_BROWSER(browser* Chrome_x) {
	// Free memory for all browser tabs. 
	tab_node* current_node = Chrome_x->list.sentinel->next;
	while (current_node != Chrome_x->list.sentinel) {
		tab_node* next_node = current_node->next;
		FREE_TAB(current_node);
		free(current_node);
		current_node = next_node;
	}
	// Free the circular list
	free(Chrome_x->list.sentinel);
	Chrome_x->list.sentinel = NULL;
	Chrome_x->list.len = 0;
	Chrome_x->current = NULL;
}

int main()
{
	browser Chrome_114;
	tabsList Incognito = {NULL, 0};
	Chrome_114.list = Incognito;
	page page_vector[50];
	char** operation_vector;
	int page_count=0, operation_count=0;
	FILE* out_file;
	int max_tabs = 0;
	out_file = fopen("tema1.out", "w");

	INIT_BROWSER(&Chrome_114);
	PARSE_INPUT(page_vector, &operation_vector, &page_count, 
		&operation_count);
	PERFORM_OPERATIONS(operation_vector, &Chrome_114, &operation_count,
					   &page_count, page_vector, out_file, &max_tabs);
	fclose(out_file);
	// Free memory for all stacks, nodes ...
	FREE_BROWSER(&Chrome_114);
	// Free memory for operation vector
	if (operation_vector != NULL) {
		for (int i = 0; i < operation_count; i++) {
			if (operation_vector[i] != NULL) {
			   free(operation_vector[i]);
			}
		}
		free(operation_vector);
	}
	
	// Free memory for all pages description
	for (int i = 0; i < page_count; i++) {
		if (page_vector[i].description != NULL) {
			free(page_vector[i].description);
			page_vector[i].description = NULL;
		}
	}
	return 0;
}