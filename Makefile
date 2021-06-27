SRC_DIRS:= lib \
	server \
	client

all:$(SRC_FILES)
	@(for dir in $(SRC_DIRS); \
		do \
		make -C $$dir; \
	done)

.PHONY:server
.PHONY:client
.PHONY:clean
server:
	@make -C lib
	@make -C server

client:
	@make -C lib
	@make -C client
clean:
	@(for dir in $(SRC_DIRS); \
		do \
		make clean -C $$dir; \
	done)
