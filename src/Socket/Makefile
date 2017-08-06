
THORSANVIL_ROOT				?= $(realpath ../../)

TARGET						= ThorsNisseSocket.slib
CXXFLAGS					+= -I$(event_ROOT_DIR)/include

%/Socket.o:			FILE_WARNING_FLAGS	+= -Wno-unknown-pragmas

include $(THORSANVIL_ROOT)/build/tools/Makefile

