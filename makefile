all: main

clean:
	rm *.o 

User.o: User.c
	gcc -c -o User.o User.c

Admin.o: Admin.c
	gcc -c -o Admin.o Admin.c

Oper.o: Oper.c
	gcc -c -o Oper.o Oper.c

Glob.o: Glob.c
	gcc -c -o Glob.o Glob.c

Main.o: Main.c
	gcc -c -o Main.o Main.c

main: Main.o Glob.o Oper.o Admin.o User.o
	gcc Main.o Glob.o Oper.o Admin.o User.o -l sqlite3