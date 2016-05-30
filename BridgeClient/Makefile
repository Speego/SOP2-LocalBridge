CXX = g++
CXXFLAGS = -I. -Wall
DEPS = client.h card.h
OBJ = client.o card.o

%.o: %.c $(DEPS)
	$(CXX) -cpp -o $@ $< $(CFXXFLAGS)

bridgeClient: $(OBJ)
	$(CXX) -cpp mainC.cpp -o $@ $^ $(CXXFLAGS)

clean:
	rm -f $(OBJ) *~ bridgeClient

.PHONY: clean
