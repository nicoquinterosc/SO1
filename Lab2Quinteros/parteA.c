#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include "header.h"


void parsePath (char* p[]) 
{
	char* var = getenv("PATH");  //var = "/home/nicoq/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr....."

	p[0] = strtok(var, ":");    //Corta el string en ":", para obtener el path

	int i = 1;
	while (p[i-1] != NULL) 
	{
		p[i] = strtok (NULL, ":");  // Continua recorriendo el mismo string
		i++;
	}
}

void prompt () 		//Texto inicial de la línea de comandos
{			
	char hostname[20];
	struct passwd * user;   //passwd : estructura que contiene nombre de usuario, contraseña,
	char* dir;				// id, directorio home, etc

	gethostname(hostname, 20);  //Obtiene el hostname
	user = getpwuid(getuid());	//Obtiene información del usuario a partir del ID
	dir = getcwd(NULL, 300);	//Obtiene el directorio actual

	printf("%s@%s:%s$ ", user->pw_name, hostname, dir);
}

int cuentaCmd(char* argv[], char* input) //Cuenta la cantidad de palabras pasadas
{		
	argv[0] = strtok(input, " \n");		//split en espacio y salto de línea
									  // argv guarda todos los tokens de input separados por espacio y salto de línea
	int i = 1;
	while (argv[i-1] != NULL) 
	{
		argv[i] = strtok (NULL, " \n");
		i++;
	}
	return --i;
}

// Busco el comando argv en la variable path, guardo el resultado en archivoCmd
void buscarCmd(char* argv, char* path[], char* archivoCmd) 
{
	int result;
	char dirActual[300];
	char* archivo;

	if (argv[0] == '/' || (argv[0] == '.' && argv[1] == '.' && argv[2] == '/'))   //   	/ ó ../
	{
		char* dir;
		char* nextDir;
		if (argv[0] == '/')
			strcpy (dirActual, "/");	// Si el comando empieza con "/" inicio la variable dirActual con "/"
		dir = strtok(argv, "/");  // Split en "/" y guardo el primer comando en dir
		nextDir = strtok(NULL, "/");   // Guardo el string siguiente a continuación de la barra

		strcat(dirActual, dir);		// Concateno el comando a la variable dirActual

		while (nextDir != NULL) 	// Recorro hasta el fin del comando
		{
			dir = nextDir;		// Guardo nextDir en la variable auxiliar
			nextDir = strtok(NULL, "/");	// Obtengo el comando siguiente a la barra
			strcat(dirActual, "/"); 	//Agrega "/" al final del path
			if (nextDir != NULL)
				strcat(dirActual, dir);
		}
		archivo = dir;   // El comando después de la última barra es el archivo
	}
	else if (argv[0] == '.' && argv[1] == '/')  //  ./
	{
		if(getcwd(dirActual, 300) == NULL)		// Guarda en dirActual el path donde me encuentro
		{
			printf("ERROR con getcwd().\n");
			return;
		}

		strcat(dirActual, "/");			// Agrega "/" al final del path
		archivo = strtok(argv, "/");	// Split en "/" y guardo el primer token
		archivo = strtok(NULL, "/");	// Guardo el segundo token (nombre de archivo)
	}
	else 
	{				
		char aux[150];
		int i = 0;

		while (path[i] != NULL) 	// Busca el comando en la variable de entorno PATH
		{		
			strcpy(aux, path[i]);
			strcat(aux, "/");
			strcat(aux, argv);

			result = access(aux, F_OK);		// Chequea que el usuario tenga acceso al archivo

			if (!result) 		
			{
				strcpy(archivoCmd, aux);	// Si es así actualizo el archivoCmd y retorno
				return;
			}
			i++;
		}
		archivoCmd[0] = 'E';	// Sino devuelvo error
		return;
	}

	strcat(dirActual, archivo);				// Concatena el nombre del archivo con el directorio
	result = access(dirActual, F_OK);		// Chequea que el usuario tenga acceso al archivo
	if (!result)
		strcpy(archivoCmd, dirActual);			// Si es así actualizo el archivoCmd
	else
		archivoCmd[0] = 'E';	// Sino devuelvo error
}

// Crea un proceso hijo que atiende la petición
void crearProc (char* archivoCmd, char* argv[], int argc, char* paths[]) 
{
	int pid, wait, segundo, entrada, salida, tuberia, pipeFlag = 0;
	char fileIn[30];
	char fileOut[30];
	char* argv1[20];
	char* argv2[20];

	segundo = segPlano (argc, argv);
	entrada = redEntrada (argv, fileIn);
	salida = redSalida (argv, fileOut);
	tuberia = pipes(argv, argv1, argv2);

	pid = fork();
	if (pid < 0) 
	{
		perror ("Error - Child creation");
		exit(1);
	}
	else if (pid == 0)  // Se trata del proceso hijo recién creado
	{	
		if (entrada == 1)
		{
			if(freopen (fileIn, "r", stdin) == NULL) 	//Abro el archivo y redirecciono el contenido a la entrada
			{
				printf("Error en la lectura freopen().\n");
			}
		}

		if (salida == 2)
		{
			if(freopen (fileOut, "w+", stdout) == NULL)  //Abro el archivo para utilizarlo como salida
			{
				printf("Error en la lectura freopen().\n");
			}
		}

		if (tuberia == 1) 
		{
			execPipe(argv1, argv2, paths);
			pipeFlag = 1;
		}
		else 
		{
			if (entrada == 1 || salida == 2)
			{
				obtenerCmdRedir (entrada, salida, argv);
			}
			execv (archivoCmd, argv);
			perror (archivoCmd);
			exit(1);
		}

		if(!pipeFlag)
		{
			execv(archivoCmd, argv);
			perror(archivoCmd);
			exit(1);
		}
	}

	if (segundo == 1)
	{
		waitpid (pid, &wait, WNOHANG);   //El padre no espera, devuelve inmediatamente si ningún hijo cambió de estado
	}
	else{
		waitpid (pid, &wait, 0);  //El padre espera a que el hijo finalice
	}
}