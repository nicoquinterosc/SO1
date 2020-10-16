OBJECTS := desencriptador/desencriptador.o encriptador/encriptador.o ioctl.o

obj-m := desencriptador/desencriptador.o encriptador/encriptador.o
fifo-objs := $(OBJECTS)

build:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

insmod:
	sudo insmod encriptador/encriptador.ko
	sudo insmod desencriptador/desencriptador.ko
	
rmmod:
	sudo rmmod encriptador
	sudo rmmod desencriptador

rmdev:
	sudo rm /dev/encriptador
	sudo rm /dev/desencriptador	

msg:
	sudo tail /var/log/syslog	