#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "headers.h"

int main()
{
    printf("Bienvenido a la prueba del programa. Suma de enteros ingresados.\n");

    int n, i, *ptr, suma = 0;
    printf("Ingresar cantidad de numeros: ");
    scanf("%d", &n);
    ptr = (int*) malloc2(n * sizeof(int));
    if(ptr == NULL)
    {
        printf("Error! Memoria no alocada.");
        return -1;
    }
    printf("Ingresa los numeros de a uno, presionando enter: \n");
    for(i = 0; i < n; i++)
    {
        scanf("%d", ptr + i);
        suma += *(ptr + i);
    }
    printf("Suma = %d \n", suma);
    free2(ptr);
    return 0;
}