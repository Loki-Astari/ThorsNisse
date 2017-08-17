
SRC		= $(wildcard *.cpp)
OBJ		= $(patsubst %.cpp, %.o, $(SRC))

all: $(OBJ)
	$(CXX) -shared -o AddBeer.dylib $(OBJ) -L/usr/local/lib -L../../../build/lib -lThorsExpress17D
build: all
install: all

clean:
	rm -rf $(OBJ) AddBeer.dylib
veryclean: clean

%.o: %.cpp
	$(CXX) -std=c++1z -c -I/usr/local/include -I../../../build/include $(CXXFLAGS) $(CPPFLAGS) $<



