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

Graph graph;
int accountCount = 0;
typedef enum {
    Male,
    Female,
    Other
} Gender;

typedef struct info_acc{
    char *name;
    char *city;
    Gender gender;
    int friendCount;
} *Info;

/*Basic graph function*/
Graph createGraph();
void addVertex(int id, char *name, char *city, Gender gender);
Info getVertexInfo(int id);
void addEdge(int id1, int id2);
int hasEdge(int id1, int id2);
int readNodeData(char *fileName); // return 1 if read successfully, 0 otherwise
int readConnectionData(char *fileName); // return 1 if read successfully, 0 otherwise

/*Sorting function*/
int compareName(int id1, int id2);
int compareFriendCount(int id1, int id2);
void genSort(int *accList, int l, int r, int (*compare)(int, int));
void swap(int *accList, int id1, int id2);

/*Shortest Path*/
int shortestPath(int s, int t, int *path); // return path length

/*Debuging*/
void testPrintVertex();
void testPrintEdge();
int testConnectedGraph(); // testing DataSet
void getMinMaxFriendCount(); // testing DataSet

int main(int argc, char *argv[]) {
    graph = createGraph();
    if (argc >= 3) {
        if (readNodeData(argv[1]) == 0) return -1;
        if (readConnectionData(argv[2]) == 0) return -1;
    }

    testPrintEdge();
    printf("\n");
    testPrintVertex();
    printf("\n");

    return 0;
}

/* ------------------------ Basic Function ------------------------ */

Graph createGraph() {
    Graph graph;
    graph.edges = make_jrb();
    graph.vertices = make_jrb();
    return graph;
}

void addVertex(int id, char *name, char *city, Gender gender) {
    JRB account = jrb_find_int(graph.vertices, id);
    if (account == NULL) {
        Info info = (Info) malloc(sizeof(struct info_acc));
        info->name = strdup(name);
        info->city = strdup(city);
        info->gender = gender;
        info->friendCount = 0;

        jrb_insert_int(graph.vertices, id, new_jval_v(info));
    }
    else {
        Info info = (Info) jval_v(account->val);
        free(info->name);
        free(info->city);
        info->name = strdup(name);
        info->city = strdup(city);
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

int readNodeData(char *fileName) {
    FILE *dataStream = fopen(fileName, "rt");
    if (dataStream == NULL) return 0;

    char currentChar = getc(dataStream);
    while (currentChar != EOF) {
        char name[30], city[30];
        int count = 0, gender;
        currentChar = getc(dataStream);
        while (currentChar != '\"') {
            name[count++] = currentChar;
            if (count > 30) return 0;
            currentChar = getc(dataStream);
        }
        name[count] = '\0';
        
        if (fscanf(dataStream, "%s%d", city, &gender) != 2) return 0;
        addVertex(++accountCount, name, city, gender);

        currentChar = getc(dataStream);
        while (currentChar != EOF && currentChar != '\"')
            currentChar = getc(dataStream);
    }
    
    fclose(dataStream);
    return 1;
}

int readConnectionData(char *fileName) {
    FILE *dataStream = fopen(fileName, "rt");
    if (dataStream == NULL) return 0;
    int id1, id2;
    while (fscanf(dataStream, "%d%d", &id1, &id2) == 2) {
        if (id1 >= id2) return 0;
        addEdge(id1, id2);
    }

    fclose(dataStream);
    return 1;
}

/* ------------------------ Sorting Function ------------------------ */

int compareName(int id1, int id2) {
    
}

int compareFriendCount(int id1, int id2) {
    
}

void swap(int *accList, int id1, int id2) {
    
}

void genSort(int *accList, int l, int r, int (*compare)(int, int)) {
    
}

/* ------------------------ Shortest Path ------------------------ */

int shortestPath(int s, int t, int *path) {

}

/* ------------------------ Debuging ------------------------ */

void testPrintVertex() {
    JRB account;
    jrb_traverse(account, graph.vertices) {
        printf("ID: %d\n", jval_i(account->key));
        Info info = (Info) jval_v(account->val);
        char gender[10];
        switch (info->gender) {
        case 0: strcpy(gender, "Male"); break;
        case 1: strcpy(gender, "Female"); break;
        default: strcpy(gender, "Other"); break;
        }
        printf("name: %s\ncity: %s\ngender: %s\nfriend count: %d\n\n", info->name, info->city, gender, info->friendCount);
    }
}

void testPrintEdge() {
    JRB account, tree, friend;
    jrb_traverse(account, graph.edges) {
        printf("%d: ", jval_i(account->key));
        tree = (JRB) jval_v(account->val);
        jrb_traverse(friend, tree) {
            printf("%d ", jval_i(friend->key));
        }
        printf("\n");
    }
}
