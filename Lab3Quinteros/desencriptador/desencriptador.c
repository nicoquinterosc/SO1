#include <linux/kernel.h>
#include <linux/module.h> 
#include <linux/fs.h> /* Carga todas las funciones read, write, open, release */
#include <linux/uaccess.h> /* Para la funcion put_user */

#define SUCCESS 0
#define DEVICE_NAME "desencriptador" /* Nombre del dispositivo */
#define BUF_LEN 80 /* Longitud maxima del mensaje desde el dispositivo */

static int Device_Open = 0; /* Previene multiples accesos al dispositivo */
static char Message[BUF_LEN]; /* Mensaje que mostrara al llamar al dispositivo */
static char *Message_Ptr; /* Por si el tamaño del buffer es mas chico que el mensaje a cargar */
static int majorNumber;

// Se invoca cando se quiere abrir el archivo de dispositivo cat /dev/"mycharfile"
static int device_open (struct inode *inode, struct file *file) 
{
 	if (Device_Open)
 		return -EBUSY;

 	Device_Open++;
 	 	Message_Ptr = Message; // Inicializo el mensaje 

 	try_module_get(THIS_MODULE);  // Se ejecuta antes que el codigo del modulo para cargarlo de forma segura.

 	return SUCCESS;
}

// Se invoca cuando un proceso cierra el achivo de dispositivo.
static int device_release (struct inode *inode, struct file *file) 
{	
 	Device_Open--; // Se decrementa para poder eliminar el módulo y estar listo para la proxima llamada

 	module_put(THIS_MODULE); // Sirve para quitar el modulo cuando termino de usarse 
 	return 0;
}

// Se llama cuando un proceso que ya ha abierto el archivo de dispositivo intenta leerlo.
static ssize_t device_read (struct file *filp, char *buffer, size_t length, loff_t * offset) 
{
 	int bytes_read = 0; // Numero de bytes escritos actualmente en el buffer

 	if (*Message_Ptr == 0) // Indica el final del archivo
 		return 0;

 	while (length && *Message_Ptr) // Pone los datos en el buffer
	{
 		/*
 		* El buffer esta en un segmento de datos de usario, no en el
 		* segmento de kernel, por eso la asignacion no funciona. Para
 		* esto hay que usar put_user para copiar datos de segmento
 		* kernel a segmento de usario
 		*/
 		put_user(*(Message_Ptr++), buffer++);
 		length--;
 		bytes_read++;
 	}
 	
 	return bytes_read;
}

// Se invoca cuando alguien trata de escribir en el archivo de dispositivo, ej: echo "hi" > /dev/"mycharfile"
static ssize_t device_write (struct file *file, const char __user * buffer, size_t length, loff_t * offset) 
{
	int i;

	for (i = 0; i < length && i < BUF_LEN; i++)
		
		get_user(Message[i], buffer + i);  // Esta funcion copia una sola variable simple del espacio del
		  								   // usuario al espacio del kernel. Funcion inversa al put_user.

	for (i = 0; i < length && i < BUF_LEN; i++)	// Desencripta el mensaje cambiando cada caracter por el que se encuentra
		Message[i] = (Message[i]-2);			// 2 posiciones antes (ASCII)

	Message_Ptr = Message;

	return i;
}

// Solo se incluyen las funciones que se van a usar, las otras quedan como NULL.
struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release, /* Tambien llamado close */
};

// Inicializa y registra el modulo cuando es cargado.
int init_module (void) 
{
	//Trata de registrar el dispositivo de caracteres alocando dinámicamente un major_number para el dispositivo
   	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);

   	if (majorNumber<0)
	{
		printk(KERN_ALERT "Encriptador ha fallado en %d Major number \n", majorNumber);  
      	return majorNumber;
   	}

   	printk(KERN_INFO "%s: se ha registrado correctamente con major number %d\n", DEVICE_NAME, majorNumber);
	printk(KERN_INFO "Para interactuar con el dispositivo,\n");
	printk(KERN_INFO "debe crear un archivo de dispositivo \n");
	printk(KERN_INFO "de la siguiente forma:\n");
	printk(KERN_INFO "sudo mknod -m 666 /dev/%s c %d 0\n", DEVICE_NAME, majorNumber);

 	return SUCCESS;
}

// Esta funcion se llama cuando se remueve el modulo
void cleanup_module (void) 
{
	unregister_chrdev(majorNumber, DEVICE_NAME); // Quita el modulo

	printk(KERN_INFO "%s: se ha removido correctamente con major number %d\n", DEVICE_NAME, majorNumber);
}