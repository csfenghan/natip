include ../makefile.define

CC=g++
LD=g++
AR=ar
CC_FLAGS:=-g -DJSONCPP -I $(INC_PATH)
LD_FLAGS:= -ljsoncpp
SRC_FILES:=jdt.cc
OBJ_FILES:=$(patsubst %.cc,%.o,$(SRC_FILES))
FINAL_OBJ=libjdt.a

all:$(FINAL_OBJ)
$(FINAL_OBJ):$(OBJ_FILES)
	@ $(AR) rs $@ $^

%.o:%.cc
	@ $(CC) $(CC_FLAGS) -c $^ -o $@

.PHONY:clean
clean:
	rm $(FINAL_OBJ)
	rm $(OBJ_FILES)
