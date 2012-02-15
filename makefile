# f2hpp Makefile
# Copyright (C) 2012 David Jolly

CC=g++
APP=f2hpp
SRC=src/

all: 
	$(CC) -o $(APP) $(SRC)$(APP).cpp

clean:
	rm -f $(APP)
