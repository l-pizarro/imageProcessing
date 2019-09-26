all: general

general:
	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 main.c -o laboratorio1 -lm -Wall

run:
	./laboratorio1 -c 3 -m hola.txt -n 3 -b

clean: 
	rm laboratorio1