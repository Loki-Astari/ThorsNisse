
SRC		= $(wildcard *.cpp)
OBJ		= $(patsubst %.cpp, %.o, $(SRC))

all: $(OBJ)
	$(CXX) -shared -o AddBeer.dylib $(OBJ) -L/usr/local/lib -lThorsExpress17D

clean:
	rm -rf $(OBJ) AddBeer.dylib

%.o: %.cpp
	$(CXX) -std=c++1z -c -I/usr/local/include $(CXXFLAGS) $(CPPFLAGS) $<



