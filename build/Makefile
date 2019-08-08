#
# This is an example Makefile for a countwords program.  This
# program uses both the scanner module and a counter module.
# Typing 'make' or 'make count' will create the executable file.
#

# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#
# for C++ define  CC = g++
CC = g++
CFLAGS  = -g -Wall
OBJ_PATH = $(PWD)/obj
INCLUDE_PATH = $(PWD)/../include
BIN_PATH = $(PWD)
SRC_PATH = $(PWD)/../src
# typing 'make' will invoke the first target entry in the file 
# (in this case the default target entry)
# you can name this target entry anything, but "default" or "all"
# are the most commonly used names by convention
#
default: scheduler

# To create the executable file count we need the object files
# countwords.o, counter.o, and scanner.o:
#
scheduler: ds_scheduler.o ds_node.o ds_link.o ds_flow.o ds_config_reader.o
	@cd obj;\
		$(CC) $(CFLAGS) ds_scheduler.o  ds_node.o ds_link.o ds_flow.o ds_config_reader.o -o $(BIN_PATH)/scheduler
	@cd ..
	@echo "generated $@"

# To create the object file countwords.o, we need the source
# files countwords.c, scanner.h, and counter.h:
#
ds_scheduler.o:  $(SRC_PATH)/ds_scheduler.cpp 
	@$(CC) $(CFLAGS) -c $(SRC_PATH)/ds_scheduler.cpp -o $(OBJ_PATH)/ds_scheduler.o -I $(INCLUDE_PATH)
	@echo "compiled $@"

ds_node.o:  $(SRC_PATH)/ds_node.cpp 
	@$(CC) $(CFLAGS) -c $(SRC_PATH)/ds_node.cpp -o $(OBJ_PATH)/ds_node.o -I $(INCLUDE_PATH)
	@echo "compiled $@"

ds_link.o:  $(SRC_PATH)/ds_link.cpp 
	@$(CC) $(CFLAGS) -c $(SRC_PATH)/ds_link.cpp -o $(OBJ_PATH)/ds_link.o -I $(INCLUDE_PATH)
	@echo "compiled $@"

ds_flow.o:  $(SRC_PATH)/ds_flow.cpp 
	@$(CC) $(CFLAGS) -c $(SRC_PATH)/ds_flow.cpp -o $(OBJ_PATH)/ds_flow.o -I $(INCLUDE_PATH)
	@echo "compiled $@"

ds_config_reader.o:  $(SRC_PATH)/ds_config_reader.cpp 
	@$(CC) $(CFLAGS) -c $(SRC_PATH)/ds_config_reader.cpp -o $(OBJ_PATH)/ds_config_reader.o -I $(INCLUDE_PATH)
	@echo "compiled $@"


# To start over from scratch, type 'make clean'.  This
# removes the executable file, as well as old .o object
# files and *~ backup files:
#
clean: 
	@$(RM) $(default) $(OBJ_PATH)/*.o *~
	@echo "Deleted object files and binary"
