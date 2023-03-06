/*
    TO-DO
    1. add node to db -> DONE
    2. add attr of node to db -> DONE
    3. write node data to file -> DOING
    4. read node data from file
    5. load db from file
    6. query select with simple compare
    7. query select with AND condition
    8. query delete
    9. test delete correctly
    10. test free memory correctly
*/
#include "headers/graph_db.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int used_mem = 0;

int BUFFER_SIZE = 1024*32;
int MAX_NODE_RELATIONS = 8;

int get_used_mem(){
    return used_mem;
}

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


void db_fflush(graph_db * db){
    fflush(db->file_storage);
}

void db_fseek(graph_db * db, int offset, int flag){
    db_fflush(db);
    fseek(db->file_storage, offset, flag);
}
int db_ftell(graph_db *db){
    db_fflush(db);
    return ftell(db->file_storage);
}

void write_buffer (char * buffer, int * n_buffer, float what) {
    char * w = (char *) &what;
    int i;
    buffer += *n_buffer;
    for (i = 0; i < sizeof(float); i++, buffer++, (*n_buffer)++){
        *buffer = w[i];
    }
}

void db_fwrite(void * buf, int item_size, int n_items, graph_db * db){
    char * _buf = (char *) buf;
    int n_free = BUFFER_SIZE - db->n_write_buffer;
    int n_bytes = item_size * n_items;
    int i = db->n_write_buffer;
    if (db->i_read_buffer < db->n_read_buffer){
        fseek(db->file_storage, db->i_read_buffer - db->n_read_buffer, SEEK_CUR);
        db->i_read_buffer = 0;
        db->n_read_buffer = 0;
    }
    if (n_free > 0) {
        int to_write = n_free < n_bytes ? n_free : n_bytes;
        db->n_write_buffer += to_write;
        n_bytes -= to_write;
         for (; to_write > 0; to_write--, i++){
            db->write_buffer[i] = *_buf++;
        }
    }
    if (db->n_write_buffer == BUFFER_SIZE) {
        fwrite(db->write_buffer, 1, BUFFER_SIZE, db->file_storage);
        fwrite(_buf, 1, n_bytes, db->file_storage);
        db->n_write_buffer = 0;
    }
}

void store_relations(graph_db * db, node_relation * relations) {
    while(relations != NULL){
        db_fwrite(&relations->node, sizeof(relations->node), 1, db);
        relations = relations -> next_node_relation;
    }
    db_fwrite(&relations, sizeof(relations), 1, db);
}

void store_attrs(graph_db * db, attr * attributes) {
    int name_length = 0;
    while(attributes != NULL) {
        name_length = 1 + strlen(attributes->name_attr);
        db_fwrite(&name_length, sizeof(name_length), 1, db);
        db_fwrite(attributes->name_attr, name_length, 1 ,db);
        db_fwrite(&attributes->type_attr, sizeof(attributes->type_attr), 1, db);
        attributes = attributes->next;
    }
    name_length = 0;
    db_fwrite(&name_length, sizeof(name_length), 1, db);
}

void store_node_to_file(graph_db * db, scheme_node * node) {
    int type_length = 1 + strlen(node->type);
    db_fwrite(&node, sizeof(node), 1, db);
    db_fwrite(&type_length, sizeof(type_length), 1, db);
    db_fwrite(node->type, type_length, 1, db);
    store_relations(db, node->first_node_relation);
    store_attrs(db, node->first_attr);
}

void store_scheme_to_file(graph_db * db, db_scheme * scheme){
    scheme_node * node = scheme->first_node;
    while (node != NULL) {
        store_node_to_file(db, node);
        node = node->next_node;
    }
    db_fwrite(&node, sizeof(node), 1, db);
    node = scheme->first_node;
    while (node != NULL){
        int EMPTY_OFF_SET = 0;
        node->root_off_set = db_ftell(db);
        db_fwrite(&EMPTY_OFF_SET, sizeof(EMPTY_OFF_SET), 1, db);
        db_fwrite(&EMPTY_OFF_SET, sizeof(EMPTY_OFF_SET), 1, db);
        node = node->next_node;
    }

}

