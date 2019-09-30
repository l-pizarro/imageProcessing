# Image Processing

C application for image processing (fork + pipe + dup2 + execv)  

## Getting Started

These instructions will get you the project up and running on your local machine for testing purposes.

### Prerequisites

This program requires a GNU/Linux distribution, the GNU Compiler Collection (GCC) and **libpng** library. To intall it use the follow command:

```
sudo apt-get install libpng-dev
```

For more info, visit the website http://www.libpng.org/

### Compiling

This program comes with a makefile. To compile it, make sure to be on the root folder and run the next command:

```
make all
```

Once done, there will be a executable for the program called **iamge_proc**. You can also manually compile this program, must use GCC to do it (see the commands in the makefile).

## Running the program

Once the program is compiled you can run it by using the getopt format:

* -c: Image number
* -m: TXT file for image convolution mask
* -n: Sorting threshold
* -b: Displays results on command line (1 if True)

Example

```
./image_proc -c 10 -m Mask.txt -n 70 -b
```

Make sure the images you will use are on the **testImages** folder

## Authors

* **Gabriel Gaete**
* **Leandro Pizarro**
