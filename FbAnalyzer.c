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
int compareString(const char *s1, const char *s2); // not case sensitive
int compareName(int id1, int id2);
int compareFriendCount(int id1, int id2);
void genSort(int *accList, int l, int r, int (*compare)(int, int));
void swap(int *accList, int id1, int id2);

/*Search function*/
char* getGender(Gender gender);
int checkAccount(int id, char *name, char *city, char *gender);
void binarySearch(int *accList, int l, int r, char *name, char *city, char *gender);

/*Shortest Path*/
int shortestPath(int s, int t, int *path); // return path length

/*Recommend Friend*/
void RecommendFriend(int id);

/*Debuging*/
void printAccount(int id);
void testPrintVertex();
void testPrintEdge();
int testConnectedGraph(); // testing DataSet
void getMinMaxFriendCount(); // testing DataSet

/*User Interface*/
void clearBuffer();
int getID();
int getMenu();
void menuSortSearch();
void menuShortestPath();
void menuRecommendFriend();

int main(int argc, char *argv[]) {
    graph = createGraph();
    if (argc >= 3) {
        if (readNodeData(argv[1]) == 0) return -1;
        if (readConnectionData(argv[2]) == 0) return -1;
    }
    
    getMinMaxFriendCount();
    testConnectedGraph();
    printf("\nPress enter to continue");
    getchar();

    int menu;
    do {
        menu = getMenu();
        switch (menu) {
        case '1':
            menuSortSearch();
            break;
        case '2':
            menuShortestPath();
            break;
        case '3':
            menuRecommendFriend();
            break;
        default: break;
        }
    } while (menu != '4');

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
    printf("\nDropping Graph...\n");
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
    printf("Done!\n");
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
    printf("Reading Node Data File...\t\t");
    FILE *dataStream = fopen(fileName, "rt");
    if (dataStream == NULL) return 0;

    char currentChar = getc(dataStream);
    while (currentChar != EOF) {
        char name[30], city[30];
        int count = 0, gender;
        currentChar = getc(dataStream);
        while (currentChar != '\"') {
            name[count++] = currentChar;
            if (count > 30 || currentChar == '\n') return 0;
            currentChar = getc(dataStream);
        }
        name[count] = '\0';
        
        if (fscanf(dataStream, "%s%d", city, &gender) != 2) return 0;
        addVertex(++accountCount, name, city, gender);

        currentChar = getc(dataStream);
        while (currentChar != EOF && currentChar != '\"')
            currentChar = getc(dataStream);
    }
    
    printf("%d Nodes\n", accountCount);
    fclose(dataStream);
    return 1;
}

int readConnectionData(char *fileName) {
    printf("Reading Connection Data File...\t\t");
    FILE *dataStream = fopen(fileName, "rt");
    if (dataStream == NULL) return 0;
    int id1, id2, count = 0;
    while (fscanf(dataStream, "%d%d", &id1, &id2) == 2) {
        if (id1 >= id2 || id2 > accountCount) return 0;
        addEdge(id1, id2);
        count++;
    }

    printf("%d Edges\n\n", count);
    fclose(dataStream);
    return 1;
}

/* ------------------------ Sorting Function ------------------------ */

int compareString(const char *s1, const char *s2) { 
	while (*s1 && (toupper(*s1) == toupper(*s2)))
        s1++, s2++;
    return toupper(*s1) - toupper(*s2);
}

int compareName(int id1, int id2) {
    JRB acc1, acc2;
    acc1 = jrb_find_int(graph.vertices, id1);
    acc2 = jrb_find_int(graph.vertices, id2);
    
    Info info1, info2;
    info1 = (Info) jval_v(acc1->val);
    info2 = (Info) jval_v(acc2->val);

    return strcmp(info1->name, info2->name);
}

