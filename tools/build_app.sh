#!/bin/bash
cd ../
rm bin/*
gcc app/main.c -o netstat_app -lnetstat -I./inc
mv netstat_app bin/

