compile:
 ./configure  --disable-cxx --prefix=/home/...

After configure the file named Makefile is generated in current directory
if no error while configuring.
"make" and "make install" to install the target in you directory described
by '--prefix'
The static lib's path is ./lib/.lib/libconfig.a

If you need the static lib and it's head file only just do configure and
make,then you will get your lib file ./lib/.lib/libconfig.a and head file
./lib/libconfig.h

Or you can cd to the lib directory and execute make commands directly.



System install libconfig for ubuntu:
sudo apt-get install libconfig-dev
