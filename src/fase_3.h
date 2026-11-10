#ifndef FASE_3_H
#define FASE_3_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estructura del nodo para comandos
typedef struct commandNode {
    int ID;
    char action[50];
    struct commandNode* next;
} commandNode, comandNode;

// Estructura de la Cola (FIFO)
typedef struct queue {
    comandNode* front;
    comandNode* rear;
} queue;

// Estructura de la Pila (LIFO)
typedef struct stack {
    comandNode* top;
} stack;

// Prototipos de funciones para la gestión de Cola y Pila
void init_Queue(queue* q);
void init_Stack(stack* s);
void enqueue(queue* q, int ID, const char* action);
void push(stack* s, int ID, const char* action);
void execute_command(queue* q, stack* s);
void undo_command(stack* s);
void free_queue(queue* q);
void free_stack(stack* s);

#endif // FASE_3_H