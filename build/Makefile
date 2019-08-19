CC = g++
CFLAGS  = -g -Wall
OBJ_PATH = $(PWD)/obj
INCLUDE_PATH = $(PWD)/../include
BIN_PATH = $(PWD)
SRC_PATH = $(PWD)/../src
OBJ_FILES = ds_scheduler.o
OBJ_FILES += ds_node.o
OBJ_FILES += ds_link.o
OBJ_FILES += ds_flow.o
OBJ_FILES += ds_config_reader.o
OBJ_FILES += ds_route.o

default: scheduler

scheduler: $(OBJ_FILES)
	@echo "generating $@"
	@cd obj;\
		$(CC) $(CFLAGS) $(OBJ_FILES) -o $(BIN_PATH)/scheduler
	@cd ..

	@echo "Cimpilation Finished! $@"

ds_scheduler.o:  $(SRC_PATH)/ds_scheduler.cpp 
	@echo "compiling $@"
	@$(CC) $(CFLAGS) -c $(SRC_PATH)/ds_scheduler.cpp -o $(OBJ_PATH)/ds_scheduler.o -I $(INCLUDE_PATH)

ds_node.o:  $(SRC_PATH)/ds_node.cpp 
	@echo "compiling $@"
	@$(CC) $(CFLAGS) -c $(SRC_PATH)/ds_node.cpp -o $(OBJ_PATH)/ds_node.o -I $(INCLUDE_PATH)

ds_link.o:  $(SRC_PATH)/ds_link.cpp 
	@echo "compiling $@"
	@$(CC) $(CFLAGS) -c $(SRC_PATH)/ds_link.cpp -o $(OBJ_PATH)/ds_link.o -I $(INCLUDE_PATH)

ds_flow.o:  $(SRC_PATH)/ds_flow.cpp 
	@echo "compiling $@"
	@$(CC) $(CFLAGS) -c $(SRC_PATH)/ds_flow.cpp -o $(OBJ_PATH)/ds_flow.o -I $(INCLUDE_PATH)

ds_config_reader.o:  $(SRC_PATH)/ds_config_reader.cpp 
	@echo "compiling $@"
	@$(CC) $(CFLAGS) -c $(SRC_PATH)/ds_config_reader.cpp -o $(OBJ_PATH)/ds_config_reader.o -I $(INCLUDE_PATH)

ds_route.o:  $(SRC_PATH)/ds_route.cpp 
	@echo "compiling $@"
	@$(CC) $(CFLAGS) -c $(SRC_PATH)/ds_route.cpp -o $(OBJ_PATH)/ds_route.o -I $(INCLUDE_PATH)

clean: 
	@$(RM) $(default) $(OBJ_PATH)/*.o *~
	@echo "Deleted object files and binary"
