#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lab5.h"

// Global Variables defined in main.c
extern Node *g_root; // Pointer to root Node

int check_integrity() {
    // TODO: Implement this function
    // Use the Queue functions provided to you
    if (g_root == NULL) {
        return 1;
    }
    Queue q;
    q_init(&q);
    q_enqueue(&q, g_root, 0);

    int capacity = 16;
    int count = 0;
    Node **visited = (Node**) malloc(capacity * sizeof(Node *));

    while (!q_is_empty(&q)) {
        Node *curr;
        int id;
        q_dequeue(&q, &curr, &id);

        // cycle / repeated-node check
        for (int i = 0; i < count; i++) {
            if (visited[i] == curr) {
                free(visited);
                q_free(&q);
                return 0;
            }
        }

        if (count >= capacity) {
            capacity *= 2;
            visited = (Node**) realloc(visited, capacity * sizeof(Node *));
        }
        visited[count++] = curr;

        if (curr->isQuestion) {
            // question nodes must have both children
            if (curr->yes == NULL || curr->no == NULL) {
                free(visited);
                q_free(&q);
                return 0;
            }

            q_enqueue(&q, curr->yes, 0);
            q_enqueue(&q, curr->no, 0);
        } else {
            // animal nodes must have no children
            if (curr->yes != NULL || curr->no != NULL) {
                free(visited);
                q_free(&q);
                return 0;
            }
        }
    }

    free(visited);
    q_free(&q);
    return 1;
    return 1;
}

int check_animal() {
    clear_display();
    char animalBuf[256];
    char *tmpAnimal = get_input(0, 2, "What animal do you want to check? ");
    strcpy(animalBuf, tmpAnimal);
    // TODO: Implement this function
    char* key = canonicalize(animalBuf);
    if (!h_contains(&g_index, key)){
        clear_display();
        display_bold_message(0, 4, "That animal is not valid/doesnt exist");
        free(key);
        wait_for_any_input();
        return 0;
    }
    Entry* lookup = h_get_entry(&g_index, key);
    free(key);
    display_stats_message(2, 2, lookup);
    wait_for_any_input();
    return lookup->info.timesGuessed + lookup->info.timesCorrect;
}