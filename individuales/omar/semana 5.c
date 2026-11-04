#include <stdio.h>

int main() {
    // 1. Declarar una variable y un puntero
    int numero = 42;
    int *p = &numero; // 'p' almacena la dirección de memoria de 'numero'

    printf("=== 1. Estado inicial ===\n");
    printf("Valor de 'numero': %d\n", numero);
    printf("Direccion de 'numero': %p\n", (void*)&numero);
    printf("Valor via puntero (*p): %d\n\n", *p);

    // 2. Modificar la variable usando *p y comprobar el cambio
    *p = 99; // Se modifica el valor en la memoria a la que apunta 'p'

    printf("=== 2. Tras modificar mediante *p = 99 ===\n");
    printf("Nuevo valor de 'numero': %d\n", numero);
    printf("Nuevo valor vía puntero (*p): %d\n\n", *p);

    // 3. Verificar un puntero NULL antes de usarlo
    int *ptr_nulo = NULL;

    printf("=== 3. Comprobacion de puntero NULL ===\n");
    if (ptr_nulo != NULL) {
        printf("El puntero apunta a: %d\n", *ptr_nulo);
    } else {
        printf("El puntero es NULL. Se evito un error de acceso a memoria (Segmentation Fault).\n");
    }

    return 0;
}
