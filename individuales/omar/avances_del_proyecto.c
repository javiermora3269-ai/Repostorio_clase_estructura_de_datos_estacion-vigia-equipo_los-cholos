#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// M A C R O S
#define MINIMUM_CELSIUS_TEMPERATURE -273.15

/* 
 * NOTA DE DEREK:
 * En este archivo, algunas funciones sirven SOLO como metodos auxiliares en ciertas secciones.
 * Todas estas funciones auxiliares tendran nombres mucho mas especificos de lo normal.
 * Ademas, todas estas auxiliares SIEMPRE comenzaran con _ (un guion bajo).
 * POR FAVOR, NO USAR LAS FUNCIONES AUXILIARES DIRECTAMENTE EN EL CODIGO DEL MAIN.
*/

// H E L P E R   F U N C T I O N S
void _input(const short int InferiorLimit, const short int SuperiorLimit, short int *_Option, const char *Question) {
    short int Attempts = 1;
    printf("%s\n", Question);
    do {
        for (int i = InferiorLimit; i <= SuperiorLimit; i++) {
            printf("| %d ", i);
        }
        printf("|  -> ");

        scanf(" %hd", _Option);
        Attempts++;

        if ((*_Option < InferiorLimit || *_Option > SuperiorLimit) && Attempts > 3) {
            *_Option = InferiorLimit;
            printf("Tus intentos se han acabado. Se escogio %hd automaticamente.\n", InferiorLimit);
            return;
        }
    } while (*_Option < InferiorLimit || *_Option > SuperiorLimit);
}

void print_menu(void) {
    printf("1: Capturar Lectura\n2: Ver Lecturas y Estadisticas\n3: Ver Registro de Eventos y Alertas"
           "\n4: Enviar Comando al Actuador\n5: Ejecutar Comandos Pendientes\n6: Deshacer Ultimo Comando\n0: Salir\n\n");
}

void print_welcoming(void) {
    printf("---------------------------------------------------------------\n"
           "                Bienvenido a la Estacion Vigia!\n"
           "---------------------------------------------------------------\n\n");
}

// D A T A   S T R U C T U R E S
typedef struct lecture {
    int ID;
    int time;
    double value;
} lecture;

typedef struct forward_list {
    lecture data;
    char message[50];
    struct forward_list *next;
} forward_list;

typedef struct comandos {
    int ID;
    int value_effect;
    char value_message[50];
} comandos;

typedef struct lista_de_comandos {
    comandos stored_command;
    char message[50];
    struct lista_de_comandos *next;
    struct lista_de_comandos *prev;
} lista_de_comandos;

// List of Menu Commands
enum {
    SALIR,
    CAPTURAR_LECTURA,
    VER_LECTURAS,
    VER_REGISTRO,
    ENVIAR_COMANDO,
    EJECUTAR_COMANDOS,
    DESHACER_COMANDO
};

// List of Fan Commands / Actuator Options
enum {
    VENTILADOR_APAGAR,
    VENTILADOR_BAJO,
    VENTILADOR_MEDIO,
    VENTILADOR_ALTO
};

// M E T H O D S   F O R   F O R W A R D _ L I S T

void _append_new_forward_list_lecture(const lecture *new_lecture, forward_list **used_list, const char *new_message) { 
    if (*used_list == NULL) {
        *used_list = (forward_list*)malloc(sizeof(forward_list));
        if (*used_list != NULL) {
            (*used_list)->data = *new_lecture;
            strncpy((*used_list)->message, new_message, sizeof((*used_list)->message) - 1);
            (*used_list)->message[sizeof((*used_list)->message) - 1] = '\0';
            (*used_list)->next = NULL;
        } else {
            printf("Oops! An error occurred trying to make a new lecture.\n");
        }
        return;
    }

    forward_list *current = *used_list;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = (forward_list*)malloc(sizeof(forward_list));
    if (current->next != NULL) {
        current->next->data = *new_lecture;
        strncpy(current->next->message, new_message, sizeof(current->next->message) - 1);
        current->next->message[sizeof(current->next->message) - 1] = '\0';
        current->next->next = NULL;
    } else {
        printf("Oops! An error occurred trying to make a new lecture.\n");
    }
}

void insert_forward_lectures(forward_list **used_list, const lecture *new_lecture, size_t lectures_size) {
    if (new_lecture == NULL) {
        printf("Oops! An error has occurred when registering the lectures.\n");
        return;
    }
    for (size_t i = 0; i < lectures_size; i++) {
        _append_new_forward_list_lecture(new_lecture + i, used_list, "");
    }
}

