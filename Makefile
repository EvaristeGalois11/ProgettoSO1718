SRC=./src
BIN=./bin
OPTIMIZATION=-o3
PROGRAMS=$(BIN)/ertms $(BIN)/train

all: $(PROGRAMS)
	@echo The executables are in the $(BIN) folder

ertms: $(BIN)/ertms
	
train: $(BIN)/train
	
$(BIN)/ertms: $(BIN)/ertms.o
	gcc $(OPTIMIZATION) $(BIN)/ertms.o -o $(BIN)/ertms -lm
    
$(BIN)/ertms.o: $(SRC)/ertms.c
	gcc $(OPTIMIZATION) -c $(SRC)/ertms.c -o $(BIN)/ertms.o

$(BIN)/train: $(BIN)/train.o $(BIN)/route.o
	gcc $(OPTIMIZATION) $(BIN)/train.o $(BIN)/route.o -o $(BIN)/train

$(BIN)/train.o: $(SRC)/train.c $(SRC)/route.h
	gcc $(OPTIMIZATION) -c $(SRC)/train.c -o $(BIN)/train.o

$(BIN)/route.o: $(SRC)/route.c $(SRC)/route.h
	gcc $(OPTIMIZATION) -c $(SRC)/route.c -o $(BIN)/route.o

clean:
	rm -f $(BIN)/core $(BIN)/*.o $(PROGRAMS)