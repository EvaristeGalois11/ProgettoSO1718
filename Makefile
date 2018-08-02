SRC=./src
BIN=./bin
OPTIMIZATION=-o3
PROGRAMS=$(BIN)/ertms $(BIN)/train $(BIN)/rbc

all: $(PROGRAMS)
	@echo The executables are in the $(BIN) folder

ertms: $(BIN)/ertms
	
train: $(BIN)/train

rbc: $(BIN)/rbc
	
$(BIN)/ertms: $(BIN)/ertms.o $(BIN)/common.o
	gcc $(OPTIMIZATION) $(BIN)/ertms.o $(BIN)/common.o -o $(BIN)/ertms -lm -lrt -pthread 

$(BIN)/ertms.o: $(SRC)/ertms.c
	gcc $(OPTIMIZATION) -c $(SRC)/ertms.c -o $(BIN)/ertms.o

$(BIN)/train: $(BIN)/train.o $(BIN)/route.o $(BIN)/log.o $(BIN)/common.o
	gcc $(OPTIMIZATION) $(BIN)/train.o $(BIN)/route.o $(BIN)/log.o $(BIN)/common.o -o $(BIN)/train -lm

$(BIN)/train.o: $(SRC)/train.c
	gcc $(OPTIMIZATION) -c $(SRC)/train.c -o $(BIN)/train.o

$(BIN)/rbc: $(BIN)/rbc.o $(BIN)/route.o $(BIN)/common.o
	gcc $(OPTIMIZATION) $(BIN)/rbc.o $(BIN)/route.o $(BIN)/common.o -o $(BIN)/rbc -lm

$(BIN)/rbc.o: $(SRC)/rbc.c
	gcc $(OPTIMIZATION) -c $(SRC)/rbc.c -o $(BIN)/rbc.o

$(BIN)/route.o: $(SRC)/route.c $(SRC)/route.h
	gcc $(OPTIMIZATION) -c $(SRC)/route.c -o $(BIN)/route.o

$(BIN)/log.o: $(SRC)/log.c $(SRC)/log.h
	gcc $(OPTIMIZATION) -c $(SRC)/log.c -o $(BIN)/log.o

$(BIN)/common.o: $(SRC)/common.c $(SRC)/common.h
	gcc $(OPTIMIZATION) -c $(SRC)/common.c -o $(BIN)/common.o

clean:
	rm -f $(BIN)/core $(BIN)/*.o $(PROGRAMS)