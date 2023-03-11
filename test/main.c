#include "../headers/graph_db.h"
#include "../headers/data_helper.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(){
    char * movie_name[5] = {"Mist", "Prometeus", "Flew over the cookoo's nest", "Shawshank redemption", "AfterNoon"};
    char * Families[10] = {"Stepanov", "Hamatova", "Churikova", "Pitt", "Delon", "Williams", "Nickolson", "Boyarskaya", "Freeman", "De Vito"};

    graph_db * db;
    db_scheme * scheme;
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
    printf("%d\n", get_used_mem());

    // DELETE RELATION
    del_node_relation(movie_node, deleted_node);
    printf("%d\n", get_used_mem());
    // DELETE NODE
    del_node_from_scheme(scheme, deleted_node);
    printf("%d\n", get_used_mem());

    // DELETE ATTR FROM NODE
    int i;
    del_attr_from_node(actor_node, search_attr_by_name(actor_node, "toDelete", &i));
    printf("%d\n", get_used_mem());
    db = create_new_graph_db_by_scheme(scheme, "storage.txt");
    printf("%d\n", get_used_mem());

    for (i = 0; i < 10; i++){
        if (i%2 == 0){
            create_node_for_db(db, movie_node);
            set_value_for_attr_of_node(db, movie_node, "Title", create_string_for_db(db, movie_name[i/2]));
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
    return 0;
    restart_node_pointer(db, movie_node);
    i = 0;
    while (open_node_to_db(db, movie_node)){
        int year;
        char * title;
        year = get_attr_value_of_node(movie_node, "Year");
        title = get_string_from_db(db, get_attr_value_of_node(movie_node, "title"));
        printf("%s [%i]\n", title, year);
        register_free(1 + strlen(title));
        free(title);
        next_node(db, movie_node);
        ++i;
    }
    // get used_mem
    printf("%d\n", get_used_mem());
    printf("There is %i movies\n", i);
    return 0;
}
