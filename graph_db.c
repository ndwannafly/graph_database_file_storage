#include "headers/graph_db.h"
#include <stdlib.h>
#include <string.h>

int used_mem = 0;

db_scheme * create_new_scheme(){
    db_scheme * new_db_scheme = (db_scheme *) malloc(sizeof(db_scheme));
    used_mem += sizeof(db_scheme);
    new_db_scheme->first_node = NULL;
    new_db_scheme->last_node =  NULL;
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

scheme_node * search_node_by_type_name(db_scheme * scheme, char * type_name, int * n) {
    scheme_node * current_node = scheme -> first_node;
    *n = 0;
    while (current_node != NULL) {
        if (strcmp(type_name, current_node->type) == 0){
            return current_node;
        } else {
            current_node = current_node -> next_node;
            (*n)++;
        }
    }
    *n = -1;
    return NULL;
}

scheme_node * add_node_to_scheme(db_scheme * scheme, char * type_name){
    scheme_node * new_node = (scheme_node *) malloc(sizeof(scheme_node));
    if (scheme -> first_node == NULL || scheme -> last_node == NULL ) {
        used_mem += sizeof(scheme_node);
        scheme->first_node = new_node;
        scheme->last_node = new_node;
    } else {
        // check if there already exists the same node
        int n;
        if (search_node_by_type_name(scheme, type_name, &n)) // if exists
            return NULL;
        used_mem += sizeof(scheme_node);
        scheme->last_node->next_node = new_node;
        scheme->last_node = new_node;
    }
    new_node->type = (char *) malloc(1+strlen(type_name)*sizeof(char));
    used_mem += 1 + strlen(type_name) * sizeof(char);
    strcpy(new_node->type, type_name);
    new_node->last_attr = NULL;
    new_node->first_attr = NULL;
    new_node->next_node = NULL;

    return new_node;
}
