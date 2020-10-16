#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include "headers.h"

#define alinear4(x) (((((x) -1) >> 2) << 2) + 4) // Obtengo el múltiplo de 4 igual o inmediatamente superior

void * base = NULL; // Se inicializa la base del heap en NULL

// Divide el bloque de acuerdo al tamaño requerido. El bloque b debe existir
void dividirBloque (t_block b, size_t s)
{
    t_block new;
    new = (t_block)(b -> data + s); // Como el campo data es de tipo char[], nos aseguramos que la suma se haga de a bytes.
    new -> size = b -> size - s - TAMANO_BLOQUE;
    new -> next = b -> next;
    new -> prev = b;
    new -> free = 1;
    new -> ptr = new -> data;
    b -> size = s;
    b -> next = new;

    if (new -> next)
        new -> next -> prev = new;
}

// Agrega un nuevo bloque al final del heap, retorna NULL en caso de error
t_block extenderHeap ( t_block last , size_t s)
{
    t_block b;
    b = sbrk (0);

    if (sbrk(TAMANO_BLOQUE + s) == (void *) -1)
        return (NULL);    
    
    b -> size = s;
    b -> next = NULL;
    b -> prev = last;
    b -> ptr = b -> data;

    if (last)
        last -> next = b;

    b -> free = 0;
    return(b);
}

// Encuentra el primer bloque libre que cumpla con el tamaño requerido, por algoritmo First Fit
t_block encontrarBloque (t_block *last , size_t size)
{
    t_block b = base;
    while (b && !(b -> free && b -> size >= size)) 
    {
        * last = b;
        b = b -> next;
    }
    return (b);
}

// Obtiene un bloque desde una dirección
t_block obtenerBloque (void *p)
{
    char * tmp;
    tmp = p;
    return (p = tmp -= TAMANO_BLOQUE);
}

// Valida una dirección para liberar
int validarDireccion (void *p)
{
    if (base)
    {
        if ( p > base && p < sbrk(0))
        {
            return (p == (obtenerBloque(p)) -> ptr);
        }
    }
    return (0);
}

t_block fusion(t_block b)
{
    if (b -> next && b -> next -> free)
    {
        b -> size += TAMANO_BLOQUE + b -> next -> size;
        b -> next = b -> next -> next;

        if (b -> next)
            b -> next -> prev = b;
    }
    return (b);
}

void * malloc2(size_t size)
{
    t_block b, last;
    size_t s;
    s = alinear4(size);
    if (base) 
    {                   // Primero encuentra un bloque
        last = base;
        b = encontrarBloque(&last, s);

        if (b) 
        {
            if ((b -> size - s) >= (TAMANO_BLOQUE + 4))  // Si se puede, divido el bloque
                dividirBloque(b, s);
            b -> free = 0;
        } 
        else 
        {   // No hay bloque donde quepa, por lo tanto extiendo el heap
            b = extenderHeap(last, s);

            if (!b)
                return (NULL); // Retorna NULL en caso de error
        }
    } 
    else 
    {   // Se ingresa por primera vez
        b = extenderHeap(NULL, s);

        if (!b)
            return (NULL);  // Retorna NULL en caso de error

        base = b;
    }
    return (b -> data);
}

void free2(void * p)
{
    t_block b;
    if (validarDireccion(p))
    {
        b = obtenerBloque(p);
        b -> free = 1;
       
        if (b -> prev && b -> prev -> free)   // De ser posible, se fusiona con el bloque previo
            b = fusion(b -> prev);

        if (b -> next)  // Si hay bloque siguiente, trato de fusionar
            fusion(b);
        else
        {
            if (b -> prev) 
                b -> prev -> next = NULL;   // Libero el final del heap
            else
                base = NULL; // No hay más bloques, vuelvo al estado inicial

            brk(b); // Muevo el break a la posición del bloque liberado
        }
    } 
}
