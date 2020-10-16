/*
 *	QUINTEROS CASTILLA, NICOLÁS DANIEL - 34351470
 *
 *	SISTEMAS OPERATIVOS I - TRABAJO PRÁCTICO N°2
 *
*/

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pwd.h>
#include "header.h"

int main () 
{
	int argc, i;
	char cmd[300];
	char archivoCmd[300];
	char *path[20];
	char *argv[300];

	parsePath (path);	// Guardo las variables de entorno en path

	while (1) 
	{
		prompt ();   // Texto inicial de la línea de comandos

		if(fgets (cmd, 300, stdin) == NULL)         // Lee el string ingresado hasta un salto de linea y lo almacena en cmd
		{
			printf("ERROR en la lectura de fgets().\n");
			return 0;
		}

		argc = cuentaCmd(argv, cmd);   // Cuenta cantidad de palabras

		if (argc == 0)   // Si no se ingresó nada, vuelvo a imprimir prompt
		{		
			continue;
		}

		else {
			if (!strcmp (cmd, "exit"))			// exit para salir
				return 0;						// strcmp devuelve 0 si hay coincidencia

			else if (!strcmp (cmd, "cd")) 		// cd para cambiar directorio
			{
				if(argv[1] == NULL) 		// Si solo se ingresa el comando cd sin nada mas, me lleva a /home/usr
				{
					char* var = getenv("HOME");
					i = chdir(var);
					if (i != 0)						// Si devuelve 0 se actualiza el path y vuelvo a imprimir prompt
						printf ("El directorio no es correcto.\n");
					continue;
				}
				else if(argv[2] != NULL)
				{
					printf ("El directorio no es correcto.\n");
					continue;
				}
				else
				{
					i = chdir (argv[1]);
					if (i != 0)						// Si devuelve 0 se actualiza el path y vuelvo a imprimir prompt
						printf ("El directorio no es correcto.\n");
					continue;
				}
			}
			else
			{
				buscarCmd (argv[0], path, archivoCmd);     // Si no es exit o cd buscamos el comando
				if (archivoCmd[0] == 'E')		// Error
					printf("Comando no encontrado\n");
				else
					crearProc(archivoCmd, argv, argc, path);	// Crea un proceso hijo que ejecute el comando
			}
			
		}
	}
	return 0;
}