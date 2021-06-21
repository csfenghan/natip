SRC_DIRS:= lib \
	server \
	client

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
