SRCS:=$(wildcard *.cc)
OBJECTS:=$(patsubst %c, %o, $(SRCS))
CC:=g++
LIBS:=-lpthread
CLIENT:=client

$(CLIENT):$(SRCS)
	$(CC) $^ -o $@ -g

print:
	echo $(OBJECTS)
clean:
	rm -rf $(OBJECTS) $(CLIENT)
