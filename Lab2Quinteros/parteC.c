#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h>
#include "header.h"

// Busca el comando de redireccion de entrada
int redEntrada (char* argV[], char fileIn[])  //argV    Contiene los comandos ingresados
{										     //fileIn  Arreglo donde se copia el nombre del archivo de redireccion de entrada
	int i = 0;
	while (argV[i] != NULL)
	{
		if (!strcmp (argV[i], "<")) 
		{
			strcpy (fileIn, argV[i + 1]);
			return 1;		//  Retorna 1 si encuentra el comando, sino 0.
		}
		i++;
	}
	return 0;
}

// Busca el comando de redireccion de salida
int redSalida (char* argv[], char fileOut[])    //argV    Contiene los comandos ingresados
{												//fileIn  Arreglo donde se copia el archivo de redireccion de salida
	int i = 0;
	while (argv[i] != NULL) 
	{
		if (!strcmp (argv[i], ">")) 
		{
			strcpy (fileOut, argv[i + 1]);
			return 2;		//  Retorna 2 si encuentra el comando, sino 0.
		}
		i++;
	}
	return 0;
}

// Reemplaza los comandos de redireccion (<,>) por "NULL" en el arreglo argV.
void obtenerCmdRedir (int entrada, int salida, char* argV[])  //entrada  Indica si hay redireccion de entrada
{															//salida   Indica si hay redireccion de salida
	if (entrada == 1) 									//argV     Contiene los comandos ingresados.
	{	
		for (int i = 0; argV[i] != NULL; i++) 
		{
			if (!strcmp(argV[i], "<")) 
			{
				argV[i] = NULL;
				break;
			}
		}
	}
	else if (entrada == 0 && salida == 2) 
	{
		for (int i = 0; argV[i] != NULL; i++) 
		{
			if (!strcmp(argV[i], ">")) 
			{
				argV[i] = NULL;
				break;
			}
		}
	}
}

 // Verifica si debe realizarse un pipeline.
int pipes(char* argv[], char* argv1[], char* argv2[])	// argv Argumentos del comando ingresado por el usuario
{														// argv1 Array donde se guardarán los argumentos del comando 1
	int indexArg, aux, indexArg2;						// argv2 Array donde se guardarán los argumentos del comando 2
	
	for(indexArg = 0; argv[indexArg] != NULL; indexArg++)	// Recorro argv hasta encontrar el pipe
	{
		aux = strcmp("|", argv[indexArg]);	
		if(aux == 0)
			break;
		argv1[indexArg] = (char*) malloc (strlen(argv[indexArg] + 1));   // Aloco memoria para guardar argv1
		strcpy(argv1[indexArg], argv[indexArg]);	// Copio en argv1 los argumentos a la izquierda del pipe
	}
	argv1[indexArg] = '\0';  // Pongo un null en argv1 en el lugar que le correspone al pipe
	
	if(argv[indexArg] == NULL)	//Si no encontró |, se devuelve un 0, si no, se obtienen los dos argv de los comandos.
		return 0;
	
	indexArg++;
	
	for(indexArg2 = 0; argv[indexArg] != NULL; indexArg2++)		// Recorro argv hasta que termine
	{	
		if(argv[indexArg] == NULL)	// Si llegó al final de argv termino
			break;
		argv2[indexArg2] = (char*) malloc (strlen(argv[indexArg] + 1));	  // Aloco memoria para guardar argv2
		strcpy(argv2[indexArg2], argv[indexArg]);	// Copio en argv2 los argumentos a la derecha del pipe
		indexArg++;
	}
	argv2[indexArg2] = '\0';
	
	return 1;  // Devuelve 1 si se debe ejecutar el pipeline
}

// Ejecuta el Pipeline
void execPipe(char* argv1[], char* argv2[], char* paths[])	// argv1 Argumentos del comando 1
{															// argv2 Argumentos del comando 2
	char archivoCmd[256];									// paths Paths donde puede buscar los archivos que ejecutan los comandos
	int fd[2];

	if (pipe(fd) < 0)  // Crea un pipe
	{
		perror("Error - Pipe creation");
		exit(1);
	} 

	if (fork() == 0)   // Si se trata del proceso hijo
	{ 
		close(fd[0]);	// Cierra el STDIN del file descriptor
		dup2(fd[1],1);  // Redirección de STDOUT al pipe.   
		close(fd[1]);	// Cierra el STDOUT del file descriptor
		buscarCmd(argv1[0], paths, archivoCmd);		// Busca el comando (izquierda del pipe) en la variable de entorno 
		execv(archivoCmd, argv1);
		perror(archivoCmd);
		exit(1);
	} 
	else 	// Para el proceso padre
	{					
		close(fd[1]); 	// Cierra el STDOUT del file descriptor
		dup2(fd[0],0);	// Redirecciona el file descriptor, desde el pipe a STDIN.
		close(fd[0]);	// Cierra el STDIN del file descriptor
		buscarCmd(argv2[0], paths, archivoCmd);		// Busca el comando (derecha del pipe) en la variable de entorno 
		execv(archivoCmd, argv2);
		perror(archivoCmd);
		exit(1);
	}
}