void insert_forward_alerts(forward_list **used_list, const lecture *new_lecture, size_t lectures_size, double upper_bound, double lower_bound) { 
    if (new_lecture == NULL) {
        printf("Oops! An error has occurred when registering the lectures.\n");
        return;
    }
    for (size_t i = 0; i < lectures_size; i++) {
        if (new_lecture[i].value > upper_bound || new_lecture[i].value < lower_bound) {
            _append_new_forward_list_lecture(new_lecture + i, used_list, "La temperatura ha sobrepasado los limites!");
        }
    }
}

void read_forward_lectures(forward_list **used_list) { 
    if (*used_list == NULL) {
        printf("\nNo hay lecturas registradas!\n\n");
        return;
    }
    double mean = 0, max = (*used_list)->data.value, min = (*used_list)->data.value;
    int n_lectures = 0;
    for (forward_list *current = *used_list; current != NULL; current = current->next) {
        printf("\n[%d] [Hora: %d] Lectura: %.2lf", current->data.ID, current->data.time, current->data.value);

        if (current->data.value > max) max = current->data.value;
        if (current->data.value < min) min = current->data.value;

        mean += current->data.value;
        n_lectures++;
    }
    mean /= n_lectures;
    printf("\n\nLectura Maxima: %.2lf | Lectura Minima: %.2lf | Promedio: %.2lf", max, min, mean);
    printf("\n\n");
}

void read_forward_alerts(forward_list **used_list) { 
    if (*used_list == NULL) {
        printf("\nNo hay alertas registradas!\n\n");
        return;
    }
    double mean = 0, max = (*used_list)->data.value, min = (*used_list)->data.value;
    int n_lectures = 0;
    for (forward_list *current = *used_list; current != NULL; current = current->next) {
        printf("\n[%d] [Hora: %d] Lectura: %.2lf | %s", 
               current->data.ID, current->data.time, current->data.value, current->message);

        if (current->data.value > max) max = current->data.value;
        if (current->data.value < min) min = current->data.value;

        mean += current->data.value;
        n_lectures++;
    }
    mean /= n_lectures;
    printf("\n\nLectura Maxima: %.2lf | Lectura Minima: %.2lf | Promedio: %.2lf", max, min, mean);
    printf("\n\n");
}

void free_whole_forward(forward_list **used_list) {
    if (*used_list == NULL) return;

    forward_list *current_forward = *used_list;
    forward_list *next_forward = NULL;

    while (current_forward != NULL) {
        next_forward = current_forward->next;
        free(current_forward);
        current_forward = next_forward;
    }
    *used_list = NULL;
}

// M E T H O D S   F O R   L I S T A _ D E _ C O M A N D O S

void _append_new_command(lista_de_comandos **used_list, const comandos *new_cmd, const char *new_message) {
    lista_de_comandos *new_node = (lista_de_comandos*)malloc(sizeof(lista_de_comandos));
    if (new_node == NULL) {
        printf("Oops! Ocurrio un error al registrar el comando.\n");
        return;
    }

    new_node->stored_command = *new_cmd;
    strncpy(new_node->message, new_message, sizeof(new_node->message) - 1);
    new_node->message[sizeof(new_node->message) - 1] = '\0';
    new_node->next = NULL;
    new_node->prev = NULL;

    if (*used_list == NULL) {
        *used_list = new_node;
        return;
    }

    lista_de_comandos *current = *used_list;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = new_node;
    new_node->prev = current;
}

void _pop_last_command(lista_de_comandos **used_list) {
    if (*used_list == NULL) {
        printf("\nNo hay comandos pendientes para deshacer!\n\n");
        return;
    }

    lista_de_comandos *current = *used_list;
    while (current->next != NULL) {
        current = current->next;
    }

    printf("\n[Deshecho] Comando ID: %d | Accion: %s (Codigo Enum: %d)\n\n",
           current->stored_command.ID,
           current->stored_command.value_message,
           current->stored_command.value_effect);

    if (current->prev != NULL) {
        current->prev->next = NULL;
    } else {
        *used_list = NULL;
    }

    free(current);
}

void insert_command(lista_de_comandos **used_list, const comandos *new_cmd) {
    _append_new_command(used_list, new_cmd, "Comando de ventilador pendiente");
}

void execute_commands(lista_de_comandos **used_list) {
    if (*used_list == NULL) {
        printf("\nNo hay comandos pendientes para ejecutar en el ventilador!\n\n");
        return;
    }

    printf("------------- Ejecutando Comandos del Ventilador -------------\n");
    lista_de_comandos *current = *used_list;
    while (current != NULL) {
        lista_de_comandos *temp = current;
        printf("[ID: %d] Aplicando: %s | Nivel Enum: %d | Estado: %s\n",
               current->stored_command.ID,
               current->stored_command.value_message,
               current->stored_command.value_effect,
               current->message);

        current = current->next;
        free(temp);
    }
    *used_list = NULL;
    printf("---------------------------------------------------------------\n\n");
}

