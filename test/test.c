#include "../headers/graph_db.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(){
    char * country[5] = {"Russia", "Germany", "England", "Canada", "Spain"};
    char * city[10] = {"Saint Petersburg", "Kaliningrad", "Berlin", "Frankfurt", "London", "Liverpool", "Toronto", "Vancouver", "Barcelona", "Majorca"};

    graph_db * db;
    db_scheme * scheme;
    condition * cond;
    condition * cond2;
    node_set_item * ns;
    node_set_item * ns1;
    node_set_item * ns2;
    node_set_item * ns12;
    scheme_node * country_node;
    scheme_node * city_node;
    scheme_node * deleted_node;

    init_db("config.cfg");

    scheme = create_new_scheme();

    // create CITY node
    country_node = add_node_to_scheme(scheme, "country");
    add_attr_to_node(country_node, "name", AT_STRING);
    add_attr_to_node(country_node, "population", AT_INT32);

    // create COUNTRY node
    city_node = add_node_to_scheme(scheme, "city");
    add_attr_to_node(city_node, "area", AT_INT32);
    add_attr_to_node(city_node, "name", AT_STRING);
    add_attr_to_node(city_node, "to_delete", AT_BOOLEAN);

    // create DELETED node
    deleted_node = add_node_to_scheme(scheme, "deleted");
    add_attr_to_node(deleted_node, "signature", AT_INT32);

    // ADD RELATION
    add_node_relation(country_node, deleted_node);
    add_node_relation(country_node, city_node);

    // DELETE RELATION
    del_node_relation(country_node, deleted_node);
    // DELETE NODE
    del_node_from_scheme(scheme, deleted_node);

    // DELETE ATTR FROM NODE
    int i;
    del_attr_from_node(city_node, search_attr_by_name(city_node, "to_delete", &i));
    db = create_new_graph_db_by_scheme(scheme, "storage.txt");
    for (i = 0; i < 10; i++){
        if (i%2 == 0){
            create_node_for_db(db, country_node);
            set_value_for_attr_of_node(db, country_node, "name", create_string_for_db(db, country[i/2]));
            set_value_for_attr_of_node(db, country_node, "population", 1000000 + i*4000);
            post_node_to_db(db, country_node);
        }
        create_node_for_db(db, city_node);
        set_value_for_attr_of_node(db, city_node, "name", create_string_for_db(db, city[i]));
        set_value_for_attr_of_node(db, city_node, "area", 20000 + i * 100);
        post_node_to_db(db, city_node);
        open_node_to_db(db, country_node);
        if (!link_current_node_to_current_node(db, country_node, city_node)){
            printf("Can't link!\n");
        }
        post_node_to_db(db, country_node);
    }
    restart_node_pointer(db, country_node);
    i = 0;
    while (open_node_to_db(db, country_node)){
        int population;
        char * name;
        population = get_attr_value_of_node(country_node, "population");
        name = get_string_from_db(db, get_attr_value_of_node(country_node, "name"));
        printf("%s [%i]\n", name, population);
        register_free(1 + strlen(name));
        free(name);
        next_node(db, country_node);
        ++i;
    }
    printf("%d\n", get_used_mem());
    printf("There is %i countries\n", i);
    cond = create_int_or_bool_attr_condition(OP_LESS, "population", 1010000);
    // Cypher-query: Match(j:country) where j.population < 1010000 return j;
    ns = query_all_nodes_of_type(db, country_node, cond);
    ns1 = ns;
    i = 0;
    while (ns1 != NULL) {
        ns1 = ns1->next;
        ++i;
    }
    free_node_set(db, ns);
    printf("MATCH (j:country) WHERE j.population < 1010000 RETURN j;  =>  %i countries has population less than 1010000!\n", i);

    cond2 = create_logic_condition(
        OP_AND,
        create_string_attr_condition(OP_NOT_EQUAL, "name", "Saint Petersburg"),
        create_string_attr_condition(OP_NOT_EQUAL, "name", "Liverpool")
    );
    // MATCH (j:country) - [:DIRECTED]->(a:city) Where (j.population < 1010000) AND (a.name != "Saint Petersburg") AND (a.name != "Liverpool") return a
    ns2 = query_cypher_style(db, 2, country_node, cond, city_node, cond2);
    ns12 = ns2;
    i = 0;
    printf("MATCH (j:country)-[:DIRECTED]->(a:city) WHERE (j.population < 1010000) AND (a.name != 'Saint Petersburg') AND (a.name != 'Liverpool') RETURN a;  =>\n");
    while (ns12 != NULL){
        navigate_by_node_set_item(db, ns12);
        if (open_node_to_db(db, city_node)){
            char * name = get_string_from_db(db, get_attr_value_of_node(city_node, "name"));
            printf("%s [%i]\n", name, (int) get_attr_value_of_node(city_node, "area"));
            register_free(strlen(name)+1);
            free(name);
            cancel_editing_node(city_node);
        } else
            printf("Can't open city node!\n");
        ns12 = ns12->next;
        i++;
    }
    free_node_set(db, ns2);
    printf("%i cities selected!\n", i);
    // return 0;
    // MATCH (j:country)-[:DIRECTED]->(a:city) WHERE (j.population < 1010000) AND (a.name != "Saint Petersburg") AND (a.name != 'Liverpool') SET a.area=20100 RETURN a;
    printf("MATCH (j:country)-[:DIRECTED]->(a:city) WHERE (j.population < 1010000) AND (a.name != 'Saint Petersburg') AND (a.name != 'Liverpool') SET a.area=20100 RETURN a;\n");
    set_cypher_style(db, "area", 20100, 2, country_node, cond, city_node, cond2);
    // MATCH (j:country)-[:DIRECTED]->(a:city) WHERE (j.population < 1010000) AND (a.name != "Saint Petersburg") AND (a.name != 'Liverpool') RETURN a;
    printf("MATCH (j:country)-[:DIRECTED]->(a:city) WHERE (j.population < 1010000) AND (a.name != 'Saint Petersburg') AND (a.name != 'Liverpool') RETURN a;  =>\n");
    ns2 = query_cypher_style(db, 2, country_node, cond, city_node, cond2);
    ns12 = ns2;
    i = 0;
    while (ns12 != NULL) {
        navigate_by_node_set_item(db, ns12);
        if (open_node_to_db(db, city_node)) {
            char * Family = get_string_from_db(db, get_attr_value_of_node(city_node, "name"));
            printf("%s [%i]\n", Family, (int)get_attr_value_of_node(city_node, "area"));
            register_free(strlen(Family)+1);
            free(Family);
            cancel_editing_node(city_node);
        } else
            printf("Can't open city node!\n");
        ns12 = ns12->next;
        i++;
    }
    free_node_set(db, ns2);
    printf("%i cities selected!\n", i);

    // MATCH (j:country)-[:DIRECTED]->(a:city) WHERE (j.population < 1010000) AND (a.name != "Saint Petersburg") AND (a.name != 'Liverpool') DELETE a;
    printf("MATCH (j:country)-[:DIRECTED]->(a:city) WHERE (j.population < 1010000) AND (a.name != 'Saint Petersburg') AND (a.name != 'Liverpool') DELETE a;  =>\n");

    delete_cypher_style(db, 2, country_node, cond, city_node, cond2);
    printf("MATCH (j:country)-[:DIRECTED]->(a:city) WHERE (j.population < 1010000) AND (a.name != 'Saint Petersburg') AND (a.name != 'Liverpool') RETURN a;  =>\n");
    ns2 = query_cypher_style(db, 2, country_node, cond, city_node, cond2);
    ns12 = ns2;
    i = 0;
    while (ns12 != NULL) {
        navigate_by_node_set_item(db, ns12);
        if (open_node_to_db(db, city_node)) {
            char * Family = get_string_from_db(db, get_attr_value_of_node(city_node, "name"));
            printf("%s [%i]\n", Family, (int)get_attr_value_of_node(city_node, "area"));
            free(Family);
            cancel_editing_node(city_node);
        } else
            printf("Can't open city node!\n");
        ns12 = ns12->next;
        i++;
    }
    free_node_set(db, ns2);
    printf("%i cities selected!\n", i);

    restart_node_pointer(db, city_node);
    i = 0;
    while (open_node_to_db(db, city_node)) {
        next_node(db, city_node);
        i++;
    }
    printf("There is %i cities\n", i);

    restart_node_pointer(db, city_node);
    i = 0;
    while (open_node_to_db(db, city_node)) {
        delete_node(db, city_node);
        i++;
    }
    printf("There is %i cities deleted\n", i);

    close_db(db);

    free_condition(cond);
    free_condition(cond2);

    if (get_used_mem() == 0)
        printf("Memory is freed correctly!\n");
    else
        printf("Not freed: %i bytes!\n", get_used_mem());

    return 0;
}
