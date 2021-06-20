CC=g++
CC_FLAGS:=-g -ljsoncpp -DJSONCPP 
SRC_DIRS:= lib \
	server \
	client
ROOT_PATH:=/home/fenghan/natip
INC_PATH:=$(ROOT_PATH)/include
LIB_PATH:=$(ROOT_PATH)/lib

export INC_PATH LIB_PATH

all:$(SRC_FILES)
	@(for dir in $(SRC_DIRS); \
		do \
		make -C $$dir; \
	done)

.PHONY:clean
clean:
	@(for dir in $(SRC_DIRS); \
		do \
		make clean -C $$dir; \
	done)
