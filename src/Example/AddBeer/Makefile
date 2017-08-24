
SRC		= $(wildcard *.cpp)
OBJ		= $(patsubst %.cpp, %.o, $(SRC))

include ../../../Makefile.config

all: $(OBJ)
	$(CXX) -shared -o AddBeer.dylib $(OBJ) -L/Users/yorkm/Repository/ThorsNisse/build/lib $(thorsSQL_ROOT_LIBDIR) -lThorsExpress17D -lThorSQL17D -lThorMySQL17D
build: all
test:  all
install: all

clean:
	rm -rf $(OBJ) AddBeer.dylib
veryclean: clean

%.o: %.cpp
	$(CXX) -std=c++1z -c -I/Users/yorkm/Repository/ThorsNisse/build/include $(thorsSQL_ROOT_INCDIR) $(CXXFLAGS) $(CPPFLAGS) $(BOOST_CPPFLAGS) $<



