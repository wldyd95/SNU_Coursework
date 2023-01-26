# 4190.308 Computer Architecture (Spring 2019)
# Project #3: Drawing diagonal lines in an image
### Due: 11:59PM, May 19 (Sunday)


## Introduction

In this project, you will implement a basic image processing program using the x86-64 assembly language. An image file in the BMP format will be given as an input to your program. This assignment aims at introducing various primitive instructions provided by the x86-64 assembly language. In addition, you will learn the basic structure of the BMP image file.

## Problem specification

Complete the file ``bmpdiag.s`` that implements the function ``bmp_diag()`` in the x86-64 assembly language. The prototype of ``bmp_diag()`` is as follows:

```
    void bmp_diag (unsigned char *imgptr, long long width, long long height, long long gap);
```

The first argument, ``imgptr``, points to the bitmap data that stores the actual image, pixel by pixel. The next two arguments, ``width`` and ``height``, represent the width and the height of the given image (in pixels), respectively. The last argument, ``gap``, indicates the number of pixels between consecutive diagonal lines. Your task is to draw the left and right diagonal lines in red color starting at (0, 0) by manipulating the bitmap data in ``bmp_diag()``.

## Backgrounds

### RGB color model

[<img align="right" width="150" src="https://upload.wikimedia.org/wikipedia/commons/c/c2/AdditiveColor.svg?sanitize=true">](https://en.wikipedia.org/wiki/RGB_color_model)

The RGB color model is one of the most common ways to encode color images in the digital world. The RGB color model is based on the theory that all visible colors can be created using the primary additive colors: red, green, and blue. When two or three of them are combined in different amounts, other colors are produced. The RGB color model is important to graphic design as it is used in computer monitors.

### BMP file format

The BMP file format is an image file format used to store digital images, especially on Microsoft Windows operating systems. A BMP file contains a BMP file header, a Bitmap information header, an optional color palette, and an array of bytes that defines the bitmap data. Since the BMP file format has been extended several times, it supports different types of encoding modes. For example, image pixels can be stored with a color depth of 1 (black and white), 4, 8, 16, 24 (true color, 16.7 million colors) or 32 bits per pixel. Images of 8 bits and fewer can be either grayscale or indexed color mode. More details on the BMP file format can be found at http://en.wikipedia.org/wiki/BMP_file_format.

In this project, we will focus only on the __24-bit uncompressed RGB color mode__ with the "Windows V3" bitmap information header. Under this mode, our target image file has the following structure.

```
              +-----------------------------------------+
              |   BMP file header (14 bytes)            |
              +-----------------------------------------+
              |   Bitmap information header (40 bytes)  |
              +-----------------------------------------+
    imgptr -> |   Bitmap data                           |
              |                                         |
              |                                         |
              |                                         |
              +-----------------------------------------+
```

We will provide you with the skeleton code, in which all the BMP file header and the Bitmap information header are parsed. So you don't have to worry about these headers. Before manipulating the bitmap data, we check for these headers to make sure the target image file is in the right mode, and then extract the width and the height of the image. The first argument of ``bmp_diag()``, ``imgptr``, will point to the memory address that contains the actual bitmap data.

### Bitmap data format

The bitmap data describes the image, pixel by pixel. Each pixel consists of an 8-bit blue (B) byte, a green (G) byte, and a red (R) byte in that order. __Pixels are stored "upside-down"__ with respect to normal image raster scan order, starting in the lower left corner, going from left to right, and then row by row from the bottom to the top of the image. Note that __the number of bytes occupied by each row should be a multiple of 4__. If that's not the case, the remaining bytes are padded with zeroes. All you have to do is to find all the pixels on diagonal lines and change the corresponding bytes of such pixels to (B = 0, G = 0, R = 255). The following figure summarizes the structure of the bitmap data.

![BMP image format](http://csl.snu.ac.kr/courses/4190.308/2019-1/bmpformat.jpg)


## Skeleton code and sample data

The following skeleton code and sample data are provided for this project.

|  File                |          Description                 |
|----------------------|--------------------------------------|
| ``Makefile``         | This is a file used by the GNU make utility |
| ``bmp.h``	           | This header file contains various data structures for BMP file parsing and the function prototype for ``bmp_diag()``. |
| ``bmp.c``            | This is a C program which contains ``main()``, ``bmp_in()``, and ``bmp_out()`` functions. The ``bmp_in()`` function reads the content of the BMP file into the memory and parses its header. The ``bmp_out()`` function creates a new image file which is modified by ``bmp_diag()``. |
| ``bmpdiag.s``        | This is skeleton assembly code for ``bmp_diag()``. You are supposed to fill the main body of this file. |
| ``bordeaux.bmp``     | The is a sample BMP file. |
| ``bordeaux-ans.bmp`` | This is the sample output BMP file when the ``gap`` is set to 100. |


You can build the executable file using the ``make`` command. The name of the final executable file is ``bmpdiag``. You can also perform ``make run`` and ``make test`` to see if your output is correct or not, as shown in the following. 


```
$ git clone https://github.com/snu-csl/ca-pa3.git
$ cd ca-pa3
$ ls -l
total 1404
-rw-rw-r-- 1 jinsoo jinsoo   3230  4월 24 10:28 bmp.c
-rw-rw-r-- 1 jinsoo jinsoo    860  4월 24 09:54 bmpdiag.s
-rw-rw-r-- 1 jinsoo jinsoo   1519  4월 24 09:47 bmp.h
-rw-r--r-- 1 jinsoo jinsoo 689336  4월 24 10:25 bordeaux-ans.bmp
-rw-rw-r-- 1 jinsoo jinsoo 689336  4월 24 10:25 bordeaux.bmp
-rw-rw-r-- 1 jinsoo jinsoo    958  4월 24 10:26 Makefile
-rw-rw-r-- 1 jinsoo jinsoo   7266  4월 24 09:44 README.md
$
$ make
gcc -g -Og -Wall -c bmp.c -o bmp.o
as -g bmpdiag.s -o bmpdiag.o
gcc  bmp.o bmpdiag.o -o bmpdiag
$
$ make run
./bmpdiag       bordeaux.bmp out.bmp 100
BMP file: bordeaux.bmp (640 x 359 pixels, 24 bits/pixel)
$
$ make test
cmp out.bmp bordeaux-ans.bmp
out.bmp bordeaux-ans.bmp differ: byte 181, line 3
Makefile:52: recipe for target 'test' failed
make: \*\*\* [test] Error 1
$
```

The skeleton code and sample data can be download from Github at https://github.com/snu-csl/ca-pa3/. If you don't have the git utility, you need to install it first by performing "``sudo apt install git``" command. You will find `gdb` (GNU Debugger) useful when debugging your program. For more information on `gdb`, please refer to the [GDB.md](GDB.md) file in this directory.

## Restrictions

* In the main body of ``bmp_diag()``, you should __use %rax, %rcx, %rdx, %rsi, %rdi, and %r8 registers only__. If you are running out of registers, use stack as temporary storage. 

* Your program should work for BMP images of any size.

* Your program should work for any positive value of "gap".

* You should leave the bytes in the padding area untouched.

* You are allowed to define any extra functions inside of the ``bmpdiag.s`` file, if necessary.

## Sample output

This is a sample BMP file with 640 x 359 pixels (``bordeaux.bmp``). If you can not see the image, please try again with the Google Chrome browser.

<p align="center">
<img src="http://csl.snu.ac.kr/courses/4190.308/2019-1/bordeaux.bmp?sanitize=true">
</p>

If you run your program as follows, it will create a new file named ``out.bmp``.

```
    $ ./bmpdiag bordeaux.bmp out.bmp 100
```

The resulting ``out.bmp`` file should look like this. The gap between diagonal lines is 100 pixels. Your output file ``out.bmp`` should be identical to the ``bordeaux-ans.bmp`` file. You can notice that the diagonal lines start at the upper left corner, i.e., at coordinates (0, 0), and they are drawn at intervals of 100 pixels to the left and right. (Again, if you are unable to see the image, please use the Google Chrome browser.)

<p align="center">
<img src="http://csl.snu.ac.kr/courses/4190.308/2019-1/bordeaux-ans.bmp?sanitize=true">
</p>

## Hand-in instructions

* Submit only the `bmpdiag.s` file to the submission server.
* If your file contains any register names other than the allowed ones, your file will be rejected by the server.
* __Top 5 solutions with smallest code size will have a 10% extra bonus.__ The code size is measured by the total number of bytes for the object code of ``bmp_diag()``.


## Logistics

* You will work on this project alone.
* Only the upload submitted before the deadline will receive the full credit. 25% of the credit will be deducted for every single day delay.
* __You can use up to 5 _slip days_ during this semester__. If your submission is delayed by 1 day and if you decided to use 1 slip day, there will be no penalty. In this case, you should explicitly declare the number of slip days you want to use in the QnA board of the submission server after each submission. Saving the slip days for later projects is highly recommended!
* Any attempt to copy others' work will result in heavy penalty (for both the copier and the originator). Don't take a risk.

Have fun!

[Jin-Soo Kim](mailto:jinsoo.kim_AT_snu.ac.kr)  
[Systems Software and Architecture Laboratory](http://csl.snu.ac.kr)  
[Dept. of Computer Science and Engineering](http://cse.snu.ac.kr)  
[Seoul National University](http://www.snu.ac.kr)
