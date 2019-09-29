all:
	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 main.c functions/general/general_functions.c -o laboratorio1 -lm -lpng
	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/reader.c functions/general/general_functions.c functions/imageProcessing/image_processing.c  -o reader -lm -lpng
	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/convolver.c functions/general/general_functions.c functions/imageProcessing/image_processing.c  -o convolver -lm -lpng
	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/rectifier.c functions/general/general_functions.c functions/imageProcessing/image_processing.c  -o rectifier -lm -lpng
	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/pooler.c functions/general/general_functions.c functions/imageProcessing/image_processing.c  -o pooler -lm -lpng
	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/clasifier.c functions/general/general_functions.c functions/imageProcessing/image_processing.c  -o clasifier -lm -lpng
	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/writer.c functions/general/general_functions.c -o writer -lm -lpng
run:
	./laboratorio1 -c 4 -m test.txt -n 20 -b

clean:
	rm clasifier
	rm convolver
	rm laboratorio1
	rm pooler
	rm reader
	rm rectifier
	rm writer