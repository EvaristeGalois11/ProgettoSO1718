SRC=./src
BIN=./bin
OPTIMIZATION=

.PHONY: bin

all: bin $(BIN)/ertms $(BIN)/train
	@echo The executables are in the $(BIN) folder

ertms $(BIN)/ertms: bin $(BIN)/ertms.o
	gcc $(OPTIMIZATION) $(BIN)/ertms.o -o $(BIN)/ertms
    
$(BIN)/ertms.o: $(SRC)/ertms.c
	gcc $(OPTIMIZATION) -c $(SRC)/ertms.c -o $(BIN)/ertms.o

train $(BIN)/train: bin $(BIN)/train.o $(BIN)/route.o
	gcc $(OPTIMIZATION) $(BIN)/train.o $(BIN)/route.o -o $(BIN)/train

$(BIN)/train.o: $(SRC)/train.c $(SRC)/route.h
	gcc $(OPTIMIZATION) -c $(SRC)/train.c -o $(BIN)/train.o

$(BIN)/route.o: $(SRC)/route.c $(SRC)/route.h
	gcc $(OPTIMIZATION) -c $(SRC)/route.c -o $(BIN)/route.o

bin: 
	mkdir -p $(BIN)

clean:
	rm -r $(BIN)