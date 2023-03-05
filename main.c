#include "headers/graph_db.h"

int main(){
    scheme_node * city_node;

    db_scheme * scheme = create_new_scheme();
    city_node = add_node_to_scheme(scheme, "city");
    return 0;
}
