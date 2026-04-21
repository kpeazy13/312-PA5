#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lab5.h"

/* ========== Node Functions ========== */

Node *create_question_node(const char *question) {
    Node* node = (Node*) malloc(sizeof(Node));
    node->text = (char*) malloc((strlen(question) + 1)*sizeof(char));
    strcpy(node->text, question);
    node->isQuestion = 1;
    node->yes = NULL;
    node->no = NULL;
    return node;
}

Node *create_animal_node(const char *animal) {
    Node* node = (Node*) malloc(sizeof(Node));
    node->text = (char*) malloc((strlen(animal) + 1)*sizeof(char));
    strcpy(node->text, animal);
    node->isQuestion = 0;
    node->yes = NULL;
    node->no = NULL;
    return node;
}

void free_tree(Node *node) {
    // TODO: Implement this function
    if (node == NULL){
        return;   
    }
    else{
        free_tree(node->yes);
        free_tree(node->no);
        return free_node(node); 
    }
}

void free_node(Node *node) {
    // TODO: Implement this function
    free(node->text);
    free(node);
}
int count_nodes(Node *root) {
    // TODO: Implement this function
    if (root == NULL){
        return 0;
    }
    else{
        return 1 + count_nodes(root->no) + count_nodes(root->yes);
    }
}

/* ========== EditStack (for undo/redo) ========== */

void es_init(EditStack *stack) {
    // TODO: Implement this function
    stack->capacity = 16;
    stack->size = 0;
    stack->edits = (Edit*) malloc(stack->capacity * sizeof(Edit));
}

void es_push(EditStack *stack, Edit edit) {
    // TODO: Implement this function
    if (stack->size >= stack->capacity){
        stack->capacity *= 2;
        stack->edits = (Edit*) realloc(stack->edits, stack->capacity * sizeof(Edit));
    }
    stack->edits[stack->size] = edit;
    stack->size ++;
}

Edit es_pop(EditStack *stack) {
    Edit dummy = {0};
    // TODO: Implement this function
    stack->size --;
    Edit popped = stack->edits[stack->size];
    stack->edits[stack->size] = dummy;
    return popped;
}

int es_is_emtpy(EditStack *stack) {
    // TODO: Implement this function
    if (stack->size == 0){
        return 1;
    }
    return 0;
}

void es_redo_clear(EditStack *stack, Hash *hash) {
    // TODO: Implement this function
    for (int i = 0; i < stack->size; i++){
        char* key = canonicalize(stack->edits[i].newAnimal->text);
        h_remove(hash, key);
        free_node(stack->edits[i].newAnimal);
        free_node(stack->edits[i].newQuestion);
        free(key);
    }
    stack->size = 0;
}

void es_free(EditStack *stack) {
    // TODO: Implement this function
    free(stack->edits);
}

/* ========== Hash Table ========== */

char *canonicalize(const char *s) {
    // TODO: Implement this function
    char* canon = (char*) malloc((strlen(s) + 1) * sizeof(char));
    strcpy(canon, s);
    for (int i = 0; i < strlen(canon); i++){
        //lowercase letters
        if (canon[i] >= 65 && canon[i] <= 90){   
            canon[i] += 32;
        }
        //replace spaces with underscores, remove non-alphanumeric characters
        if (isspace(canon[i])){
            canon[i] = '_';
        }
        else if (!isalnum(canon[i])){
            for (int j = i; j < strlen(canon); j++){
                canon[j] = canon[j+1];
            }   
            i--;
        }
    }
    return canon;
}

// Djb2 Algorithm to generate a hash value
// http://www.cse.yorku.ca/~oz/hash.html
unsigned h_hash(const char *s) {
    unsigned hash = 5381;
    int c;
    while ((c = *s++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}


void h_init(Hash *h, int nbuckets) {
    // TODO: Implement this function
    h->buckets = (Entry**) calloc(nbuckets, sizeof(Entry*));
    h->nbuckets = nbuckets;
    h->size = 0;
}

int h_contains(const Hash *h, const char *key) {
    unsigned idx = h_hash(key) % h->nbuckets; // index into the h->buckets[]
    // TODO: Implement this function
    Entry* entry = h->buckets[idx];
    while (entry != NULL){
        if (strcmp(entry->key, key) == 0){
            return 1;
        }
        entry = entry->next;
    }
    return 0;
}

int h_put(Hash *h, const char *key, EntryInfo info) {
    unsigned idx = h_hash(key) % h->nbuckets; // index into the h->buckets[]
    // TODO: Implement this function
    if (h_contains(h, key)){
        return 0;
    }
    Entry* newEntry = (Entry*) malloc(sizeof(Entry));
    newEntry->key = (char*) malloc((strlen(key) + 1) * sizeof(char));
    strcpy(newEntry->key, key);
    newEntry->info = info;
    newEntry ->next = h->buckets[idx];
    h->buckets[idx] = newEntry;
    h->size++;
    return 1;
}

Entry *h_get_entry(Hash *h, const char* key) {
    unsigned idx = h_hash(key) % h->nbuckets; // index into the h->buckets[]
    // TODO: Implement this function
    Entry* entry = h->buckets[idx];
    while (entry != NULL){
        if (strcmp(entry->key, key) == 0){
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

int h_remove(Hash *h, const char *key) {
    unsigned idx = h_hash(key) % h->nbuckets;
    // TODO: Implement this function
    Entry* entry = h->buckets[idx];
    Entry* prev = NULL;
    Entry* next = NULL;
    while (entry != NULL){
        if (strcmp(entry->key, key) == 0){
            next = entry->next;
            if (prev == NULL){
                h->buckets[idx] = next;
            }
            else{
                prev->next = next;
            }
            free(entry->key);
            free(entry);
            h->size--;
            return 1;  
        }
        prev = entry;
        entry = entry->next;
    }
    return 0;
}

void h_free(Hash *h){
    // TODO: Implement this function
    for(int i = 0; i < h->nbuckets; i++){
        Entry* entry = h->buckets[i];
        Entry* next;
        while (entry != NULL){
            next = entry->next;
            free(entry->key);
            free(entry);
            entry = next;
        }
    }
    free(h->buckets);
    h->buckets = NULL;
    h->size = 0;
    h->nbuckets = 0;
}

/* ========== Queue (for BFS traversal) ========== */
// All functions are done for you since you did this last lab :)

void q_init(Queue *q) {
    q->front = NULL;
    q->end = NULL;
    q->size = 0;
}

void q_enqueue(Queue *q, Node *node, int id) {
    QueueNode *qn = (QueueNode*)malloc(sizeof(QueueNode));
    qn->node = node;
    qn->id = id;
    qn->next = NULL;
    
    if (q->end == NULL) {
        q->front = q->end = qn;
    } else {
        q->end->next = qn;
        q->end = qn;
    }
    q->size++;
}

int q_dequeue(Queue *q, Node **node, int *id) {
    if (q->front == NULL) return 0;
    
    QueueNode *temp = q->front;
    *node = temp->node;
    *id = temp->id;
    
    q->front = q->front->next;
    if (q->front == NULL) q->end = NULL;
    
    free(temp);
    q->size--;
    return 1;
}

int q_is_empty(Queue *q) {
    return q->size == 0;
}

void q_free(Queue *q) {
    Node *dummy;
    int dummyId;
    while (!q_is_empty(q)) {
        q_dequeue(q, &dummy, &dummyId);
    }
}