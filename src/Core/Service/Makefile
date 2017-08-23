
THORSANVIL_ROOT				?= $(realpath ../../../)

TARGET						= ThorsNisseCoreService.a
CXXFLAGS					+= -I$(event_ROOT_DIR)/include
LDLIBS						+= -L$(event_ROOT_DIR)/lib -l$(event_ROOT_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_COROUTINE_LIB) $(BOOST_CONTEXT_LIB)
LINK_LIBS					= ThorsNisseCoreSocket
UNITTEST_LDLIBS				+= -L$(event_ROOT_DIR)/lib -l$(event_ROOT_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_COROUTINE_LIB) $(BOOST_CONTEXT_LIB)
UNITTEST_LINK_LIBS			+= ThorsNisseCoreSocket

%/Server.o:					FILE_WARNING_FLAGS	+= -Wno-unknown-pragmas

include $(THORSANVIL_ROOT)/build/tools/Makefile




