src=./src
bin=./bin

all: $(bin)/ertms $(bin)/train
	@echo The executables are in the $(bin) folder

ertms $(bin)/ertms: $(bin)/ertms.o
	gcc $(bin)/ertms.o -o $(bin)/ertms
    
$(bin)/ertms.o: $(src)/ertms.c
	gcc -c $(src)/ertms.c -o $(bin)/ertms.o

train $(bin)/train: $(bin)/train.o $(bin)/route.o
	gcc $(bin)/train.o $(bin)/route.o -o $(bin)/train

$(bin)/train.o: $(src)/train.c $(src)/route.h
	gcc -c $(src)/train.c -o $(bin)/train.o

$(bin)/route.o: $(src)/route.c $(src)/route.h
	gcc -c $(src)/route.c -o $(bin)/route.o

clean:
	rm -r $(src)