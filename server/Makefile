include ../makefile.define

CC=g++
LD=g++
CC_FLAGS:=-g -I $(INC_PATH) -DJSONCPP
LD_FLAGS:= -lunix_lib -lpthread -ljsoncpp -ljdt -L $(LIB_PATH)

SRC_FILES:=main.cc \
	./natip_server.cc
OBJ_FILES:=$(patsubst %.cc,%.o,$(SRC_FILES))
FINAL_OBJ=server

all:$(FINAL_OBJ)
$(FINAL_OBJ):$(OBJ_FILES)
	 $(LD) $^ -o $@ $(LD_FLAGS)

%.o:%.cc
	 $(CC) -c $(CC_FLAGS) $^ -o $@

.PHONY:clean
clean:
	echo $(OBJ_FILES)
	rm $(FINAL_OBJ)
	rm $(OBJ_FILES)	
