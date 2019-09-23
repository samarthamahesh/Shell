output: shell.o
	gcc shell.o -o shell

shellcse.o: shell.c
	gcc -c shell.c

clean:
	rm *.o shell