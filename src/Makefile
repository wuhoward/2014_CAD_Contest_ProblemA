#CC        = gcc -lstdc++
CXX       = g++
CFLAGS    = -g -Wall
CFLAGS    = -O3
OBJ_FILES = main.o explorer.o generator.o exploreGlucose.o
#LIBS      = -lm -ldl -rdynamic -lreadline -lncurses -lpthread
LIBS      = -lm -ldl -rdynamic
LIBABC    = libabc.a

expcnf: $(OBJ_FILES) $(LIBABC)
	$(CXX) $(CFLAGS) $(OBJ_FILES) $(LIBABC) $(LIBS) -o $@

main.o             : main.cpp
	$(CXX) $(CFLAGS) -c main.cpp -o $@

explorer.o         : explorer.cpp explorer.h
	$(CXX) $(CFLAGS) -c explorer.cpp -o $@

generator.o        : generator.cpp generator.h
	$(CXX) $(CFLAGS) -c generator.cpp -o $@

exploreGlucose.o   : exploreGlucose.cpp
	$(CXX) $(CFLAGS) -c exploreGlucose.cpp -o $@

clean:
	rm -f *.o expcnf.exe
