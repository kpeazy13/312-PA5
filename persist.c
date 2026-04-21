#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "lab5.h"

extern Node *g_root;
extern Hash g_index;

#define MAGIC 0x41544C35  /* "ATL5" */
#define VERSION 1

typedef struct {
    Node *node;
    int id;
} NodeMapping;

int save_tree(const char *filename) {
    if (g_root == NULL) return 0;
    
    FILE *fp = fopen(filename, "wb");
    if (!fp) return 0;
    
    /* BFS to assign IDs */
    Queue q;
    q_init(&q);
    
    NodeMapping *mappings = (NodeMapping*)malloc(1000 * sizeof(NodeMapping));
    int nodeCount = 0;
    int capacity = 1000;
    
    q_enqueue(&q, g_root, 0);
    mappings[0].node = g_root;
    mappings[0].id = 0;
    nodeCount = 1;
    
    /* Build mapping */
    while (!q_is_empty(&q)) {
        Node *node;
        int id;
        q_dequeue(&q, &node, &id);
        
        if (node->yes) {
            if (nodeCount >= capacity) {
                capacity *= 2;
                mappings = (NodeMapping*)realloc(mappings, capacity * sizeof(NodeMapping));
            }
            mappings[nodeCount].node = node->yes;
            mappings[nodeCount].id = nodeCount;
            q_enqueue(&q, node->yes, nodeCount);
            nodeCount++;
        }
        
        if (node->no) {
            if (nodeCount >= capacity) {
                capacity *= 2;
                mappings = (NodeMapping*)realloc(mappings, capacity * sizeof(NodeMapping));
            }
            mappings[nodeCount].node = node->no;
            mappings[nodeCount].id = nodeCount;
            q_enqueue(&q, node->no, nodeCount);
            nodeCount++;
        }
    }
    
    /* Write header */
    uint32_t magic = MAGIC;
    uint32_t version = VERSION;
    uint32_t count = nodeCount;
    
    fwrite(&magic, sizeof(uint32_t), 1, fp);
    fwrite(&version, sizeof(uint32_t), 1, fp);
    fwrite(&count, sizeof(uint32_t), 1, fp);
    
    /* Write nodes in BFS order */
    for (int i = 0; i < nodeCount; i++) {
        Node *node = mappings[i].node;
        
        uint8_t isQuestion = node->isQuestion;
        uint32_t textLen = strlen(node->text);
        
        fwrite(&isQuestion, sizeof(uint8_t), 1, fp);
        fwrite(&textLen, sizeof(uint32_t), 1, fp);
        fwrite(node->text, 1, textLen, fp);
        
        /* Find child IDs */
        int32_t yesId = -1, noId = -1;
        
        if (node->yes) {
            for (int j = 0; j < nodeCount; j++) {
                if (mappings[j].node == node->yes) {
                    yesId = j;
                    break;
                }
            }
        }
        
        if (node->no) {
            for (int j = 0; j < nodeCount; j++) {
                if (mappings[j].node == node->no) {
                    noId = j;
                    break;
                }
            }
        }
        
        fwrite(&yesId, sizeof(int32_t), 1, fp);
        fwrite(&noId, sizeof(int32_t), 1, fp);
    }
    
    fclose(fp);
    free(mappings);
    q_free(&q);
    return 1;
}

int load_tree(const char *filename) {
    h_free(&g_index);
    h_init(&g_index, 31);

    es_redo_clear(&g_redo, &g_index);
    es_free(&g_redo);
    es_free(&g_undo);
    es_init(&g_redo);
    es_init(&g_undo);

    FILE *fp = fopen(filename, "rb");
    if (!fp) return 0;
    
    /* Read header */
    uint32_t magic, version, count;
    
    if (fread(&magic, sizeof(uint32_t), 1, fp) != 1 || magic != MAGIC) {
        fclose(fp);
        return 0;
    }
    
    if (fread(&version, sizeof(uint32_t), 1, fp) != 1) {
        fclose(fp);
        return 0;
    }
    
    if (fread(&count, sizeof(uint32_t), 1, fp) != 1) {
        fclose(fp);
        return 0;
    }
    
    /* Allocate node array */
    Node **nodes = (Node**)calloc(count, sizeof(Node*));
    int32_t *yesIds = (int32_t*)calloc(count, sizeof(int32_t));
    int32_t *noIds = (int32_t*)calloc(count, sizeof(int32_t));
    
    /* Read nodes */
    for (uint32_t i = 0; i < count; i++) {
        uint8_t isQuestion;
        uint32_t textLen;
        
        if (fread(&isQuestion, sizeof(uint8_t), 1, fp) != 1) {
            goto load_error;
        }
        
        if (fread(&textLen, sizeof(uint32_t), 1, fp) != 1) {
            goto load_error;
        }
        
        if (textLen > 10000) goto load_error;  /* Sanity check */
        
        char *text = (char*)malloc(textLen + 1);
        if (fread(text, 1, textLen, fp) != textLen) {
            free(text);
            goto load_error;
        }
        text[textLen] = '\0';
        
        if (fread(&yesIds[i], sizeof(int32_t), 1, fp) != 1) {
            free(text);
            goto load_error;
        }
        
        if (fread(&noIds[i], sizeof(int32_t), 1, fp) != 1) {
            free(text);
            goto load_error;
        }
        
        /* Validate IDs */
        if ((yesIds[i] >= (int32_t)count) || (noIds[i] >= (int32_t)count)) {
            free(text);
            goto load_error;
        }
        
        /* Create node */
        Node *node = (Node*)malloc(sizeof(Node));
        node->text = text;
        node->isQuestion = isQuestion;
        node->yes = NULL;
        node->no = NULL;
        nodes[i] = node;
        if (!isQuestion) {
            char* key = canonicalize(text);
            h_put(&g_index, key, (EntryInfo){0,0});
            free(key);
        }
    }
    
    /* Link nodes using stored IDs */
    for (uint32_t i = 0; i < count; i++) {
        if (yesIds[i] >= 0 && yesIds[i] < (int32_t)count) {
            nodes[i]->yes = nodes[yesIds[i]];
        }
        if (noIds[i] >= 0 && noIds[i] < (int32_t)count) {
            nodes[i]->no = nodes[noIds[i]];
        }
    }
    
    fclose(fp);
    
    /* Replace global root */
    if (g_root) free_tree(g_root);
    g_root = nodes[0];
    
    free(nodes);
    free(yesIds);
    free(noIds);
    
    return 1;
    
load_error:
    for (uint32_t i = 0; i < count; i++) {
        if (nodes[i]) {
            free(nodes[i]->text);
            free(nodes[i]);
        }
    }
    free(nodes);
    free(yesIds);
    free(noIds);
    fclose(fp);
    return 0;
}