int compareFriendCount(int id1, int id2) {
    JRB acc1, acc2;
    acc1 = jrb_find_int(graph.vertices, id1);
    acc2 = jrb_find_int(graph.vertices, id2);
    
    Info info1, info2;
    info1 = (Info) jval_v(acc1->val);
    info2 = (Info) jval_v(acc1->val);

    if (info1->friendCount == info2->friendCount) return 0;
    return (info1->friendCount > info2->friendCount) ? 1 : -1; 
}

void swap(int *accList, int id1, int id2) {
    int tmp = accList[id1];
    accList[id1] = accList[id2];
    accList[id2] = tmp;
}

void genSort(int *accList, int l, int r, int (*compare)(int, int)) {
    
}

/* ------------------------ Search Function ------------------------ */

char* getGender(Gender gender) {
    if (gender == Male) return "male";
    else if (gender == Female) return "female";
    else return "other";
}

int checkAccount(int id, char *name, char *city, char *gender) {
    Info tmp = getVertexInfo(id);
    if (compareString(tmp->name, name) != 0) return 0;
    if (strcmp(city, "") && compareString(tmp->city, city) != 0) return 0;
    if (strcmp(gender, "") && compareString(gender, getGender(tmp->gender)) != 0) return 0;
    return 1;
}

void binarySearch(int *accList, int l, int r, char *name, char *city, char *gender) {
    int mid;
    while (l <= r) {
        mid = (l + r) / 2;
        Info tmp = getVertexInfo(accList[mid]);
        if (compareString(tmp->name, name) > 0) r = mid - 1;
        else if (compareString(tmp->name, name) < 0) l = mid + 1;
        else break;
    }
    
    printf("\n%-10s%-30s%-20s%-10s\n", "ID", "NAME", "CITY", "GENDER");
    int i = mid, j = mid-1, flag = 0;
    while (i <= accountCount - 1) {
        if (compareString(getVertexInfo(accList[i])->name, name) != 0) break;
        if (checkAccount(accList[i], name, city, gender) == 1) {
            printAccount(accList[i]);
            flag++;
        }
        i++;
    }
    while (j >= 0) {
        if (compareString(getVertexInfo(accList[j])->name, name) != 0) break;
        if (checkAccount(accList[j], name, city, gender) == 1) {
            printAccount(accList[j]);
            flag++;
        }
        j--;
    }
    if (!flag) printf("NO RESULT FOUND\n");
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

/* ------------------------ Recommend Friend ------------------------ */

void RecommendFriend(int id) {
    JRB tree, node;
    int *count;
    count = (int*) calloc(accountCount + 1, sizeof(int));
    count[id] = -1;
    
    tree = jrb_find_int(graph.edges, id);
    tree = (JRB) jval_v(tree->val);
    jrb_traverse(node, tree) {
        count[jval_i(node->key)] = -1;
    }

    jrb_traverse(node, tree) {
        JRB innerTree, innerNode;
        innerTree = jrb_find_int(graph.edges, jval_i(node->key));
        innerTree = (JRB) jval_v(innerTree->val);
        jrb_traverse(innerNode, innerTree) {
            int tmp = jval_i(innerNode->key);
            if (count[tmp] != -1) count[tmp]++;
        }
    }

    int max = 0;
    for (int i = 1; i <= accountCount; i++) {
        if (max < count[i]) max = count[i];
    }

    printf("\nRecommend friends for user %s (ID-%d)\n", getVertexInfo(id)->name, id);
    printf("%-10s%-30s%s\n", "ID", "NAME", "Mutual Friend");
    for (int i = 1; i <= accountCount; i++) {
        if (count[i] == max) {
            JRB account = jrb_find_int(graph.vertices, i);
            Info info = (Info) jval_v(account->val);
            printf("%-10d%-30s%d%s\n", i, info->name, max, " mutual friends");
        }
    }

    free(count);
}

/* ------------------------ Debuging ------------------------ */

void printAccount(int id){
    Info tmp = getVertexInfo(id);
    char gender[10];
    strcpy(gender, getGender(tmp->gender));
    printf("%-10d%-30s%-20s%-10s\n", id, tmp->name, tmp->city, gender);
}

void testPrintVertex() {
    if (accountCount > 100) return;
    printf("%-10s%-30s%-20s%-10s%-12s\n", "ID", "NAME", "CITY", "GENDER", "Friend Count");
    JRB account;
    jrb_traverse(account, graph.vertices) {
        printf("%-10d", jval_i(account->key));
        Info info = (Info) jval_v(account->val);
        char gender[10];
        strcpy(gender, getGender(info->gender));
        printf("%-30s%-20s%-10s%-12d\n", info->name, info->city, gender, info->friendCount);
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

/* ------------------------ User Interface ------------------------ */

void clearBuffer() {
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

int getID() {
    int id;
    while (1) {
        scanf("%d", &id);
        if (id > 0 && id <= accountCount) break;
        else printf("INVALID ID, try again: ");
    }
    return id;
}

int getMenu() {
    int menu;
    printf("\n---------- Menu ----------\n");
    printf("1. Sort and Search\n");
    printf("2. Shortest Path\n");
    printf("3. Recommend Friend\n");
    printf("4. Exit\n");
    printf("\nSelect function: ");
    while (1) {
        menu = getchar();
        clearBuffer();
        if (menu == '1' || menu == '2' || menu == '3' || menu == '4') break;
        else printf("INVALID INPUT, try again: ");
    } 
    return menu;
}

void menuSortSearch() {
    int menu;
    printf("\n---------- Sort / Search ----------\n");
    printf("1. Sort by name\n");
    printf("2. Sort by number of friend\n");
    printf("\nSelect function: ");
    while (1) {
        menu = getchar();
        clearBuffer();
        if (menu == '1' || menu == '2') break;
        else printf("INVALID INPUT, try again: ");
    }

    int *accList = (int*) malloc(accountCount * sizeof(int));
    for (int i = 0; i < accountCount; i++) accList[i] = i + 1;

    switch (menu) {
    case '1':
        genSort(accList, 0, accountCount - 1, &compareName);
        printf("Sort done!\n");

        char name[30], city[20], gender[10];
        printf("\nSearch name: ");
        gets(name);
        printf("Filter by city (0 if no filter): ");
        scanf("%s", city);
        if (strcmp(city, "0") == 0) city[0] = '\0';
        printf("Filter by gender (0 if no filter): ");
        while (1) {
            scanf("%s", gender);
            if (strcmp(gender, "0") == 0) gender[0] = '\0';
            else if (compareString(gender, "male") == 0);
            else if (compareString(gender, "female") == 0);
            else if (compareString(gender, "other") == 0);
            else {
                printf("INVALID GENDER, try again: ");
                continue;
            }
            break;
        }
        binarySearch(accList, 0, accountCount - 1, name, city, gender);
        break;
    case '2':
        genSort(accList, 0, accountCount - 1, &compareFriendCount);
        printf("Sort done!\n");

        break;
    default: break;
    }

    free(accList);
    clearBuffer();
    printf("\nPress enter to continue");
    getchar();
}

void menuShortestPath() {
    int id1, id2;
    printf("\n---------- Shortest Path ----------\n");
    printf("Input start ID: ");
    id1 = getID();
    printf("Input final ID: ");
    id2 = getID();

    int length, *path;
    path = (int*) malloc(accountCount * sizeof(int));
    length = shortestPath(id1, id2, path);
    printf("\nShortest path from user %s (ID-%d) to user %s (ID-%d)\n", getVertexInfo(id1)->name, id1, getVertexInfo(id2)->name, id2);
    for (int i = 0; i <= length; i++) printf("%d ", path[i]);

    free(path);
    clearBuffer();
    printf("\n\nPress enter to continue");
    getchar();
}

void menuRecommendFriend() {
    int id;
    printf("\n---------- Recommend Friend ----------\n");
    printf("Input ID: ");
    id = getID();
    
    RecommendFriend(id);
    clearBuffer();
    printf("\nPress enter to continue");
    getchar();
}
