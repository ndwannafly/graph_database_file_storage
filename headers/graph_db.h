#ifndef LAB1_GRAPH_DB
#define LAB1_GRAPH_DB
#include <stdio.h>

struct attr;
struct scheme_node;

typedef struct attr {
    char * name_attr;
    unsigned char type_attr;
    struct attr * next;
} attr;

typedef struct scheme_node{
    struct attr *first_attr;
    struct attr *last_attr;
    struct scheme_node *next_node;
    char * type;
} scheme_node;

typedef struct {
    scheme_node * first_node;
    scheme_node * last_node;
} db_scheme;

typedef struct {
    db_scheme *scheme;
    FILE * file_storage;
} graph_db;


db_scheme * create_new_scheme();

graph_db * create_new_graph_db_by_scheme(db_scheme * scheme, char * file_name);

scheme_node * add_node_to_scheme(db_scheme * scheme, char * type_name);

scheme_node * search_node_by_type_name(db_scheme * scheme, char * type_name, int * n);
#endif
