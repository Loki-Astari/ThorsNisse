
TARGET	= Loadable.dylib
SRC		= $(wildcard *.cpp)
OBJ		= $(patsubst %.cpp, %.o, $(SRC))

include ../../../../../Makefile.config

all: $(OBJ)
	$(CXX) -shared -o $(TARGET) $(OBJ) 
clean:
	rm -rf $(OBJ) $(TARGET)
build:		all
test:		all
install:	all
veryclean:	clean


%.o: %.cpp
	$(CXX) $(CXX_STD_FLAG) -fPIC -c -I.. $<



