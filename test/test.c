#include "../headers/graph_db.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(){
    char * Titles[5] = {"Mist", "Prometeus", "Flew over the cookoo's nest", "Shawshank redemption", "AfterNoon"};
    char * Families[10] = {"Stepanov", "Hamatova", "Churikova", "Pitt", "Delon", "Williams", "Nickolson", "Boyarskaya", "Freeman", "De Vito"};

    graph_db * db;
    db_scheme * scheme;
    condition * cond;
    condition * cond2;
    node_set_item * ns;
    node_set_item * ns1;
    node_set_item * ns2;
    node_set_item * ns12;
    scheme_node * movie_node;
    scheme_node * actor_node;
    scheme_node * deleted_node;

    init_db("config.cfg");

    scheme = create_new_scheme();

    // create CITY node
    movie_node = add_node_to_scheme(scheme, "Movie");
    add_attr_to_node(movie_node, "Title", AT_STRING);
    add_attr_to_node(movie_node, "Year", AT_INT32);

    // create COUNTRY node
    actor_node = add_node_to_scheme(scheme, "Actor");
    add_attr_to_node(actor_node, "Family", AT_STRING);
    add_attr_to_node(actor_node, "Name", AT_STRING);
    add_attr_to_node(actor_node, "Oscar", AT_BOOLEAN);
    add_attr_to_node(actor_node, "Year_Of_birthday", AT_INT32);
    add_attr_to_node(actor_node, "toDelete", AT_BOOLEAN);

    // create DELETED node
    deleted_node = add_node_to_scheme(scheme, "Deleted");
    add_attr_to_node(deleted_node, "Signature", AT_INT32);

    // ADD RELATION
    add_node_relation(movie_node, deleted_node);
    add_node_relation(movie_node, actor_node);

    // DELETE RELATION
    del_node_relation(movie_node, deleted_node);
    // DELETE NODE
    del_node_from_scheme(scheme, deleted_node);

    // DELETE ATTR FROM NODE
    int i;
    del_attr_from_node(actor_node, search_attr_by_name(actor_node, "toDelete", &i));
    db = create_new_graph_db_by_scheme(scheme, "storage.txt");
    for (i = 0; i < 10; i++){
        if (i%2 == 0){
            create_node_for_db(db, movie_node);
            set_value_for_attr_of_node(db, movie_node, "Title", create_string_for_db(db, Titles[i/2]));
            set_value_for_attr_of_node(db, movie_node, "Year", 2000 + i);
            post_node_to_db(db, movie_node);
        }
        create_node_for_db(db, actor_node);
        set_value_for_attr_of_node(db, actor_node, "Family", create_string_for_db(db, Families[i]));
        set_value_for_attr_of_node(db, actor_node, "Year_of_birthday", 1980 + i);
        post_node_to_db(db, actor_node);
        open_node_to_db(db, movie_node);
        if (!link_current_node_to_current_node(db, movie_node, actor_node)){
            printf("Can't link!\n");
        }
        post_node_to_db(db, movie_node);
    }
    restart_node_pointer(db, movie_node);
    i = 0;
    while (open_node_to_db(db, movie_node)){
        int year;
        char * title;
        year = get_attr_value_of_node(movie_node, "Year");
        title = get_string_from_db(db, get_attr_value_of_node(movie_node, "Title"));
        printf("%s [%i]\n", title, year);
        register_free(1 + strlen(title));
        free(title);
        next_node(db, movie_node);
        ++i;
    }
    printf("%d\n", get_used_mem());
    printf("There is %i movies\n", i);
    cond = create_int_or_bool_attr_condition(OP_LESS, "Year", 2004);
    // Cypher-query: Match(j:movie) where j.YEAR < 2004 return j;
    ns = query_all_nodes_of_type(db, movie_node, cond);
    ns1 = ns;
    i = 0;
    while (ns1 != NULL) {
        ns1 = ns1->next;
        ++i;
    }
    free_node_set(db, ns);
    printf("MATCH (j:Movie) WHERE j.Year < 2004 RETURN j;  =>  %i movies earlier 2004!\n", i);

    cond2 = create_logic_condition(
        OP_AND,
        create_string_attr_condition(OP_NOT_EQUAL, "Family", "Pitt"),
        create_string_attr_condition(OP_NOT_EQUAL, "Family", "Hamatova")
    );
    // MATCH (j:Movie) - [:DIRECTED]->(a:Actor) Where (j.Year < 2004) AND (a.Family != "Pitt") AND (a.Family != "Hamatova") return a
    ns2 = query_cypher_style(db, 2, movie_node, cond, actor_node, cond2);
    ns12 = ns2;
    i = 0;
    printf("MATCH (j:Movie)-[:DIRECTED]->(a:Actor) WHERE (j.Year < 2004) AND (a.Family != 'Pitt') AND (a.Family != 'Hamatova') RETURN a;  =>\n");
    while (ns12 != NULL){
        navigate_by_node_set_item(db, ns12);
        if (open_node_to_db(db, actor_node)){
            char * Family = get_string_from_db(db, get_attr_value_of_node(actor_node, "Family"));
            printf("%s [%i]\n", Family, (int) get_attr_value_of_node(actor_node, "Year_of_birthday"));
            register_free(strlen(Family)+1);
            free(Family);
            cancel_editing_node(actor_node);
        } else
            printf("Can't open actor node!\n");
        ns12 = ns12->next;
        i++;
    }
    free_node_set(db, ns2);
    printf("%i actors selected!\n", i);
    // MATCH (j:Movie)-[:DIRECTED]->(a:Actor) WHERE (j.Year < 2004) AND (a.Family != 'Pitt') AND (a.Family != 'Hamatova') SET a.Year_of_birthday=1975 RETURN a;
    printf("MATCH (j:Movie)-[:DIRECTED]->(a:Actor) WHERE (j.Year < 2004) AND (a.Family != 'Pitt') AND (a.Family != 'Hamatova') SET a.Year_of_birthday=1975 RETURN a;\n");
    set_cypher_style(db, "Year_of_birthday", 1975, 2, movie_node, cond, actor_node, cond2);
    // MATCH (j:Movie)-[:DIRECTED]->(a:Actor) WHERE (j.Year < 2004) AND (a.Family != 'Pitt') AND (a.Family != 'Hamatova') RETURN a;
    printf("MATCH (j:Movie)-[:DIRECTED]->(a:Actor) WHERE (j.Year < 2004) AND (a.Family != 'Pitt') AND (a.Family != 'Hamatova') RETURN a;  =>\n");
    ns2 = query_cypher_style(db, 2, movie_node, cond, actor_node, cond2);
    ns12 = ns2;
    i = 0;
    while (ns12 != NULL) {
        navigate_by_node_set_item(db, ns12);
        if (open_node_to_db(db, actor_node)) {
            char * Family = get_string_from_db(db, get_attr_value_of_node(actor_node, "Family"));
            printf("%s [%i]\n", Family, (int)get_attr_value_of_node(actor_node, "Year_of_birthday"));
            register_free(strlen(Family)+1);
            free(Family);
            cancel_editing_node(actor_node);
        } else
            printf("Can't open actor node!\n");
        ns12 = ns12->next;
        i++;
    }
    free_node_set(db, ns2);
    printf("%i actors selected!\n", i);

    // MATCH (j:Movie)-[:DIRECTED]->(a:Actor) WHERE (j.Year < 2004) AND (a.Family != 'Pitt') AND (a.Family != 'Hamatova') DELETE a;
    printf("MATCH (j:Movie)-[:DIRECTED]->(a:Actor) WHERE (j.Year < 2004) AND (a.Family != 'Pitt') AND (a.Family != 'Hamatova') DELETE a;  =>\n");

    delete_cypher_style(db, 2, movie_node, cond, actor_node, cond2);
    printf("MATCH (j:Movie)-[:DIRECTED]->(a:Actor) WHERE (j.Year < 2004) AND (a.Family != 'Pitt') AND (a.Family != 'Hamatova') RETURN a;  =>\n");
    ns2 = query_cypher_style(db, 2, movie_node, cond, actor_node, cond2);
    ns12 = ns2;
    i = 0;
    while (ns12 != NULL) {
        navigate_by_node_set_item(db, ns12);
        if (open_node_to_db(db, actor_node)) {
            char * Family = get_string_from_db(db, get_attr_value_of_node(actor_node, "Family"));
            printf("%s [%i]\n", Family, (int)get_attr_value_of_node(actor_node, "Year_of_birthday"));
            free(Family);
            cancel_editing_node(actor_node);
        } else
            printf("Can't open actor node!\n");
        ns12 = ns12->next;
        i++;
    }
    free_node_set(db, ns2);
    printf("%i actors selected!\n", i);

    restart_node_pointer(db, actor_node);
    i = 0;
    while (open_node_to_db(db, actor_node)) {
        next_node(db, actor_node);
        i++;
    }
    printf("There is %i Actors\n", i);

    restart_node_pointer(db, actor_node);
    i = 0;
    while (open_node_to_db(db, actor_node)) {
        delete_node(db, actor_node);
        i++;
    }
    printf("There is %i Actors deleted\n", i);

    close_db(db);
    free_condition(cond);
    free_condition(cond2);

    if (get_used_mem() == 0)
        printf("Memory is freed correctly!\n");
    else
        printf("Not freed: %i bytes!\n", get_used_mem());

    return 0;
}
