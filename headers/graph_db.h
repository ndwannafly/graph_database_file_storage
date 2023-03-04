#ifndef LAB1_GRAPH_DB
#define LAB1_GRAPH_DB
#include <stdio.h>

struct attr;
struct schema_node;

typedef struct {
    char * name_attr;
    unsigned char type_attr;
    struct attr * next;
} attr;

typedef struct {
    struct attr *first_attr;
    struct attr *last_attr;
    struct schema_node *next_schema_node;
} schema_node;

typedef struct {
    schema_node * first_scheme_node;
    schema_node * last_scheme_node;
} db_scheme;

typedef struct {
    db_scheme *scheme;
    FILE * file_storage;
} graph_db;


db_scheme * create_new_scheme();

graph_db * create_new_graph_db_by_scheme(db_scheme * scheme, char *FileName);


#endif
