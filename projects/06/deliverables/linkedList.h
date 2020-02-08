#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H


#define CHARLEN                ( 64 )
// #define DEBUG                   1

#ifdef DEBUG
#define DPRINTF             printf
#else
#define DPRINTF(...)       (void)0
#endif


typedef struct symbol_type{

    char symbolName[CHARLEN];
    int address;
}symbol_t;

struct linked_list_node {
  struct linked_list_node *next, *prev;
  symbol_t *symbol;
};

struct linked_list {
  struct linked_list_node *first;
  int size;
};

struct linked_list      	*linked_list_new();
struct linked_list_node 	*linked_list_new_node();
struct linked_list_node 	*linked_list_new_string_node(char *symbolName, int address);
int               			linked_list_insert(struct linked_list*, struct linked_list_node*);
void 						del_linked_list(struct linked_list* list);
void              			linked_list_print(struct linked_list*);
int 						linked_list_get_node_addr(const char* symbolName, struct linked_list* l);
int 						linked_list_check_node_avail(const char* symbolName, struct linked_list* l);
void 						linked_list_change_node_address(struct linked_list* l, const char *symbolName, int address);


#endif