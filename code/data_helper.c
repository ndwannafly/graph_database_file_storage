#include "../headers/data_helper.h"
#include "../headers/graph_db.h"
#include <string.h>

int create_string_for_db(graph_db * db, char * s){
    unsigned char Type = R_STRING;
    int length = strlen(s);
    int n = sizeof(int) + sizeof(unsigned char) + 1 + length;
    int result;
    db_fseek(db, 0, SEEK_END);
    result = db_ftell(db);
    db_fwrite(&n, sizeof(n), 1, db);
    db_fwrite(&Type, sizeof(Type), 1, db);
    db_fwrite(s, length + 1, 1, db);
    db_fflush(db);
    return result;
}
