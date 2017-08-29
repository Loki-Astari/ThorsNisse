
SRC		= $(wildcard *.cpp)
OBJ		= $(patsubst %.cpp, %.o, $(SRC))
CWD		= $(shell pwd)
BUILD		= $(realpath $(CWD)/../../..)


include ../../../Makefile.config

all: $(OBJ)
	$(CXX) -shared -o AddBeer.dylib $(OBJ) -L$(BUILD)/build/lib $(thorsSQL_ROOT_LIBDIR) -lThorsExpress17D -lThorSQL17D -lThorMySQL17D
build: all
test:  all
install: all

clean:
	rm -rf $(OBJ) AddBeer.dylib
veryclean: clean

%.o: %.cpp
	$(CXX) $(CXX_STD_FLAG) -fPIC -c -I$(BUILD)/build/include $(thorsSQL_ROOT_INCDIR) $(CXXFLAGS) $(CPPFLAGS) $(BOOST_CPPFLAGS) $<



