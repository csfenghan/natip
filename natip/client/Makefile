CC=g++
CC_FLAGS:=-g -lunix_lib -lpthread
SRC_FILES:=client.cc
FINAL_OBJ=client

all:$(FINAL_OBJ)
server:$(SRC_FILES)
	$(CC) $^ $(CC_FLAGS) -o $@

.PHONY:clean
clean:
	rm $(FINAL_OBJ)