void undo_command(lista_de_comandos **used_list) {
    _pop_last_command(used_list);
}

void free_whole_commands(lista_de_comandos **used_list) {
    if (*used_list == NULL) return;

    lista_de_comandos *current = *used_list;
    lista_de_comandos *next_node = NULL;

    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    *used_list = NULL;
}

// E S T A C I O N   V I G I A 
int main(void) {
    short int Option = 0;
    int ID_gen = 0;
    int ID_cmd_gen = 0;

    forward_list *temperature_register = NULL;
    forward_list *registered_alerts = NULL;
    lista_de_comandos *pending_commands = NULL;

    print_welcoming();
    do {
        print_menu();
        _input(SALIR, DESHACER_COMANDO, &Option, "Escoje la operacion que desea realizar: ");
        printf("\n");

        switch (Option) {
            case SALIR: 
                break;
            case CAPTURAR_LECTURA: {
                size_t n_elements_buffer = 1;
                lecture *lecture_buffer = (lecture*)malloc(sizeof(lecture) * n_elements_buffer);
                if (lecture_buffer == NULL) {
                    printf("Oops! An error occurred trying to make a new lecture.\n");
                    break;
                }

                do {
                    double new_value = 0;
                    printf("Cual fue la ultima temperatura medida en Celsius? ");
                    scanf(" %lf", &new_value);

                    if (new_value < MINIMUM_CELSIUS_TEMPERATURE) {
                        new_value = MINIMUM_CELSIUS_TEMPERATURE;
                    }

                    int new_time = 12;
                    ID_gen++;

                    lecture new_lecture = {ID_gen, new_time, new_value};
                    lecture_buffer[n_elements_buffer - 1] = new_lecture;

                    short int sub_option = 0;
                    _input(1, 2, &sub_option, "Desea agregar otro elemento? [ 1: Si | 2: No ]: ");
                    if (sub_option == 2) break;

                    lecture *aux_buffer = (lecture*)realloc(lecture_buffer, sizeof(lecture) * (n_elements_buffer + 1));
                    if (aux_buffer == NULL) {
                        printf("Oops! An error occurred trying to make a new lecture.\n");
                        break;
                    }
                    lecture_buffer = aux_buffer;
                    n_elements_buffer++;

                } while (1);

                printf("\n");
                insert_forward_lectures(&temperature_register, lecture_buffer, n_elements_buffer);
                insert_forward_alerts(&registered_alerts, lecture_buffer, n_elements_buffer, 80, 20);

                if (lecture_buffer != NULL) free(lecture_buffer);
                break;
            }
            case VER_LECTURAS: {
                printf("------------- Temperaturas Registradas en Celsius -------------\n");
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
            case ENVIAR_COMANDO: {
                short int cmd_choice = 0;
                printf("--- Configurar Ventilador ---\n");
                printf("%d: Apagar\n%d: Velocidad Baja\n%d: Velocidad Media\n%d: Velocidad Alta\n",
                       VENTILADOR_APAGAR, VENTILADOR_BAJO, VENTILADOR_MEDIO, VENTILADOR_ALTO);
                _input(VENTILADOR_APAGAR, VENTILADOR_ALTO, &cmd_choice, "Seleccione la accion para el ventilador: ");

                ID_cmd_gen++;
                comandos new_cmd;
                new_cmd.ID = ID_cmd_gen;
                new_cmd.value_effect = cmd_choice;

                switch (cmd_choice) {
                    case VENTILADOR_APAGAR: 
                        strncpy(new_cmd.value_message, "Apagar Ventilador", sizeof(new_cmd.value_message) - 1); 
                        break;
                    case VENTILADOR_BAJO: 
                        strncpy(new_cmd.value_message, "Ventilador (Nivel Bajo)", sizeof(new_cmd.value_message) - 1); 
                        break;
                    case VENTILADOR_MEDIO: 
                        strncpy(new_cmd.value_message, "Ventilador (Nivel Medio)", sizeof(new_cmd.value_message) - 1); 
                        break;
                    case VENTILADOR_ALTO: 
                        strncpy(new_cmd.value_message, "Ventilador (Nivel Alto)", sizeof(new_cmd.value_message) - 1); 
                        break;
                }
                new_cmd.value_message[sizeof(new_cmd.value_message) - 1] = '\0';

                insert_command(&pending_commands, &new_cmd);
                printf("Comando registrado exitosamente en la cola de pendientes.\n\n");
                break;
            }
            case EJECUTAR_COMANDOS: {
                execute_commands(&pending_commands);
                break;
            }
            case DESHACER_COMANDO: {
                undo_command(&pending_commands);
                break;
            }
            default: break;
        }

    } while (Option != SALIR);

    free_whole_forward(&temperature_register);
    free_whole_forward(&registered_alerts);
    free_whole_commands(&pending_commands);

    return 0;
}