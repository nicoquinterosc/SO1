/*
 *	QUINTEROS CASTILLA, NICOLÁS DANIEL - 34351470
 *
 *	SISTEMAS OPERATIVOS - TRABAJO PRÁCTICO N°1
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>


void cabecera();
void hostname();
void fecha();
void parteB();
void parteC(int);
void parteD(int, int);
void getValor(char* , char*, char*);
void CPUinfo();
void kernel();
void uptime();
void formatear(float, char*);
void fileSystems();
void tiempoCPU();
void contexto();
void bootTime();
void procesos();
void peticiones();
void memoria();
void promediosCarga();

#define ERROR     "\033[31m"				/* Rojo */
#define RESET   "\033[0m"					/* Negro */
#define BOLD   "\033[1m\033[37m"      /* Blanco resaltado */

int main(int argc, char **argv)
{
	int a, b, c;
	static int flagC = 0, flagD = 0;


  while (1)
    {
	  static struct option long_options[] =
	  {
			  {"help",   no_argument,       0, 'h'},
			  {"parteC", no_argument,       0, 's'},
			  {"parteD", required_argument, 0, 'l'},
			  {0, 0, 0, 0}
	  };

      int option_index = 0;

      c = getopt_long (argc, argv, "hsl:", long_options, &option_index);

      if (c == -1)
        break;

      switch (c)
      {
      	  case 'h':

      		  printf("KSAMP Ayuda:  ------------------------------------------------------------------\n"
      				  "   Las opciones son:\n\n"
      				  BOLD"        -h , --help"RESET"\n"
					  "                    Despliega la ayuda sobre las entradas admitidas en la aplicación.\n\n"
					  BOLD"        -s , --parteC"RESET"\n"
					  "                    Despliega la información solicitada en la parte B y parte C.\n\n"
					  BOLD"        -l a b , --parteD a b"RESET"\n"
					  "                    Despliega la información solicitada en la parte B, parte C y parte D, con 'a' intervalo y 'b' duracion.\n\n");
      		  return 0;

      	  case 's':

      		  if(argc == 2)
      		  {
        		flagC = 1;
        		break;
      		  }
      		  else
      		  {
      			  printf(ERROR"ERROR:"RESET" por favor ingrese una opcion válida.  "BOLD"-h, --help"RESET" para obtener ayuda.\n");
      		  }

      		  return 0;


      	  case 'l':

      		  if(argc == 4)
      		  {
      			  flagC = 1;
      			  flagD = 1;
      			  break;
      		  }
      		  else
      		  {
      			  printf(ERROR"ERROR:"RESET" por favor ingrese una opcion válida.  "BOLD"-h, --help"RESET" para obtener ayuda.\n");
      		  }

      		  return 0;


      	  case '?':

      		  printf(ERROR"ERROR:"RESET" por favor ingrese una opcion válida.  "BOLD"-h, --help"RESET" para obtener ayuda.\n");
      		  return 0;

      	  default:

      		  printf("Por favor ingrese una opcion válida. Las opciones son: -s , -l a b\n");
      		  return 0;
      }
    }

	cabecera();
	parteB();

	if(flagC)
		parteC(flagD);		//Paso el flagD para hacer un salto de pagina cuando solo se pida parteC
	if(flagD)
	{
		if(sscanf(argv[2], "%d", &a) != 1)
		{
			printf(""ERROR"ERROR en la lectura de los argumentos.\n"RESET"");
			return 0;
		}
		if(sscanf(argv[3], "%d", &b) != 1)
		{
			printf(""ERROR"ERROR en la lectura de los argumentos.\n"RESET"");
			return 0;
		}
		parteC(flagD);
		parteD(a, b);
	}

	return 0;
}


void cabecera()
{
	printf("\n");
	hostname();			//Nombre del usuario
	fecha();			//Fecha actual
	return;
}

void hostname()
{
	char nombre[30];
	FILE *archivo;
	archivo = fopen("/proc/sys/kernel/hostname", "r"); 	   //Leo el archivo hostname
	if(fscanf(archivo, "%[^\n]s", nombre) != 1)              //Hasta que encuentre un salto de linea y lo almaceno en "nombre"
	{
		printf(""ERROR"ERROR en la lectura del hostname.\n"RESET"");
		return;
	}

	printf("Hostname: %s \n", nombre);
	fclose(archivo);
	return;
}

