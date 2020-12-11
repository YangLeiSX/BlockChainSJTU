CC = g++
CFLAGS = -g -std=c++14 -Wall

HEADER = \
	include/main.h \
	include/semaphore.h \
	include/blockchain.hpp \
	include/block.h \
	include/client.h \
	include/honest.h \
	include/forker.h \
	include/normal.h \
	include/selfish.h

VANILLA_SOURCE = \
	src/main_vanilla.cpp \
	src/client.cpp
FORKER_SOURCE = \
	src/main_forker.cpp \
	src/client.cpp \
	src/forker.cpp \
	src/honest.cpp 
SELFISH_SOURCE = \
	src/main_selfish.cpp \
	src/client.cpp \
	src/selfish.cpp \
	src/normal.cpp 

bin/bin_vanilla: $(VANILLA_SOURCE) $(HEADER)
	$(CC) $(CFLAGS) $(VANILLA_SOURCE) -o $@

bin/bin_forker: $(FORKER_SOURCE) $(HEADER)
	$(CC) $(CFLAGS) $(FORKER_SOURCE) -o $@

bin/bin_selfish: $(SELFISH_SOURCE) $(HEADER)
	$(CC) $(CFLAGS) $(SELFISH_SOURCE) -o $@

.PHONY: clean vanilla_run forker_run selfish_run

clear:
	rm -r bin/*.dSYM
	rm visual/*.dot

vanilla_run: bin/bin_vanilla
	@bin/bin_vanilla

forker_run: bin/bin_forker
	@bin/bin_forker

selfish_run: bin/bin_selfish
	@bin/bin_selfish
