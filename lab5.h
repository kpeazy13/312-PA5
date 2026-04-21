#ifndef _lab5_h
#define _lab5_h 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Nodes
typedef struct Node {
    char *text; // Points to an array of chars (string)
    struct Node *yes; // Points to a single Node
    struct Node *no; // Points to a single Node
    int isQuestion; // 1 = question, 0 = animal
} Node;

Node *create_question_node(const char *text);
Node *create_animal_node(const char *text);
void free_tree(Node *node);
void free_node(Node *node);
int count_nodes(Node *node);

// EntryInfo, Entry, and Hash
typedef struct {
    int timesGuessed; // How many times the node was guessed
    int timesCorrect; // How many times the node was guseed correctly
} EntryInfo;

typedef struct Entry {
    char *key; // An array of chars (string)
    EntryInfo info; // Extra information about Entry
    struct Entry *next; // Pointer to next associated Entry
} Entry;

typedef struct Hash {
    Entry **buckets; // An array of Entry Chains
    int size; // Number of Entrys
    int nbuckets; // Number of buckets (Entry*) allocated for above array
} Hash;

char *canonicalize(const char *text);
void h_init(Hash *h, int num_buckets);
int h_contains(const Hash *h, const char *key);
int h_put(Hash *h, const char *key, EntryInfo info); 
Entry *h_get_entry(Hash *h, const char* key);
int h_remove(Hash *h, const char *key);
void  h_free(Hash *h);

// Edit and EditStack
typedef struct Edit {
    Node *oldQuestion; // Old Question (parent) Node
    Node* newQuestion; // New Question (parent) Node
    Node *oldAnimal; // Old Animal (child) Node
    Node *newAnimal; // New Animal (child) Node
    EntryInfo newAnimalInfo; // Info about the new Animal Node
    int wasYesChild; // (1) if OldAnimal was yes child, (0) if no child, (-1) if root
} Edit;

typedef struct EditStack {
    Edit *edits; // Points to an array of Edits
    int size; // How many Edits are currently in the above array
    int capacity; // How many Edits are currently allocated for the above array
} EditStack;

void es_init(EditStack *stack);
void es_push(EditStack *stack, Edit edit);
Edit es_pop(EditStack *stack);
int  es_is_emtpy(EditStack *stack);
void es_redo_clear(EditStack *stack, Hash *hash);
void es_free(EditStack *stack);

// QueueNode and Queue: Implemented for You!
typedef struct QueueNode {
    Node *node; // Points to a single Node
    int id; // Unique Identifier for Node
    struct QueueNode *next; // Points to the next QueueNode
} QueueNode;

typedef struct Queue {
    QueueNode *front; // Pointer to QueueNode at the front of the queue
    QueueNode *end; // Pointer to QueueNode at the end of the queue
    int size; // How big the queue currently is
} Queue;

void q_init(Queue *q);
void q_enqueue(Queue *q, Node *node, int id);
int  q_dequeue(Queue *q, Node **node, int *id);
int  q_is_empty(Queue *q);
void q_free(Queue *q);

// Functions from game.c
void play_game();
void learnNewAnimal(Node* wrongAnimal, Node* parentQuestion, int parentAnswer);
int undo_last_edit();
int redo_last_edit();

// Function from utils.c
int check_integrity();
int check_animal();

// Functions from persist.c
int save_tree(const char *filename);
int load_tree(const char *filename);

// Functions from visualize.c
void draw_tree();

// Functions from main.c
char *get_input(int y, int x, const char *prompt);
int get_yes_no(int y, int x, const char *prompt);
/** THESE ARE YOUR HELPER FUNCTIONS FOR NCURSES **/
void display_header_play_game();
void display_message(int y, int x, const char *prompt);
void display_bold_message(int y, int x, const char *prompt);
void wait_for_any_input();
void display_stats_message(int y, int x, Entry* entry);
void clear_display();

// Global Variables from main.c
extern Node *g_root;
extern EditStack g_undo;
extern EditStack g_redo;
extern Hash g_index;

// Function from tests.c
int test_main();

#endif