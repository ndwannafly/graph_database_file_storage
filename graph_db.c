#include "headers/graph_db.h"
#include <stdlib.h>

int used_mem = 0;

db_scheme * create_new_scheme(){
    db_scheme * new_db_scheme = (db_scheme *) malloc(sizeof(db_scheme));
    used_mem += sizeof(db_scheme);
    new_db_scheme->first_scheme_node = NULL;
    new_db_scheme->last_scheme_node =  NULL;
    return new_db_scheme;
}

void free_up_db(graph_db * db){
    used_mem -= sizeof(*db);
    free(db);
}

graph_db * create_new_graph_db_by_scheme(db_scheme *scheme, char *file_name){
    graph_db * new_graph_db = (graph_db *) malloc(sizeof(graph_db));
    used_mem += sizeof(graph_db);
    new_graph_db -> file_storage = fopen(file_name, "w+b");
    if (new_graph_db->file_storage) {
        int scheme_length;
        new_graph_db->scheme = scheme;

    } else {
        free_up_db(new_graph_db);
        return NULL;
    }
}
