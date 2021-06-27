include ../makefile.define

CXX=g++
LD=g++
CC_FLAGS:=-g -I $(INC_PATH) -DJSONCPP
LD_FLAGS:= -ljsoncpp -ljdt -lpthread -L $(LIB_PATH) -lmysqlcppconn

SRC_FILES:=main.cc \
	./natip_server.cc \
	./data_manager.cc

OBJ_FILES:=$(patsubst %.cc,%.o,$(SRC_FILES))
FINAL_OBJ=Server

all:$(FINAL_OBJ)
$(FINAL_OBJ):$(OBJ_FILES)
	 $(LD) $^ -o $@ $(LD_FLAGS)

%.o:%.cc
	 $(CXX) -c $(CC_FLAGS) $^ -o $@

.PHONY:clean
clean:
	rm $(FINAL_OBJ)
	rm $(OBJ_FILES)	
