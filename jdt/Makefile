CC=g++
CC_FLAGS:=-g -ljsoncpp -DJSONCPP
SRC_FILES:=main.cc \
	./mini_protocol.cc
FINAL_OBJ=demo

all:$(FINAL_OBJ)
$(FINAL_OBJ):$(SRC_FILES)
	$(CC) $^ $(CC_FLAGS) -o $@

.PHONY:clean
clean:
	rm $(FINAL_OBJ)
