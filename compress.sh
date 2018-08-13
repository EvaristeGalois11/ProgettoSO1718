#!/bin/bash
#Comprimere solo il necessario per la consegna

make clean
tar -czvf PROGETTO_SO_1718.tar.gz --exclude=.gitkeep Makefile src bin doc resources/routes