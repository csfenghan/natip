CC=g++
CC_FLAGS:=-g -lunix_lib -lpthread -ljsoncpp -L /usr/lib/x86_64-linux-gnu/
SRC_FILES:=client.cc
FINAL_OBJ=client

all:$(FINAL_OBJ)
$(FINAL_OBJ):$(SRC_FILES)
	$(CC) $^ $(CC_FLAGS) -o $@

.PHONY:clean
clean:
	rm $(FINAL_OBJ)
