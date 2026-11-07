#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef DEBUG // <- Literalmente esto no sirve de nada, nomas ignorenlo

/* 
 * NOTA DE DEREK:
 * En este archivo, algunas funciones sirven SOLO como metodos auxiliares en ciertas secciones.
 * Todas estas funciones auxiliares tendran nombres mucho mas especificos de lo normal.
 * Ademas, todas estas auxiliares SIEMPRE comenzaran con _ (un guin bajo).
 * POR FAVOR, NO USAR LAS FUNCIONES AUXILIARES DIRECTAMENTE EN EL CODIGO DEL MAIN.
*/

// MACROS
#define MINIMUM_CELSIUS_TEMPERATURE -273.15

// Helper Functions
void Input(const short int InferiorLimit, const short int SuperiorLimit, short int* _Option, const char* Question){
    short int Attempts = 1;
    printf("%s\n", Question);
    do{
        for(int i = InferiorLimit; i <= SuperiorLimit; i++) printf("| %d ", i);
        printf("|  -> ");

        scanf(" %hd", _Option);
        Attempts++;

        if((*_Option < InferiorLimit || *_Option > SuperiorLimit) && Attempts > 3) {
            *_Option = InferiorLimit;
            printf("Tus intentos se han acabado. Se escogio %hd automaticamente.\n", InferiorLimit);
            return;
        }
    }while(*_Option < InferiorLimit || *_Option > SuperiorLimit);
}

void print_menu(void){
    printf("1: Capturar Lectura\n2: Ver Lecturas y Estadisticas\n3: Ver Registro de Eventos y Alertas"
           "\n4: Enviar Comando al Actuador\n5: Ejecutar Comandos Pendientes\n6: Deshaces Ultimo Comando\n0: Salir\n\n");
}

void print_welcoming(void){
    printf("---------------------------------------------------------------\n"
           "                Bienvenido a la Estaicon Vigia!\n"
           "---------------------------------------------------------------\n\n");
}

// Data Structures
typedef struct lecture {
    int ID;
    int time;
    double value;
} lecture;

typedef struct forward_list {
    lecture data;
    char message[50];
    struct forward_list* next;
} forward_list;

typedef struct double_list {
    lecture data;
    char message[50];
    struct double_list* next;
    struct double_list* prev;
} double_list;

// List of Commands
enum {
    SALIR,
    CAPTURAR_LECTURA,
    VER_LECTURAS,
    VER_REGISTRO,
    ENVIAR_COMANDO,
    EJECUTAR_COMANDOS,
    DESHACER_COMANDO
};

enum ACTUADOR { // El actuador simulado es una V
    APAGAR
};

// Methods for forward_list

    // Esta es una funcion auxiliar para agregar elementos a la forward_list
    // NO USAR DIRECTAMENTE EN EL CODIGO POR FAVOR.
void _append_new_forward_list_lecture(lecture* new_lecture, forward_list** used_list, const char* new_message) { 
    // Si la lista esta vacia, hacer esto
    if(*used_list == NULL){
        *used_list = (forward_list*)malloc(sizeof(forward_list));
        if(*used_list != NULL){
            (*used_list)->data = *new_lecture;
            strcpy((*used_list)->message, new_message);
            (*used_list)->next = NULL;
        } else printf("Oops! It has ocurred an error trying to make a new lecture.\n");
        return;
    }

    // Si tiene mas elementos, hacer esto
    forward_list* current = *used_list;
    while(current->next != NULL){
        current = current->next;
    }

    current->next = (forward_list*)malloc(sizeof(forward_list)); // Se asigna la memoria nueva
    if(current->next != NULL){
        current->next->data = *new_lecture; // Se asigna la lectura
        strcpy(current->next->message, new_message);
        current->next->next = NULL;         // Se inicializa el puntero next
    } else printf("Oops! It has ocurred an error trying to make a new lecture.\n");
}

    // Inserta las lecturas del buffer dinamico a la lista enlazada de lecturas
void insert_forward_lectures(forward_list** used_list, lecture* new_lecture, size_t lectures_size){
    if(new_lecture == NULL) {
        printf("Oops! An error has ocurred when registering the lectures.");
        return;
    }
    for(size_t i = 0; i < lectures_size ; i++){
        _append_new_forward_list_lecture((new_lecture+i), used_list, "");
    }
}

    // Inserta las lecturas del buffer dinamico a la lista enlazada de alertas cuando pasan de cierto umbral
void insert_forward_alerts(forward_list** used_list, lecture* new_lecture, size_t lectures_size, double upper_bound, double lower_bound) { 
    if(new_lecture == NULL) {
        printf("Oops! An error has ocurred when registering the lectures.");
        return;
    }
    for(size_t i = 0; i < lectures_size ; i++){
        if(new_lecture[i].value > upper_bound || new_lecture[i].value < lower_bound){
            _append_new_forward_list_lecture((new_lecture+i), used_list, "La temperatura ha sobrepasado los limites!");
        }
    }
}

    // Funcion para leer todas las lecturas de la forward_list
