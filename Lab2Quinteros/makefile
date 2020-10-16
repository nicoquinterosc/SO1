
OBJ = baash.o parteA.o parteB.o parteC.o
baash: $(OBJ)
	gcc -Wall -pedantic -o baash $(OBJ)

baash.o: baash.c 
parteA.o: parteA.c 
parteB.o: parteB.c
parteC.o: parteC.c

run:
	./baash

clean:
	rm baash *.out *.o