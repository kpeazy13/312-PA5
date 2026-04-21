//Krish Patel
// 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lab5.h"

// Global Variables defined in main.c
extern Node *g_root; // Pointer to root Node
extern EditStack g_undo; // Stack for undo operations
extern EditStack g_redo; // Stack for redo operations
extern Hash g_index;  // Hash table

void play_game() {
    display_header_play_game();
    display_message(2, 2, "Think of an animal, and I'll try to guess it!");
    display_message(3, 2, "Press any key to start...");
    wait_for_any_input();
    if (g_root == NULL){
        display_message(2, 2, "The animal tree is empty.");
        return;
    }
    Node *currentNode = g_root;
    char prompt[512];
    /* Example of how to use the buffer above
    * Let's say node->text = "Can you use this?"
    * snprintf(prompt, sizeof(prompt), "%s (y/n): ", node->text); // Creates: "Can you use this? (y/n)"
    * int answer = get_yes_no(5, 2, prompt); 
    */

    Node *prevNode = NULL;
    int parent_answer = 0;
    clear_display();
    // TODO: Your implementation here
    while (currentNode != NULL){
        if (currentNode->isQuestion){
            snprintf(prompt, sizeof(prompt), "%s (y/n): ", currentNode->text);
            parent_answer = get_yes_no(2, 2, prompt);
            prevNode = currentNode;
            if (parent_answer){
                currentNode = currentNode->yes;
            }else{
                currentNode = currentNode->no;
            }
        }else{
            snprintf(prompt, sizeof(prompt), "Is it a(n) [%s]? (y/n)", currentNode->text);
            int answer = get_yes_no(2, 2, prompt);
            char* key = canonicalize(currentNode->text);
            Entry* entry = h_get_entry(&g_index, key);
            free(key);
            entry->info.timesGuessed++;
            if (answer){
                entry->info.timesCorrect++;
                snprintf(prompt, sizeof(prompt), "Congratulations!");
                display_message(2, 2, prompt);
                display_stats_message(4, 3, entry);
                wait_for_any_input();
                break;
            }else{
                learnNewAnimal(currentNode, prevNode, parent_answer);
                break;
            }
        }
        clear_display();   
    }
}

void learnNewAnimal(Node* wrongAnimal, Node* parent, int parentAnswer){
    char newAnimalBuf[256];
    char questionBuf[512];
    char qprompt[512];

    clear_display();    
    char *tmpAnimal = get_input(0, 2, "What animal were you thinking of? ");
    strcpy(newAnimalBuf, tmpAnimal);
    // TODO: Your implementation here
    char* key = canonicalize(newAnimalBuf);
    while (h_contains(&g_index, key)){
        clear_display();
        free(key);
        tmpAnimal = get_input(0, 2, "That animal is already in the game, please input a new animal. ");
        strcpy(newAnimalBuf, tmpAnimal);
        key = canonicalize(newAnimalBuf);
    }
    free(key);
    clear_display();
    char* tmpQuestion = get_input(0, 2, "What is a question that distinguishes this animal from my guess? ");
    strcpy(questionBuf, tmpQuestion);
    int tmpAnswer = get_yes_no(2, 2, "What is the answer for your animal (y/n) ");
    Node* newQ = create_question_node(questionBuf);
    Node* newA = create_animal_node(newAnimalBuf);
    if (tmpAnswer){
        newQ->yes = newA;
        newQ->no = wrongAnimal;
    }else{
        newQ->no = newA;
        newQ->yes = wrongAnimal;
    }
    //add animal to hash table
    EntryInfo newAnimalInfo = {0, 0};
    key = canonicalize(newAnimalBuf);
    h_put(&g_index, key, newAnimalInfo);
    free(key);
    Edit newEdit;
    if (parent == NULL){
        newEdit.oldQuestion = NULL;
        newEdit.wasYesChild = -1;
        g_root = newQ;
    }else{
        newEdit.oldQuestion = parent;
        newEdit.wasYesChild = parentAnswer;
        if (parentAnswer){
        parent->yes = newQ;
    }else{
        parent->no = newQ;
    }
    }

    newEdit.oldAnimal = wrongAnimal;
    newEdit.newAnimalInfo = newAnimalInfo;
    newEdit.newAnimal = newA;
    newEdit.newQuestion = newQ;
    es_push(&g_undo, newEdit);
    es_redo_clear(&g_redo, &g_index);
    display_message(4, 2, "Thank you for your input!");
}




int undo_last_edit() {
    // TODO: Implement this function
    if (es_is_emtpy(&g_undo)){
        return 0;
    }
    Edit edit = es_pop(&g_undo);
    if (edit.wasYesChild == -1){
        g_root = edit.oldAnimal;
    }
    else if (edit.wasYesChild == 1){
        edit.oldQuestion->yes = edit.oldAnimal;
    }else{
        edit.oldQuestion->no = edit.oldAnimal;
    }
    char* key = canonicalize(edit.newAnimal->text);
    Entry* entry = h_get_entry(&g_index, key);
    edit.newAnimalInfo = entry->info;
    h_remove(&g_index, key);
    free(key);
    es_push(&g_redo, edit);
    return 1;
}

int redo_last_edit() {
    // TODO: Implement this function
    if (es_is_emtpy(&g_redo)){
        return 0;
    }
    Edit redo = es_pop(&g_redo);
    if (redo.oldQuestion == NULL){
        g_root = redo.newQuestion;
    }
    else if (redo.wasYesChild){
        redo.oldQuestion->yes = redo.newQuestion;
    }else{
        redo.oldQuestion->no = redo.newQuestion;
    }
    char* key = canonicalize(redo.newAnimal->text);
    h_put(&g_index, key, redo.newAnimalInfo);
    free(key);
    es_push(&g_undo, redo);
    return 1;
}
