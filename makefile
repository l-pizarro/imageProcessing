all:
	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 main.c functions/general/general_functions.c functions/general/barrier_implementation.c functions/imageProcessing/image_processing.c -o lab2 -lm -Wall -lpng
# 	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/reader.c functions/general/general_functions.c functions/imageProcessing/image_processing.c functions/general/barrier_implementation.c -o reader -lm -Wall -lpng
# 	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/convolver.c functions/general/general_functions.c functions/imageProcessing/image_processing.c functions/general/barrier_implementation.c -o convolver -lm -Wall -lpng
# 	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/rectifier.c functions/general/general_functions.c functions/imageProcessing/image_processing.c functions/general/barrier_implementation.c -o rectifier -lm -Wall -lpng
# 	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/pooler.c functions/general/general_functions.c functions/imageProcessing/image_processing.c functions/general/barrier_implementation.c -o pooler -lm -Wall -lpng
# 	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/clasifier.c functions/general/general_functions.c functions/imageProcessing/image_processing.c functions/general/barrier_implementation.c -o clasifier -lm -Wall -lpng
# 	gcc -I/usr/local/Cellar/libpng/1.6.37/include/libpng16 -L/usr/local/Cellar/libpng/1.6.37/lib -lpng16 ./functions/pipeline/writer.c functions/general/general_functions.c functions/imageProcessing/image_processing.c functions/general/barrier_implementation.c -o writer -lm -Wall -lpng
run:
	./lab2 -c 4 -m test.txt -n 20 -b -h 3 -t 9

clean:
# 	rm clasifier
# 	rm convolver
	rm lab2
# 	rm pooler
# 	rm reader
# 	rm rectifier
# 	rm writer