void read_forward_lectures(forward_list** used_list) { 
    if(*used_list == NULL) {
        printf("\nNo hay lecturas registradas!\n\n");
        return;
    }
    double mean = 0, max = (*used_list)->data.value, min = (*used_list)->data.value;
    int n_lectures = 0;
    for(forward_list* current = *used_list; current != NULL; current = current->next){
        printf("\n[%d] [Hora: %d] Lectura: %.2lf", current->data.ID, current->data.time, current->data.value);

        if(current->data.value > max) max = current->data.value;
        if(current->data.value < min) min = current->data.value;

        mean += current->data.value;
        n_lectures++;
    }
    mean /= n_lectures;
    printf("\n\nLectura Maxima: %.2lf | Lectura Minima: %.2lf | Promedio: %.2lf", max, min, mean);
    printf("\n\n");
}

    // Funcion para leer todas las alertas generadas de la forward_list
void read_forward_alerts(forward_list** used_list) { // Funcion para leer todas las lecturas de la forward_list
    if(*used_list == NULL) {
        printf("\nNo hay alertas registradas!\n\n");
        return;
    }
    double mean = 0, max = (*used_list)->data.value, min = (*used_list)->data.value;
    int n_lectures = 0;
    for(forward_list* current = *used_list; current != NULL; current = current->next){
        printf("\n[%d] [Hora: %d] Lectura: %.2lf | %s", 
            current->data.ID, current->data.time, current->data.value, current->message);

        if(current->data.value > max) max = current->data.value;
        if(current->data.value < min) min = current->data.value;

        mean += current->data.value;
        n_lectures++;
    }
    mean /= n_lectures;
    printf("\n\nLectura Maxima: %.2lf | Lectura Minima: %.2lf | Promedio: %.2lf", max, min, mean);
    printf("\n\n");
}

void free_whole_forward(forward_list** used_list) { // Funcion para liberar la memoria de una completa forward_list
    if(*used_list == NULL) return;

    // Declarando variables auxiliares
    forward_list* current_forward = *used_list;
    forward_list* next_forward = current_forward->next;

    // Ciclo principal
    while(current_forward != NULL){
        free(current_forward);
        current_forward = next_forward;
        if(current_forward != NULL){
            next_forward = current_forward->next;
        }
    }
}

//  E S T A C I O N   V I G I A 
int main(void){

    short int Option = 0;
    int ID_gen = 0;
    forward_list* temperature_register = NULL;
    forward_list* registered_alerts = NULL;

    print_welcoming();
    do { // Aqui comienza el ciclo principal

    print_menu();
    Input(SALIR, DESHACER_COMANDO, &Option, "Escoje la operacion que desea realizar: ");
    printf("\n");

    switch(Option){
        case SALIR: break;
        case CAPTURAR_LECTURA:
        {
            // Capturando la lectura
            size_t n_elements_buffer = 1;
            lecture* lecture_buffer = (lecture*)malloc(sizeof(lecture) * n_elements_buffer);
            do{
                double new_value = 0;
                printf("Cual fue la ultima temperatura medida en Celcius? ");
                scanf(" %lf", &new_value);

                // Corrigiendo al lectura por si se introduce una erronea
                if(new_value < MINIMUM_CELSIUS_TEMPERATURE){
                    new_value = MINIMUM_CELSIUS_TEMPERATURE;
                }
                // Generando los identificadores
                int new_time = 12;
                ID_gen++;

                lecture new_lecture = {ID_gen, new_time, new_value};
                lecture_buffer[n_elements_buffer-1] = new_lecture;

                Input(1, 2, &Option, "Desea agregar otro elemento? [ 1: No | 2: Si ]: ");
                if(Option == 1) break; // Si no se agregan mas elementos, simplemente se rompe el ciclo

                // Sino, se agrega un elemento mas con realloc()
                n_elements_buffer++; 
                lecture* aux_buffer = realloc(lecture_buffer, sizeof(lecture) * n_elements_buffer);
                if(aux_buffer == NULL){
                    printf("Oops! It has ocurred an error trying to make a new lecture.\n");
                    break;
                } else lecture_buffer = aux_buffer;
                
            } while(1);

            printf("\n"); // Acomodando la consola
            
            // Creando nueva lectura
            insert_forward_lectures(&temperature_register, lecture_buffer, n_elements_buffer);
            // Generando alertas si la temperatura se sale de los limites
            insert_forward_alerts(&registered_alerts, lecture_buffer, n_elements_buffer, 80, 20);

            if(lecture_buffer != NULL) free(lecture_buffer);

            break;
        }
        case VER_LECTURAS: 
        {
            printf("------------- Temperaturas Registradas en Celcius -------------\n");
            read_forward_lectures(&temperature_register);
            printf("---------------------------------------------------------------\n\n");
            break;
        }
        case VER_REGISTRO: {
            printf("------------------ !!! Alertas Generadas !!! ------------------\n");
            read_forward_alerts(&registered_alerts);
            printf("---------------------------------------------------------------\n\n");
            break;
        }
        case ENVIAR_COMANDO: break;
        case EJECUTAR_COMANDOS: break;
        case DESHACER_COMANDO: break;
        default: break;
    }

    } while(Option != 0); // Aqui termina el ciclo principal

    // Liberando la memoria
    free_whole_forward(&temperature_register);
    free_whole_forward(&registered_alerts);

    return 0;
}

#endif