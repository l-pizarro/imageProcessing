all:
	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 main.c functions/general/general_functions.c -o image_proc -lm -Wall -lpng
	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/reader.c functions/general/general_functions.c functions/imageProcessing/image_processing.c  -o reader -lm -Wall -lpng
	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/convolver.c functions/general/general_functions.c functions/imageProcessing/image_processing.c  -o convolver -lm -Wall -lpng
	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/rectifier.c functions/general/general_functions.c functions/imageProcessing/image_processing.c  -o rectifier -lm -Wall -lpng
	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/pooler.c functions/general/general_functions.c functions/imageProcessing/image_processing.c  -o pooler -lm -Wall -lpng
	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/clasifier.c functions/general/general_functions.c functions/imageProcessing/image_processing.c  -o clasifier -lm -Wall -lpng
	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/writer.c functions/general/general_functions.c -o writer -lm -Wall -lpng
run:
	./image_proc -c 4 -m test.txt -n 20 -b

clean:
	rm clasifier
	rm convolver
	rm laboratorio1
	rm pooler
	rm reader
	rm rectifier
	rm writer