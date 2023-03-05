#include "headers/graph_db.h"
#include <stdio.h>

int main(){

    graph_db * db;
    db_scheme * scheme;
    scheme_node * city_node;
    scheme_node * country_node;
    scheme_node * deleted_node;

    scheme = create_new_scheme();

    // create CITY node
    city_node = add_node_to_scheme(scheme, "city");
    add_attr_to_node(city_node, "name", AT_STRING);

    // create COUNTRY node
    country_node = add_node_to_scheme(scheme, "country");
    add_attr_to_node(country_node, "name", AT_STRING);
    add_attr_to_node(country_node, "to_be_deleted", AT_BOOLEAN);

    // create DELETED node
    deleted_node = add_node_to_scheme(scheme, "deleted");
    add_attr_to_node(deleted_node, "code", AT_INT32);

    // ADD RELATION
    add_node_relation(city_node, deleted_node);
    add_node_relation(city_node, country_node);
    printf("%d\n", get_used_mem());

    // DELETE RELATION
    del_node_relation(city_node, deleted_node);
    printf("%d\n", get_used_mem());
    // DELETE NODE
    del_node_from_scheme(scheme, deleted_node);
    printf("%d\n", get_used_mem());

    // DELETE ATTR FROM NODE
    int i;
    del_attr_from_node(country_node, search_attr_by_name(country_node, "to_be_deleted", &i));

    db = create_new_graph_db_by_scheme(scheme, "storage.txt");

    // get used_mem
    printf("%d\n", get_used_mem());


    return 0;
}
