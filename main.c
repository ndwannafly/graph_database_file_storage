#include "headers/graph_db.h"
#include <stdio.h>

int main(){
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

    // create DELETED node
    deleted_node = add_node_to_scheme(scheme, "deleted");
    add_attr_to_node(deleted_node, "code", AT_INT32);


    add_node_relation(city_node, deleted_node);
    add_node_relation(city_node, country_node);

    printf("%d", get_used_mem());
    return 0;
}
