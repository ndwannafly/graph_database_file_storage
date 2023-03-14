#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../headers/graph_db.h"

#define n_movies 5
#define max_rand_str_length 99

char * gen_rand_str() {
    int n = rand() % (max_rand_str_length+1);
    char * result = (char *) malloc(n+1);
    int i;
    for (i = 0; i < n; i++)
        result[i] = 'A' + (rand() % ('Z'-'A'+1));
    result[n] = 0;
    return result;
}

double seconds() {
#ifdef _WIN32
    return (1.0*clock()/CLK_TCK);
#else
    return (1.0*clock()/CLOCKS_PER_SEC);
#endif
}

int main(){
    char *Titles[n_movies] = {"Mist", "Prometeus", "Flew over the cookoo's nest", "Shawshank redemption", "AfterNoon"};
    db_scheme *scheme;
    graph_db *db;
    scheme_node *movie_node;
    scheme_node *actor_node;
    condition *cond;
    node_set_item *ns;
    double start;
    int n_links;
    int i, j;

    init_db("config.cfg");

    scheme = create_new_scheme();

    movie_node = add_node_to_scheme(scheme, "Movie");
    add_attr_to_node(movie_node, "Title", AT_STRING);
    add_attr_to_node(movie_node, "Year", AT_INT32);
    actor_node = add_node_to_scheme(scheme, "Actor");
    add_attr_to_node(actor_node, "Family", AT_STRING);
    add_attr_to_node(actor_node, "Name", AT_STRING);
    add_attr_to_node(actor_node, "Oscar", AT_BOOLEAN);
    add_attr_to_node(actor_node, "Year_of_birthday", AT_INT32);

    add_node_relation(movie_node, actor_node);

    db = create_new_graph_db_by_scheme(scheme, "graphs.mydb");
    printf("Memory consumption (insertions+updates+deletings) [O(1)]:\n");
    for (i = 0; i < n_movies*200; i++) {
        char * Family = gen_rand_str();
        if (i % 200 == 0) {
            printf("%i items : %i bytes\n", i, get_used_mem());
            create_node_for_db(db, movie_node);
            set_value_for_attr_of_node(db, movie_node, "Title", create_string_for_db(db, Titles[i/200]));
            set_value_for_attr_of_node(db, movie_node, "Year", 2000 + i);
            post_node_to_db(db, movie_node);
            n_links = 0;
        }
        create_node_for_db(db, actor_node);
        set_value_for_attr_of_node(db, actor_node, "Family", create_string_for_db(db, Family));
        free(Family);
        set_value_for_attr_of_node(db, actor_node, "Year_of_birthday", 1980 + i);
        post_node_to_db(db, actor_node);
        open_node_to_db(db, movie_node);
        if (n_links < get_max_links_num()) {
            if (!link_current_node_to_current_node(db, movie_node, actor_node))
                printf("Can't connect!\n");
            n_links++;
        }
        post_node_to_db(db, movie_node);
        if ((i % 200) > 150)
            delete_node(db, actor_node);
    }

    printf("Timings (insertions) [O(1)]:\n");
    printf("File size is proportional to number of items inserted:\n");
    start = seconds();
    for (i = 0; i < n_movies*2000; i++) {
        char * Title = gen_rand_str();
        if (i % 2000 == 0) {
            printf("%i inserted : %lf seconds elapsed to insert 2000 items: fileSize = %li\n", i, (seconds() - start), get_db_size(db));
            start = seconds();
        }
        create_node_for_db(db, movie_node);
        set_value_for_attr_of_node(db, movie_node, "Title", create_string_for_db(db, Title));
        free(Title);
        set_value_for_attr_of_node(db, movie_node, "Year", 2000 + i);
        post_node_to_db(db, movie_node);
    }

    printf("Deleting... ");
    delete_cypher_style(db, 1, movie_node, NULL);
    printf("done\n");

    cond = create_int_or_bool_attr_condition(OP_LESS, "Year", 2004);

    printf("Timings of queries (selection, updating) [O(n)]:\n");
    for (i = 0; i < n_movies*4000; i++) {
        char * Title = gen_rand_str();
        if (i % 4000 == 0) {
            start = seconds();

            ns = query_cypher_style(db, 1, movie_node, cond);
            free_node_set(db, ns);

            printf("n = %i : Time to select = %lf [seconds]\n", i, (seconds() - start));

            start = seconds();

            set_cypher_style(db, "Year", 1975, 1, movie_node, cond);

            printf("n = %i : Time to update = %lf [seconds]\n", i, (seconds() - start));
        }
        create_node_for_db(db, movie_node);
        set_value_for_attr_of_node(db, movie_node, "Title", create_string_for_db(db, Title));
        free(Title);
        set_value_for_attr_of_node(db, movie_node, "Year", 2000 + i);
        post_node_to_db(db, movie_node);
    }

    printf("Deleting... ");
    delete_cypher_style(db, 1, movie_node, NULL);
    printf("done\n");

    printf("Timings of queries (deletion) [O(n)]:\n");
    for (j = 0; j < n_movies; j++) {
        for (i = 0; i < j*4000; i++) {
            char * Title = gen_rand_str();
            create_node_for_db(db, movie_node);
            set_value_for_attr_of_node(db, movie_node, "Title", create_string_for_db(db, Title));
            free(Title);
            set_value_for_attr_of_node(db, movie_node, "Year", 2000 + i);
            post_node_to_db(db, movie_node);
        }
        start = seconds();

        delete_cypher_style(db, 1, movie_node, cond);

        printf("n = %i : Time to delete = %lf [seconds]\n", j*4000, (seconds() - start));
    }

    close_db(db);
    free_condition(cond);

    if (get_used_mem() == 0)
        printf("Memory is freed correctly!\n");
    else
        printf("Not freed: %i bytes!\n", get_used_mem());

    return 0;
}
