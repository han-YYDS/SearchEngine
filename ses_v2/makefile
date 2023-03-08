SRCS:=$(wildcard ./src/*.cc)
OBJECTS:=$(patsubst %c, %o, $(SRCS))
CC:=g++
LIBS:=-lpthread
SERVER:=./bin/server

$(SERVER):$(SRCS)
	$(CC) $^ -o $@  $(LIBS) -llog4cpp -lhiredis -Wall -O0 -g

.PHONY:clean print
print:
	echo $(OBJECTS)
clean:
	rm -rf $(OBJECTS) $(SERVER)
