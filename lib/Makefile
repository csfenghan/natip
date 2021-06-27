include ../makefile.define

CC=gcc
CXX=g++
LD=g++
AR=ar
C_FLAGS:=-g -DJSONCPP -I $(INC_PATH)
LD_FLAGS:= -ljsoncpp -lpthread
SRC_FILES:=./jdt_connection.cc \
	./jdt_value.cc \
	./jdt_parse.cc \
	./unix_api.c
OBJ_FILES:=$(patsubst %.cc,%.o,$(SRC_FILES))
OBJ_FILES:=$(patsubst %.c,%.o,$(OBJ_FILES))
FINAL_OBJ=libjdt.a

all:$(FINAL_OBJ)
$(FINAL_OBJ):$(OBJ_FILES)
	@ $(AR) rs $@ $^

%.o:%.c
	@ $(CC) $(C_FLAGS) -c $^ -o $@

%.o:%.cc
	@ $(CXX) $(C_FLAGS) -c $^ -o $@

.PHONY:clean
clean:
	rm $(FINAL_OBJ)
	rm $(OBJ_FILES)