void fecha()
{
	time_t t;
	time(&t);
	printf("Fecha: %s \n", ctime(&t));     //Obtengo la hora y fecha del sistema
	return;
}

void parteB()
{
	CPUinfo();		//Tipo y modelo de CPU
	kernel();		//Version del kernel
	uptime();		//Cantidad de tiempo transcurrido desde que se inicio el sistema operativo
	fileSystems();	//Cantidad de filesystems que soporta el kernel
	return;
}

void CPUinfo()
{
	char tipo[50];
	char modelo[100];
	char valor[256];

	getValor("/proc/cpuinfo", valor, "vendor_id");
	sscanf(valor, "vendor_id : %s", tipo);

	getValor("/proc/cpuinfo", valor, "model name");
	sscanf(valor, "model name : %[^\n]s", modelo);

	printf("CPU - Tipo: %s \nModelo: %s \n", tipo, modelo);
	return;
}

void getValor(char* file, char* value, char* key)   //Metodo para obtener un
{													//valor desde un archivo, mediante una palabra clave
	FILE* archivo;
	char aux[500];
	char* esta = NULL;
	archivo = fopen(file,"r");

	while(feof(archivo)==0)
	{
		if(fgets(aux, 500, archivo) == NULL)         //Lee un string hasta un salto de linea y lo almacena en aux
		{
			printf(""ERROR"ERROR en la lectura del archivo.\n"RESET"");
			return;
		}
		esta = strstr(aux, key);              //Suprime el key de la cadena aux
		if(esta!=NULL)
			break;

	}

	fclose(archivo);
	strcpy(value,esta);
	return;

}

void kernel()
{
	FILE* archivo;
	char version[50];
	archivo = fopen("/proc/version","r");
	if(fscanf(archivo, "%[^(]s", version) != 1)
	{
		printf(""ERROR"ERROR en la lectura de la versión del kernel.\n"RESET"");
		return;
	}
	printf("Kernel: %s \n", version);
	fclose(archivo);
	return;
}

void uptime()
{
	FILE* archivo;
	float seg;
	char tiempo[18];
	archivo = fopen("/proc/uptime", "r");
	if(fscanf(archivo, "%f", &seg) != 1)
	{
		printf(""ERROR"ERROR en la lectura del tiempo de inicio.\n"RESET"");
		return;
	}
	formatear(seg, tiempo);					//Transforma los segundos a un formato dd hh:mm:ss
	printf("Uptime: %s \n", tiempo);
	fclose(archivo);
	return;
}

void formatear(float seg, char* tiempo)
{
	unsigned int dias, horas, min;
	float s;

	dias = (int)(seg / 86400);				//Calculo los dias
	seg = seg - (long)(dias * 86400);		//Guardo los segundos que restan
	horas = (int)(seg / 3600);				//Calculo las horas
	seg = seg - (long)(horas * 3600);		//Guardo los segundos que restan
	min = (int)(seg / 60);					//Calculo los minutos
	seg = seg - (long)(min * 60);			//Guardo los segundos que restan
	s = seg;
	if (dias > 0)						//Si hay más de un día lo muestro e imprimo con formato
	{
		sprintf(tiempo, "%2uD %1u:%02u:%02.2f", dias, horas, min, s);
	}
	else
	{
		sprintf(tiempo, "%1u:%02u:%02.2f", horas, min, s);
	}

	return;
}

void fileSystems()
{
	FILE* archivo;
	int a, lineas = 0;
	archivo = fopen("/proc/filesystems", "r");

	while ((a = fgetc(archivo)) != EOF)          //Cuento la cantidad de sistemas de archivo en filesystems
	{
		if (a == '\n')
	        lineas++;
	}

	printf("Cantidad de sistemas de archivo soportados por el kernel: %d \n", lineas);
	fclose(archivo);
	return;
}

