#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "lab5.h"

void test_hash() {
    printf("Testing Hash Table...\n");
    
    Hash h;
    h_init(&h, 7);
    
    assert(h.size == 0);
    
    // Create sample info for animals
    EntryInfo dogInfo = {5, 4}; // Guessed 5 times, correct 4 times
    EntryInfo catInfo = {2, 2};
    
    // Test basic insertion
    assert(h_put(&h, "dog", dogInfo) == 1);
    assert(h_put(&h, "cat", catInfo) == 1);
    
    // Test duplicate key prevention (should return 0)
    assert(h_put(&h, "dog", dogInfo) == 0); 
    
    assert(h.size == 2);
    assert(h_contains(&h, "dog"));
    assert(h_contains(&h, "cat"));
    assert(!h_contains(&h, "bird"));
    
    // Test Retrieval and Info verification
    Entry *e = h_get_entry(&h, "dog");
    assert(e != NULL);
    assert(strcmp(e->key, "dog") == 0);
    assert(e->info.timesGuessed == 5);
    assert(e->info.timesCorrect == 4);

    // Test collisions
    for (int i = 0; i < 20; i++) {
        char key[20];
        sprintf(key, "animal_%d", i);
        EntryInfo temp = {0, 0};
        h_put(&h, key, temp);
    }
    assert(h.size == 22);
    
    // Test removal
    assert(h_remove(&h, "dog") == 1);
    assert(h.size == 21);
    assert(!h_contains(&h, "dog"));
    
    h_free(&h);
    printf("Hash table tests passed!\n");
}

void test_integrity() {
    printf("Testing Integrity Checker...\n");
    
    /* Valid tree */
    Node *root = create_question_node("Q1");
    root->yes = create_animal_node("A1");
    root->no = create_animal_node("A2");
    
    Node *saved = g_root;
    g_root = root;
    
    assert(check_integrity());
    
    /* Invalid tree - question with one child */
    Node* save_animal = root->no;
    root->no = NULL;
    assert(!check_integrity());
    
    /* Restore */
    root->no = save_animal;
    assert(check_integrity());
    
    free_tree(g_root);
    g_root = saved;
    
    printf("Integrity tests passed\n");
}

void test_canonicalize() {
    printf("Testing Canonicalization...\n");
    
    char *c1 = canonicalize("Does it meow?");
    assert(strcmp(c1, "does_it_meow") == 0);
    free(c1);
    
    char *c2 = canonicalize("Is It BIG??");
    assert(strcmp(c2, "is_it_big") == 0);
    free(c2);
    
    char *c3 = canonicalize("ABC123");
    assert(strcmp(c3, "abc123") == 0);
    free(c3);
    
    printf("Canonicalization tests passed\n");
}

void test_edit_stack() {
    printf("Testing Edit Stack...\n");
    
    // Initialize Stack and a dummy Hash table for es_redo_clear to use
    EditStack s;
    es_init(&s);
    Hash dummy_hash;
    h_init(&dummy_hash, 10);
    
    assert(es_is_emtpy(&s));
    assert(s.size == 0);
    
    /* Create dummy nodes for testing */
    Node *oldQ1 = create_question_node("OldQuestion1?");
    Node *newQ1 = create_question_node("NewQuestion1?");
    Node *oldA1 = create_animal_node("OldAnimal1");
    Node *newA1 = create_animal_node("NewAnimal1");
    
    Node *oldQ2 = create_question_node("OldQuestion2?");
    Node *newQ2 = create_question_node("NewQuestion2?");
    Node *oldA2 = create_animal_node("OldAnimal2");
    Node *newA2 = create_animal_node("NewAnimal2");
    
    // Create edits
    Edit e1 = {oldQ1, newQ1, oldA1, newA1, {0,0}, -1};
    Edit e2 = {oldQ2, newQ2, oldA2, newA2, {0,0}, 1};
    
    // Test push
    es_push(&s, e1);
    assert(s.size == 1);
    
    es_push(&s, e2);
    assert(s.size == 2);
    
    // Test pop (LIFO order)
    Edit popped = es_pop(&s);
    assert(popped.oldQuestion == oldQ2);
    assert(s.size == 1);
    
    // Test redo clear
    es_redo_clear(&s, &dummy_hash); 
    assert(s.size == 0);
    assert(es_is_emtpy(&s));
    
    // Clean up Stack and Hash
    es_free(&s);
    h_free(&dummy_hash);
    
    // Clean Up
    free_node(popped.newQuestion);
    free_node(popped.newAnimal);
    free_node(oldQ1);
    free_node(oldA1);
    free_node(oldQ2);
    free_node(oldA2);
    
    printf("Edit stack tests passed\n");
}

int test_main() {
    printf("\n=== Running Unit Tests ===\n\n");
    
    test_hash();
    test_canonicalize();
    test_integrity();
    test_edit_stack();
    
    printf("\n=== All Tests Passed! ===\n\n");
    return 0;
}
