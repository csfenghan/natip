CC=g++
CC_FLAGS:=-g -lunix_lib -lpthread -ljsoncpp 
SRC_FILES:=main.cc \
	./natip_client.cc
FINAL_OBJ=client

all:$(FINAL_OBJ)
$(FINAL_OBJ):$(SRC_FILES)
	$(CC) $^ $(CC_FLAGS) -o $@

.PHONY:clean
clean:
	rm $(FINAL_OBJ)