void parteC(int flagD)
{
	tiempoCPU();      //Tiempo de CPU para usuarios, sistema, idle
	contexto();		  //Cantidad de cambios de contexto
	bootTime();	      //Tiempo de inicio del sistema
	procesos();		  //Cantidad de procesos creados
	if(flagD == 0)
		printf("\n");
	return;
}

void tiempoCPU()
{
	FILE* archivo;
	float usuario, sistema, idle;
	char tUsuario[15], tSistema[15], tIdle[15];
	archivo = fopen("/proc/stat", "r");
	if(fscanf(archivo, "cpu %f %*f %f %f", &usuario, &sistema, &idle) != 3)	//El primero, tercero y cuarto numero corresponden a los tiempos de usuario, sistema y ocioso respectivamente
	{
		printf(""ERROR"ERROR en la lectura del tiempo de CPU.\n"RESET"");
		return;
	}
	formatear(usuario / 100, tUsuario);				//Divido por 100 porque el tiempo esta en centesimas de segundo
	printf("Tiempo de CPU para usuarios: %s \n", tUsuario);
	formatear(sistema / 100, tSistema);
	printf("Tiempo de CPU para sistema: %s \n", tSistema);
	formatear(idle / 100, tIdle);
	printf("Tiempo de CPU ocioso: %s \n", tIdle);
	fclose(archivo);
	return;
}

void contexto()
{
	char valor[256];
	unsigned int cambios;

	getValor("/proc/stat", valor, "ctxt");
	sscanf(valor, "ctxt %u", &cambios);

	printf("Cambios de contexto: %u \n", cambios);
	return;
}

void bootTime()
{
	char valor[256];
	time_t btime;
	unsigned int aux;
	char booted[40];

	getValor("/proc/stat", valor , "btime");
	sscanf(valor, "btime %u", &aux);
	btime = (time_t)aux;
	strftime(booted, sizeof(booted),"%c", localtime(&btime));
	printf("Boot Time: %s \n", booted);
	return;
}

void procesos()
{
	char valor[256];
	int procesos;

	getValor("/proc/stat", valor, "processes");
	sscanf(valor, "processes %d", &procesos);
	printf("Procesos creados: %d\n", procesos);
	return;
}

void parteD(int intervalo, int duracion)
{
	if(intervalo > duracion)
	{
		printf("\n"ERROR"ERROR: "RESET"El intervalo no puede ser mayor a la duración. Ingrese un valor válido.\n");
		return;
	}

	int i;
	i = duracion/intervalo;
	printf("-----------------------------------------------\n");
	while(i > 0){					//Imprimo la información cada (intervalo) segundos, hasta que termine la duracion
		peticiones();				//Peticiones a disco
		memoria();					//Memoria total y disponible
		promediosCarga();			//Promedio de carga en el último minuto
		i--;
		sleep(intervalo);
		printf("-----------------------------------------------\n");
	}
	printf("\n");
	return;
}

void peticiones()
{
	char valor[256];
	unsigned int lecturas, escrituras, pedidos;

	getValor("/proc/diskstats", valor, "sda");
	sscanf(valor, "sda %u", &lecturas);			//El primer valor corresponde a lecturas
	sscanf(valor, "sda %*u %*u %*u %*u %u", &escrituras);	//El quinto valor corresponde a escrituras
	pedidos = escrituras + lecturas;
	printf("Pedidos a disco: %u\n", pedidos);
	return;
}

void memoria()
{
	char valor[256];
	unsigned int total, libre;

	getValor("/proc/meminfo", valor, "MemTotal");
	sscanf(valor, "MemTotal: %u", &total);
	getValor("/proc/meminfo", valor, "MemFree");
	sscanf(valor, "MemFree: %u", &libre);
	printf("Memoria total: %u MB\n", total/1024);
	printf("Memoria disponible: %u MB\n", libre/1024);
	return;
}

void promediosCarga(){
	FILE *archivo;
	float carga;
	archivo = fopen("/proc/loadavg","r");

	if(fscanf(archivo,"%f", &carga) != 1)
	{
		printf(""ERROR"ERROR en la lectura del promedio de carga.\n"RESET"");
		return;
	}

	printf("Promedio de carga en el último minuto: %f\n", carga);
	fclose(archivo);
	return;
}
