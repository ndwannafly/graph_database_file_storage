#ifndef LAB1_GRAPH_DB
#define LAB1_GRAPH_DB
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

enum attr_type{AT_INT32, AT_FLOAT, AT_STRING, AT_BOOLEAN};
enum operand_type{OPRD_INT_BOOL_FLOAT, OPRD_STRING, OPRD_ATTR_NAME, OPRD_CONDITION};
enum operation_type{OP_EQUAL, OP_NOT_EQUAL, OP_LESS, OP_GREATER, OP_NOT, OP_AND, OP_OR};
enum record_type{R_EMPTY, R_STRING, R_NODE};

struct attr;
struct scheme_node;
struct condition;


typedef struct {
    unsigned char operand_type;
    union {
        struct condition * op_condition;
        char * op_string;
        float op_int_bool_float;
        char * op_attr_name;
    };
} condition_operation;
typedef struct condition{
    unsigned char operation_type;
    condition_operation * operand_1;
    condition_operation * operand_2;
} condition;
typedef struct node_relation{
    struct scheme_node * node;
    struct node_relation * next_node_relation;
} node_relation;

typedef struct scheme_node{
    struct attr * first_attr;
    struct attr * last_attr;
    struct scheme_node * next_node;
    node_relation * first_node_relation;
    node_relation * last_node_relation;
    char * type;
    int root_off_set;
    int first_off_set;
    int last_off_set;
    int added;
    int n_buffer;
    char * buffer;
    int prev_offset;
    int this_offset;
} scheme_node;
typedef struct node_set_item{
    scheme_node * node;
    int prev_offset;
    int this_offset;
    struct node_set_item * next;
    struct node_set_item * prev;
} node_set_item;
typedef struct attr {
    char * name_attr;
    unsigned char type_attr;
    struct attr * next;
} attr;

typedef struct {
    scheme_node * first_node;
    scheme_node * last_node;
} db_scheme;

typedef struct {
    db_scheme * scheme;
    FILE * file_storage;
    char * write_buffer;
    int n_write_buffer;
    char * read_buffer;
    int n_read_buffer;
    int i_read_buffer;
} graph_db;


void init_db(char * file_name);
void close_db(graph_db *db);
int get_used_mem();
int get_max_links_num();
long int get_db_size(graph_db * db);
db_scheme * create_new_scheme();

graph_db * create_new_graph_db_by_scheme(db_scheme * scheme, char * file_name);

scheme_node * add_node_to_scheme(db_scheme * scheme, char * type_name);

scheme_node * search_node_by_type_name(db_scheme * scheme, char * type_name, int * n);

attr * add_attr_to_node(scheme_node * node, char * name, unsigned char type);

attr * search_attr_by_name(scheme_node * node, char * name, int * n);

node_relation * add_node_relation(scheme_node * node, scheme_node * next_related_node);

node_relation * search_node_relation_by_nodes (scheme_node * node, scheme_node * next_related_node);

int create_string_for_db(graph_db * db, char * s);

void del_node_relation(scheme_node * node, scheme_node * to_delete_node);
void del_node_from_scheme(db_scheme * scheme, scheme_node * node);
void del_attr_from_node(scheme_node * node, attr * to_delete_attr);
void free_node_from_scheme(scheme_node * node);
void store_attrs(graph_db * db, attr * attributes);
void store_relations(graph_db * db, node_relation * relations);


void register_free(int amount);
int next_node(graph_db * db, scheme_node * node);
void cancel_editing_node(scheme_node * node);
void create_node_for_db(graph_db * db, scheme_node * node);
void post_node_to_db(graph_db * db, scheme_node * node);
int open_node_to_db(graph_db * db, scheme_node * node);
void restart_node_pointer(graph_db * db, scheme_node * node);
int link_current_node_to_current_node(graph_db * db, scheme_node * from_node, scheme_node * to_node);

void set_value_for_attr_of_node(graph_db * db, scheme_node * node, char * attr_name, float value);
float get_attr_value_of_node(scheme_node * node, char * attr_name);
char * get_string_from_db (graph_db * db, int offset);

condition * create_int_or_bool_attr_condition(unsigned char operation, char * attr_name, int val);
condition * create_float_attr_condition(unsigned char operation, char * attr_name, float val);
condition * create_string_attr_condition(unsigned char operation, char * attr_name, char * val);
condition * create_logic_condition(unsigned char operation, condition * operand_1, condition * operand_2);

node_set_item * query_all_nodes_of_type(graph_db * db, scheme_node * node, condition * cond);
node_set_item * query_cypher_style(graph_db * db, int n_links, ...);
node_set_item * query_node_set(graph_db * DB, node_set_item * node_set, condition * cond);
void navigate_by_node_set_item (graph_db * db, node_set_item * node_set);
float * get_directed_to_list(graph_db * db, scheme_node * node, int * n);
void delete_cypher_style(graph_db * db, int n_links, ...);
int delete_node(graph_db * db, scheme_node * node);
void set_node_attr(graph_db * db, scheme_node * node, char * attr_name, float value);
void set_cypher_style(graph_db * db, char * attr_name, float attr_val, int n_links, ...);
int test_node_condition(graph_db * db, scheme_node * node, condition * cond);

void free_node_set(graph_db * db, node_set_item * node_set);
void free_db_scheme_node(scheme_node * node);
void free_db_scheme(db_scheme * scheme);
void free_db_scheme_attr(attr * at);
void free_condition(condition * cond);
#endif
