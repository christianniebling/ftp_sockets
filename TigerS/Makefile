SERVER = TigerS
CREATE = down_create

.SILENT: all $(SERVER) $(CREATE) clean
.PHONY: all clean

all: $(SERVER) $(CREATE)

$(SERVER): $(SERVER).c
	gcc -Wall -pedantic $(SERVER).c -o $(SERVER)

$(CREATE): $(CREATE).c
	gcc -Wall -pedantic $(CREATE).c -o $(CREATE)


clean: 
	-rm -f $(SERVER)
	-rm -f $(CREATE)
