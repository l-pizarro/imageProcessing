all:
	gcc main.c functions/general/general_functions.c -o laboratorio1 -lm -Wall
	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/reader.c functions/general/general_functions.c functions/imageProcessing/image_processing.c  -o reader -lm -Wall
	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/convolver.c functions/general/general_functions.c functions/imageProcessing/image_processing.c  -o convolver -lm -Wall
	gcc ./functions/pipeline/rectifier.c -o rectifier -lm -Wall
	gcc ./functions/pipeline/poller.c -o poller -lm -Wall
	gcc ./functions/pipeline/clasifier.c -o clasifier -lm -Wall
	gcc ./functions/pipeline/writer.c -o writer -lm -Wall
run:
	./laboratorio1 -c 4 -m test.txt -n 70 -b