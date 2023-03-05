#ifndef LAB1_GRAPH_DB
#define LAB1_GRAPH_DB
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

enum attr_type{AT_INT32, AT_FLOAT, AT_STRING, AT_BOOLEAN};

struct attr;
struct scheme_node;

typedef struct node_relation{
    struct scheme_node * node;
    struct node_relation * next_node_relation;
} node_relation;

typedef struct scheme_node{
    struct attr * first_attr;
    struct attr * last_attr;
    struct scheme_node * next_node;
    node_relation * first_node_relation;
    node_relation * last_node_relation;
    char * type;
    int root_off_set;
    int first_off_set;
    int last_off_set;
} scheme_node;

typedef struct attr {
    char * name_attr;
    unsigned char type_attr;
    struct attr * next;
} attr;

typedef struct {
    scheme_node * first_node;
    scheme_node * last_node;
} db_scheme;

typedef struct {
    db_scheme * scheme;
    FILE * file_storage;
    char * write_buffer;
    int n_write_buffer;
    char * read_buffer;
    int n_read_buffer;
    int i_read_buffer;
} graph_db;


db_scheme * create_new_scheme();

graph_db * create_new_graph_db_by_scheme(db_scheme * scheme, char * file_name);

scheme_node * add_node_to_scheme(db_scheme * scheme, char * type_name);

scheme_node * search_node_by_type_name(db_scheme * scheme, char * type_name, int * n);

attr * add_attr_to_node(scheme_node * node, char * name, char type);

attr * search_attr_by_name(scheme_node * node, char * name, int * n);

node_relation * add_node_relation(scheme_node * node, scheme_node * next_related_node);

node_relation * search_node_relation_by_nodes (scheme_node * node, scheme_node * next_related_node);

void del_node_relation(scheme_node * node, scheme_node * to_delete_node);
void del_node_from_scheme(db_scheme * scheme, scheme_node * node);
void del_attr_from_node(scheme_node * node, attr * to_delete_attr);

void free_node_from_scheme(scheme_node * node);

void store_attrs(graph_db * db, attr * attributes);

void store_relations(graph_db * db, node_relation * relations);

#endif
