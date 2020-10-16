#include <stdlib.h>

// Estructura del bloque, son los meta-datos que contienen información sobre el bloque
struct s_block 
{
    size_t size;
    struct s_block * next;
    struct s_block * prev;
    int free;
    void * ptr;  // Puntero al bloque asignado
    char data [1];
};
typedef struct s_block * t_block;

#define TAMANO_BLOQUE 20 //sizeof (struct s_block) // Tamaño de los meta-datos

void dividirBloque (t_block, size_t);
t_block extenderHeap (t_block, size_t);
t_block encontrarBloque (t_block *, size_t);
t_block obtenerBloque (void *);
int validarDireccion (void *);
t_block fusion (t_block);
void * malloc2 (size_t);
void free2 (void *);