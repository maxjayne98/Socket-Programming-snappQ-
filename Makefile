output : serverFinal.o clientFinal2.o 
	gcc serverFinal.o  -o serverFinal
	gcc clientFinal2.o  -o clientFinal2


serverFinal.o:serverFinal.c 
	gcc -c serverFinal.c
client.o:clientFinal2.c 
	gcc -c clientFinal2.c

