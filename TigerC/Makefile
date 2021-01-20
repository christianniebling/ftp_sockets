CLIENT = TigerC
CREATE = up_create

.SILENT: all $(CLIENT) $(CREATE) clean
.PHONY: all clean

all: $(CLIENT) $(CREATE)

$(CLIENT): $(CLIENT).c
	gcc -Wall -pedantic $(CLIENT).c -o $(CLIENT)

$(CREATE): $(CREATE).c
	gcc -Wall -pedantic $(CREATE).c -o $(CREATE)

clean: 
	-rm -f $(CLIENT)
	-rm -f $(CREATE)
