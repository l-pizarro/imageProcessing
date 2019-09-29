all:
	gcc main.c functions/general/general_functions.c -o laboratorio1 -lm -Wall
	gcc ./functions/pipeline/reader.c -o reader -lm -Wall
	gcc ./functions/pipeline/convolver.c -o convolver -lm -Wall
	gcc ./functions/pipeline/rectifier.c -o rectifier -lm -Wall
	gcc ./functions/pipeline/poller.c -o poller -lm -Wall
	gcc ./functions/pipeline/clasifier.c -o clasifier -lm -Wall
	gcc ./functions/pipeline/writer.c -o writer -lm -Wall

run:
	./laboratorio1 -c 10 -m Hola.txt -n 555 -b