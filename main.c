#include "headers/graph_db.h"
#include <stdio.h>

int main(){
    scheme_node * city_node;

    db_scheme * scheme = create_new_scheme();
    city_node = add_node_to_scheme(scheme, "city");
    add_attr_to_node(city_node, "name", AT_STRING);

    printf("%d", get_used_mem());
    return 0;
}
