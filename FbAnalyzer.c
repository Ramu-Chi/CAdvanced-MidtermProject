#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dllist.h"
#include "jrb.h"
#include "jval.h"

typedef struct {
    JRB edges; // friend connection graph 
    JRB vertices; // to store information of user
} Graph;

typedef enum {
    Male,
    Female,
    Other
} Gender;

typedef struct info_acc{
    char *name;
    Gender gender;
    int friendCount;
} *Info; // will be stored in vertices.val

Graph graph; // graph is GLOBAL !!!
int accountCount = 0;

Graph createGraph();
void addVertex(int id, char *name, Gender gender);
Info getVertexInfo(int id);
void addEdge(int id1, int id2);
int hasEdge(int id1, int id2);
int readDataFile(char *fileName); // return 1 if successfully open & read, 0 otherwise

void test_printVertex() { // ham de test (ve sau xoa di)
    JRB account;
    jrb_traverse(account, graph.vertices) {
        printf("ID: %d\n", jval_i(account->key));
        Info info = (Info) jval_v(account->val);
        printf("name: %s\ngender: %d\nfriend count: %d\n\n", info->name, info->gender, info->friendCount);
    }
}

void test_printEdge() { // ham de test (ve sau xoa di)
    JRB account, tree, friend;
    jrb_traverse(account, graph.edges) {
        printf("%d: ", jval_i(account->key));
        tree = (JRB) jval_v(account->val);
        jrb_traverse(friend, tree) {
            printf("%d ", jval_i(friend->key));
        }
        printf(".\n");
    }
}

int main(int argc, char *argv[]) {
    graph = createGraph();
    if (argc >= 2) {
        if (readDataFile(argv[1]) == 0) return -1;
    }

    return 0;
}

Graph createGraph() {
    Graph graph;
    graph.edges = make_jrb();
    graph.vertices = make_jrb();
    return graph;
}

void addVertex(int id, char *name, Gender gender) {
    JRB account = jrb_find_int(graph.vertices, id);
    if (account == NULL) {
        Info info = (Info) malloc(sizeof(struct info_acc));
        info->name = strdup(name);
        info->gender = gender;
        info->friendCount = 0;

        jrb_insert_int(graph.vertices, id, new_jval_v(info));
    }
    else {
        Info info = (Info) jval_v(account->val);
        free(info->name);
        info->name = strdup(name);
        info->gender = gender;
    }
}

Info getVertexInfo(int id) {
    JRB account = jrb_find_int(graph.vertices, id);
    if (account == NULL) return NULL;
    return (Info) jval_v(account->val);
}

void addEdge(int id1, int id2) {
    JRB account, tree;
    if (!hasEdge(id1, id2)) {
        account = jrb_find_int(graph.edges, id1);
        if (account == NULL) {
            tree = make_jrb();
            jrb_insert_int(graph.edges, id1, new_jval_v(tree));
        } else {
            tree = (JRB) jval_v(account->val);
        }
        jrb_insert_int(tree, id2, new_jval_i(1));

        account = jrb_find_int(graph.edges, id2);
        if (account == NULL) {
            tree = make_jrb();
            jrb_insert_int(graph.edges, id2, new_jval_v(tree));
        } else {
            tree = (JRB) jval_v(account->val);
        }
        jrb_insert_int(tree, id1, new_jval_i(1));

        Info info;
        info = getVertexInfo(id1);
        info->friendCount++;
        info = getVertexInfo(id2);
        info->friendCount++;
    }
}

int hasEdge(int id1, int id2) {
    JRB account, tree;
    account = jrb_find_int(graph.edges, id1);
    if (account == NULL) return 0;

    tree = (JRB) jval_v(account->val);
    if (jrb_find_int(tree, id2) == 0) return 0;
    return 1;
}