graph_db * create_new_graph_db_by_scheme(db_scheme *scheme, char *file_name){
    graph_db * new_graph_db = (graph_db *) malloc(sizeof(graph_db));
    used_mem += sizeof(graph_db);
    new_graph_db -> file_storage = fopen(file_name, "w+b");
    if (new_graph_db->file_storage) {
        int scheme_length;
        new_graph_db->scheme = scheme;
        new_graph_db->write_buffer = (char *)malloc(BUFFER_SIZE);
        used_mem += BUFFER_SIZE;
        new_graph_db->n_write_buffer = 0;
        new_graph_db->read_buffer = (char *)malloc(BUFFER_SIZE);
        used_mem += BUFFER_SIZE;
        new_graph_db->i_read_buffer = 0;
        new_graph_db->n_read_buffer = 0;
        db_fseek(new_graph_db, sizeof(int), SEEK_SET);
        store_scheme_to_file(new_graph_db, scheme);
        scheme_length = db_ftell(new_graph_db);
        db_fseek(new_graph_db, 0 , SEEK_SET);
        db_fwrite(&scheme_length, sizeof(scheme_length), 1, new_graph_db);
        db_fflush(new_graph_db);
        return new_graph_db;
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
    scheme_node * new_node;
    if (scheme -> first_node == NULL || scheme -> last_node == NULL ) {
        new_node = (scheme_node *) malloc(sizeof(scheme_node));
        used_mem += sizeof(scheme_node);
        scheme->first_node = new_node;
        scheme->last_node = new_node;
    } else {
        // check if there already exists the same node
        int n;
        if (search_node_by_type_name(scheme, type_name, &n)) // if exists
            return NULL;
        new_node = (scheme_node *) malloc(sizeof(scheme_node));
        used_mem += sizeof(scheme_node);
        scheme->last_node->next_node = new_node;
        scheme->last_node = new_node;
    }
    new_node->type = (char *) malloc(1+strlen(type_name)*sizeof(char));
    used_mem += 1 + strlen(type_name) * sizeof(char);
    strcpy(new_node->type, type_name);
    new_node->first_node_relation = NULL;
    new_node->last_node_relation = NULL;
    new_node->last_attr = NULL;
    new_node->first_attr = NULL;
    new_node->next_node = NULL;
    new_node->buffer = (char *) malloc(BUFFER_SIZE*sizeof(char));
    used_mem += BUFFER_SIZE * sizeof(char);
    new_node->n_buffer = 0;
    return new_node;
}

attr * search_attr_by_name(scheme_node * node, char * name, int * n){
    attr * current_attr = node->first_attr;
    *n = 0;
    while(current_attr != NULL) {
        if (strcmp(name, current_attr->name_attr) == 0)
            return current_attr;
        else {
            current_attr = current_attr->next;
            (*n)++;
        }
    }
    *n -= 1;
    return NULL;
}

attr * add_attr_to_node(scheme_node * node, char * name, char type) {
    attr * new_attr;
    if (node->first_attr == NULL || node->last_attr == NULL){
        new_attr = (attr *) malloc(sizeof(attr));
        used_mem += sizeof(attr);
        node->first_attr = new_attr;
        node->last_attr = new_attr;
    } else {
        int n;
        if (search_attr_by_name(node, name, &n)){
            return NULL;
        }
        new_attr = (attr *) malloc(sizeof(attr));
        used_mem += sizeof(attr);
        node->last_attr->next = new_attr;
        node->last_attr = new_attr;
    }
    new_attr->name_attr = (char *) malloc(1 + strlen(name) * sizeof(char));
    used_mem += 1 + strlen(name) * sizeof(char);
    strcpy(new_attr->name_attr, name);
    new_attr->type_attr = type;
    new_attr->next = NULL;

    return new_attr;
}

node_relation * search_node_relation_by_nodes (scheme_node * node, scheme_node * next_related_node){
    node_relation * current_node_relation = node->first_node_relation;
    while(current_node_relation != NULL) {
        if (next_related_node == current_node_relation->node)
            return current_node_relation;
        else
            current_node_relation = current_node_relation->next_node_relation;
    }
    return NULL;
}

node_relation * add_node_relation(scheme_node * node, scheme_node * next_related_node){
    node_relation * new_node_relation;

    if (search_node_relation_by_nodes(node, next_related_node))
        return NULL;

    new_node_relation = (node_relation *) malloc(sizeof(node_relation));
    used_mem += sizeof(node_relation);
    new_node_relation->node = next_related_node;
    new_node_relation->next_node_relation = NULL;

    if (node->first_node_relation == NULL || node->last_node_relation == NULL) {
        node->first_node_relation = new_node_relation;
        node->last_node_relation = new_node_relation;
    } else {
        node->last_node_relation->next_node_relation = new_node_relation;
        node->last_node_relation = new_node_relation;
    }

    return new_node_relation;
}

void del_node_relation(scheme_node * node, scheme_node * to_delete_node){
    if (node->first_node_relation != NULL && node -> last_node_relation != NULL) {
        if (node->first_node_relation == node -> last_node_relation) {
            if (node->first_node_relation ->node == to_delete_node) {
                used_mem -= sizeof(node_relation);
                free(node->first_node_relation);
                node->first_node_relation = NULL;
                node->last_node_relation = NULL;
            }
        } else if (node->first_node_relation->node == to_delete_node) {
            node_relation * deleted = node->first_node_relation;
            node->first_node_relation = node->first_node_relation->next_node_relation;
            used_mem -= sizeof(node_relation);
            free(deleted);
        } else {
            node_relation * prev = node->first_node_relation;
            while (prev != NULL && prev->next_node_relation->node != to_delete_node) {
                prev = prev->next_node_relation;
            }
            if (prev != NULL) {
                node_relation * deleted = prev->next_node_relation;
                if (node->last_node_relation->node == to_delete_node){
                    node->last_node_relation = prev;
                    prev->next_node_relation = NULL;
                } else {
                    prev -> next_node_relation = prev ->next_node_relation ->next_node_relation;
                }
                used_mem -= sizeof(node_relation);
                free(deleted);
            }
        }
    }
}

void free_node_from_scheme(scheme_node * node) {
    node_relation * relation = node->first_node_relation;
    while(relation != NULL){
        node_relation * to_delete = relation;
        used_mem -= sizeof(node_relation);
        free(to_delete);
        relation = relation->next_node_relation;
    }

    attr * cur_attr = node->first_attr;
    while(cur_attr != NULL) {
        attr * to_delete = cur_attr;
        used_mem -= sizeof(attr);
        free(to_delete);
        cur_attr = cur_attr ->next;
    }

    used_mem -= 1 + strlen(node->type);
    free(node->type);

    used_mem -= sizeof(scheme_node);
    free(node);
}

void del_node_from_scheme(db_scheme * scheme, scheme_node * node){
    if (scheme->first_node != NULL && scheme->last_node != NULL) {
        if (scheme->first_node == scheme->last_node){ // scheme has only one node
            free_node_from_scheme(node);
            scheme->first_node = NULL;
            scheme->last_node = NULL;
        } else if (scheme->first_node == node) {
            scheme->first_node = node->next_node;
            free_node_from_scheme(node);
        } else{
            scheme_node * prev = scheme->first_node;
            while (prev != NULL && prev->next_node != node) {
                prev = prev->next_node;
            }
            if (prev != NULL) {
                if (scheme->last_node == node) {
                    scheme->last_node = prev;
                    prev->next_node = NULL;
                } else {
                    prev->next_node = node->next_node;
                }
                free_node_from_scheme(node);
            }
        }
    }
}

void del_attr_from_node(scheme_node * node, attr * to_delete_attr){
    if (node->first_attr != NULL && node->last_attr != NULL){
        // case has only one attribute
        if (node->first_attr == node->last_attr) {
            free(node->first_attr);
            used_mem -= sizeof(attr);
            node->first_attr = NULL;
            node->last_attr = NULL;
        } else if (node->first_attr == to_delete_attr){
            attr * deleted = node ->first_attr;
            used_mem -= sizeof(attr);
            node->first_attr = node->first_attr->next;
            free(deleted);
        } else {
            attr * prev = node ->first_attr;
            while (prev != NULL && prev->next != to_delete_attr){
                prev = prev->next;
            }
            if (prev != NULL) {
                attr * deleted = prev->next;
                if (node->last_attr == to_delete_attr) {
                    node->last_attr = prev;
                    prev->next = NULL;
                } else {
                    prev->next = prev->next->next;
                }
                used_mem -= sizeof(attr);
                free(deleted);
            }

        }
    }
}

void cancel_editing_node(scheme_node * node) {
    node->n_buffer = 0;
    node->added = 0;
}

void create_node_for_db(graph_db * db, scheme_node * node){
    attr * cur_attr = node->first_attr;
    int i;
    cancel_editing_node(node);
    node->n_buffer = 0;
    node->added = 1;
    while (cur_attr != NULL) {
        write_buffer(node->buffer, &node->n_buffer, 0.0);
        cur_attr = cur_attr->next;
    }
    write_buffer(node->buffer, &node->n_buffer, 0.0);

    for (i=0; i < MAX_NODE_RELATIONS; i++) {
        write_buffer(node->buffer, &node->n_buffer, 0.0);
        write_buffer(node->buffer, &node->n_buffer, 0.0);
    }
}

void set_value_for_attr_of_node(graph_db * db, scheme_node * node, char * attr_name, float value){
    int n;
    if (node->n_buffer > 0 && search_attr_by_name(node, attr_name, &n)){
        n *= sizeof(float);
        write_buffer(node->buffer, &n, value);
    }
}

void shutdown_db(graph_db * db){
    return;
}
