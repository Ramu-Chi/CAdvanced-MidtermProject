#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
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
void dropGraph();
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

/*Search function*/
void printAccount(int id, char *city, char *gender);
void binarySearch(int *accList, int l, int r, char *name, char *city, char *gender);

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
    
    // testPrintEdge();
    // printf("\n");
    // testPrintVertex();
    // printf("\n");

    getMinMaxFriendCount();
    testConnectedGraph();

    dropGraph();
    return 0;
}

/* ------------------------ Basic Function ------------------------ */

Graph createGraph() {
    Graph graph;
    graph.edges = make_jrb();
    graph.vertices = make_jrb();
    return graph;
}

void dropGraph() {
    JRB node, tree;
    jrb_traverse(node, graph.vertices) {
        Info info = (Info) jval_v(node->val);
        free(info->name);
        free(info->city);
        free(info);
    }
    jrb_free_tree(graph.vertices);

    jrb_traverse(node, graph.edges) {
        tree = (JRB) jval_v(node->val);
        jrb_free_tree(tree);
    }
    jrb_free_tree(graph.edges);
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
        if (id1 >= id2 || id2 > accountCount) return 0;
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

/* ------------------------ Search Function ------------------------ */
void printAccount(int id, char *city, char *gender){
    Info tmp = getVertexInfo(id);
    printf("%d\n%s\n", id, tmp->name);
    if (city != NULL) printf("%s\n", tmp->city);
    if (gender != NULL) printf("%s\n", tmp->gender);
}


void binarySearch(int *accList, int l, int r, char *name, char *city, char *gender) {
    while (l<=r)
    {
        int mid = (l + r)/2, midtmp = mid;
        Info tmp = getVertexInfo(*(accList+mid));
        if (strcmp(tmp->name, name) == 0)
        {
            if (gender == NULL && city == NULL)
            {
                printAccount(mid, city, gender);
                while (mid < l)
                {
                    tmp = getVertexInfo(*(accList+mid));
                    if (strcmp(tmp->name, name) != 0) break;
                    else printAccount(*(accList+mid), city, gender);
                    mid--;
                }
                while (midtmp > r)
                {
                    tmp = getVertexInfo(*(accList+mid));
                    if (strcmp(tmp->name, name) != 0) break;
                    else printAccount(*(accList+midtmp), city, gender);
                    midtmp++;
                }
            }
            else if (gender != NULL)
            {
                while (mid < l)
                {
                    tmp = getVertexInfo(*(accList+mid));
                    if (strcmp(tmp->name, name) != 0) break;
                    if (strcmp(tmp->gender, gender) == 0) printAccount(*(accList+mid), city, gender);
                    mid--;
                }
                while (midtmp > r)
                {
                    tmp = getVertexInfo(*(accList+mid));
                    if (strcmp(tmp->name, name) != 0) break;
                    if (strcmp(tmp->gender, gender) == 0) printAccount(*(accList+midtmp), city, gender);
                    midtmp++;
                }
            }
            else if (city != NULL)
            {
                while (mid < l)
                {
                    tmp = getVertexInfo(*(accList+mid));
                    if (strcmp(tmp->name, name) != 0) break;
                    if (strcmp(tmp->city, city) == 0) printAccount(*(accList+mid), city, gender);
                    mid--;
                }
                while (midtmp > r)
                {
                    tmp = getVertexInfo(*(accList+mid));
                    if (strcmp(tmp->name, name) != 0) break;
                    if (strcmp(tmp->city, city) == 0) printAccount(*(accList+midtmp), city, gender);
                    midtmp++;
                }
            }
            else
            {
                while (mid < l)
                {
                    tmp = getVertexInfo(*(accList+mid));
                    if (strcmp(tmp->name, name) != 0) break;
                    if (strcmp(tmp->city, city) == 0 && strcmp(tmp->gender, gender) == 0) printAccount(*(accList+mid), city, gender);
                    mid--;
                }
                while (midtmp > r)
                {
                    tmp = getVertexInfo(*(accList+mid));
                    if (strcmp(tmp->name, name) != 0) break;
                    if (strcmp(tmp->city, city) == 0 && strcmp(tmp->gender, gender) == 0) printAccount(*(accList+midtmp), city, gender);
                    midtmp++;
                }
            }
        }
        else if (strcmp(tmp->name, name) > 0) binarySearch(accList, l, r, name, city, gender);
        else binarySearch(accList, mid, r, name, city, gender);
    }
}

/* ------------------------ Shortest Path ------------------------ */

int shortestPath(int s, int t, int *path) {
    if (s > accountCount || t > accountCount || s < 1 || t < 1) {
        if (s > accountCount || s < 1) printf("No node %d\n", s);
        if (t > accountCount || t < 1) printf("No node %d\n", t);
        return 0;
    }
    
    int *distance, *visit, *previous;
    distance = (int*) calloc(accountCount + 1, sizeof(int));
    for (int i = 0; i < accountCount + 1; i++) distance[i] = accountCount + 1;
    visit = (int*) calloc(accountCount + 1, sizeof(int));
    previous = (int*) calloc(accountCount + 1, sizeof(int));

    distance[s] = 0;
    visit[s] = 1;
    previous[s] = -1;

    Dllist ptr, queue, node;
    queue = new_dllist();
    dll_append(queue, new_jval_i(s));

    while (!dll_empty(queue)) {
        node = dll_first(queue);
        int u = jval_i(node->val);
        if (u == t) break;
        dll_delete_node(node);

        JRB account, tree;
        tree = (JRB) jval_v(jrb_find_int(graph.edges, u)->val);
        jrb_traverse(account, tree) {
            int v = jval_i(account->key);
            if (visit[v] == 0) {
                visit[v] = 1;
                dll_append(queue, new_jval_i(v));
            }
            if (distance[v] > distance[u] + 1) {
                distance[v] = distance[u] + 1;
                previous[v] = u;
            }
        }
    }
    free_dllist(queue);

    int length, tmp;
    length = tmp = distance[t];
    while (t != -1) {
        path[tmp--] = t;
        t = previous[t];
    }

    free(distance);
    free(visit);
    free(previous);
    return length;
}

/* ------------------------ Debuging ------------------------ */

void testPrintVertex() {
    if (accountCount > 100) return;
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
    if (accountCount > 100) return;
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

int testConnectedGraph() {
    int *visit = (int*) calloc(accountCount + 1, sizeof(int));
    Dllist queue, node;
    queue = new_dllist();
    dll_append(queue, new_jval_i(1)); // go from ID 1
    int count = accountCount + 1;

    while(!dll_empty(queue)) {
        node = dll_first(queue);
        int value = jval_i(node->val);
        dll_delete_node(node);
        count--;
        JRB ptr, tree;
        tree = jrb_find_int(graph.edges, value);
        tree = (JRB) jval_v(tree->val);
        jrb_traverse(ptr, tree) {
            int tmp = jval_i(ptr->key);
            if (visit[tmp] == 0) {
                visit[tmp] = 1;
                dll_append(queue, new_jval_i(tmp));
            }
        }
    }

    if (count == 0) printf("Connected Graph\n");
    else {
        printf("%d disconnected node:\n", count);
        for (int i = 1; i <= accountCount; i++) {
            if (visit[i] == 0) printf("%d ", i);
        }
        printf("\n");
    }
    
    free(visit);
    free_dllist(queue);
    return count;
}

void getMinMaxFriendCount() {
    JRB account;
    int minFriendCount = accountCount, maxFriendCount = 0;
    jrb_traverse(account, graph.vertices) {
        int tmp = ((Info) jval_v(account->val))->friendCount;
        if (tmp < minFriendCount) minFriendCount = tmp;
        if (tmp > maxFriendCount) maxFriendCount = tmp;
    }
    printf("Min friend count: %d\n", minFriendCount);
    printf("Max friend count: %d\n", maxFriendCount);
}