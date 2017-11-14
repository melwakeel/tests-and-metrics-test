#!/bin/bash
cd ../
rm ./lib/*
gcc -c -fPIC src/netstat.c -o netstat.o -I./inc
gcc -shared -Wl,-soname,libnetstat.so netstat.o -L/usr/local/lib -lcurl -o libnetstat.so
sudo cp *.so /usr/lib/
mv *.so lib/
rm *.o
