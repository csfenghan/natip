CC=g++
CC_FLAGS:=-g -lunix_lib -lpthread -ljsoncpp
SRC_FILES:=main.cc \
	   ./natip_server.cc
FINAL_OBJ=server

all:$(FINAL_OBJ)
$(FINAL_OBJ):$(SRC_FILES)
	$(CC) $^ $(CC_FLAGS) -o $@

.PHONY:clean
clean:
	rm $(FINAL_OBJ)
