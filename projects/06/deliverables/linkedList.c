#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "linkedList.h"

/*

typedef struct symbol_type{

    char symbolName[CHARLEN];
    int address;
}symbol_t;

struct linked_list_node {
  struct linked_list_node *next, *prev;
  symbol_t symbol;
};

struct linked_list {
  symbol_t *first;
  int size;
};

*/


/*
int main(int argc, char **argv)
{
	struct linked_list *l;
	// struct linked_list_node *n; 

	l= linked_list_new();
	// n = linked_list_new_string_node("Thomas"); 

	linked_list_insert(l, linked_list_new_string_node("Diana"));
	linked_list_insert(l, linked_list_new_string_node("Belea"));
	linked_list_insert(l, linked_list_new_string_node("Thomas"));
	linked_list_insert(l, linked_list_new_string_node("Bracht"));
	linked_list_insert(l, linked_list_new_string_node("Laumann"));
	linked_list_insert(l, linked_list_new_string_node("Jespersen"));
	linked_list_insert(l, linked_list_new_string_node("BITCHES!"));

	linked_list_print(l);

	return 0;
}

*/
static int free_address = 16;

void
linked_list_print(struct linked_list* l)
{
	struct linked_list_node* n = l->first;
	printf("Size: %d\n[", l->size);

	while (1) {
		printf("'%s'", (char*) n->symbol->symbolName);
		if (n->next == NULL) break;
		else printf(" <-> ");
		n = n->next;
	}

	printf("]\n");
}

struct linked_list* linked_list_new()
{
	struct linked_list* list;

	list = (struct linked_list*)malloc(sizeof(struct linked_list));

	list->first = NULL;
	list->size  = 0;

	return list;
}

struct linked_list_node* linked_list_new_node()
{
	struct linked_list_node* node;

	/* Allocate! */
	node = (struct linked_list_node*)malloc(sizeof(struct linked_list_node));

	node->next = NULL;
	node->prev = NULL;
	node->symbol  = NULL;

	return node;
}

void linked_list_change_node_address(struct linked_list* l, const char *symbolName, int address){

	struct linked_list_node* n = l->first;
	while (n) {

		if(strcmp(n->symbol->symbolName, symbolName) == 0){
			n->symbol->address = address;
			// printf("linked_list_change_node_address(): address for symbol %s changed to = %d\n",symbolName, n->symbol->address);
			//decrement free address count
			free_address--;
			return;
		}
		n = n->next;
	}
	// printf("linked_list_change_node_address(): failed to find symbol %s\n",symbolName);
}

struct linked_list_node*
linked_list_new_string_node(char *symbolName, int address)
{
	struct linked_list_node* node;

	node = linked_list_new_node();

	node->symbol = (symbol_t*)malloc(sizeof(symbol_t));
	strncpy(node->symbol->symbolName,symbolName,strlen(symbolName));

	if(address == -1){
		node->symbol->address = free_address;
		free_address++;
	}
	else
		node->symbol->address = address;

	// DPRINTF("linked_list_new_string_node(): new symbol %s has address %lu, with length = %d\n",symbolName, node->symbol->address, strlen(symbolName));

	return node;
}

// int
// linked_list_get_node_addr(const char* symbolName, struct linked_list* l){

// 	struct linked_list_node* n = l->first;

// 	DPRINTF("linked_list_get_node_addr(): symbolName = %s\n",symbolName);

// 	while (n->next) {

// 		DPRINTF("linked_list_get_node_addr(): n->symbol->symbolName = %s\n",n->symbol->symbolName);
		
// 		if(strcmp(n->symbol->symbolName, symbolName) == 0){
// 			DPRINTF("linked_list_get_node_addr(): address for symbol %s = %d\n",symbolName, n->symbol->address);
// 			return n->symbol->address;
// 		}
// 		n = n->next;
// 	}
// 	//check the last element in the linked list
// 	if(strcmp(n->symbol->symbolName, symbolName) == 0){
// 		DPRINTF("linked_list_get_node_addr(): address for symbol %s = %d\n",symbolName, n->symbol->address);
// 		return n->symbol->address;
// 	}

// 	printf("linked_list_get_node_addr(): no symbol %s found\n",symbolName);
// 	return -1;
// }

int
linked_list_get_node_addr(const char* symbolName, struct linked_list* l){

	struct linked_list_node* n = l->first;

	DPRINTF("linked_list_get_node_addr(): symbolName = %s\n",symbolName);

	while (n) {

		DPRINTF("linked_list_get_node_addr(): n->symbol->symbolName = %s\n",n->symbol->symbolName);
		
		if(strcmp(n->symbol->symbolName, symbolName) == 0){
			DPRINTF("linked_list_get_node_addr(): address for symbol %s = %d\n",symbolName, n->symbol->address);
			return n->symbol->address;
		}
		n = n->next;
	}

	// printf("linked_list_get_node_addr(): no symbol %s found\n",symbolName);
	return -1;
}

int
linked_list_check_node_avail(const char* symbolName, struct linked_list* l){

	struct linked_list_node* n = l->first;

	while (n) {
		
		if(strcmp(n->symbol->symbolName, symbolName) == 0){
			// printf("linked_list_check_node_avail(): symbol %s found with address = %lu\n",symbolName,n->symbol->address);
			return 1;
		}
		n = n->next;
	}
	return 0;
}

int linked_list_insert(struct linked_list *list, struct linked_list_node *item){

	if (list->first == NULL) {
		list->first = item;
		goto end;
	}

	struct linked_list_node *n, *last;
	n = list->first;

	for (last = n; n ; last = n, n = n->next);

	n = item;

	/* Make them point to each other */
	item->prev = last;
	last->next = item;

end:
	list->size++;
	return 0;
}


void del_linked_list(struct linked_list* list){

	struct linked_list_node* n = list->first;

	while (1) {
		
		if (n->next == NULL){
			free(n->symbol);
			n->symbol = NULL;
			free(n->prev);
			n->prev = NULL;
			free(n);
			n = NULL;
			break;
		}
		else{
			free(n->symbol);
			n->symbol = NULL;
			n->prev = NULL;
			n = n->next;
			free(n->prev);
			n->prev = NULL;
		}
	}
	free(list);
	list = NULL;
}




