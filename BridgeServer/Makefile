CXX = g++
CXXFLAGS = -I. -Wall
DEPS = server.h table.h player.h card.h
OBJ = server.o table.o player.o card.o

%.o: %.c $(DEPS)
	$(CXX) -cpp -o $@ $< $(CFXXFLAGS)

bridgeServer: $(OBJ)
	$(CXX) -cpp main.cpp -o $@ $^ $(CXXFLAGS)

clean:
	rm -f $(OBJ) *~ bridgeServer

.PHONY: clean
