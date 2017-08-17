
THORSANVIL_ROOT				?= $(realpath ../../../)

TARGET						= ThorsNisseCoreService.a
CXXFLAGS					+= -I$(event_ROOT_DIR)/include
LDLIBS						+= -L$(event_ROOT_DIR)/lib -l$(event_ROOT_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_COROUTINE_LIB)
LINK_LIBS					= ThorsNisseSocket
UNITTEST_LINK_LIBS			= ThorsNisseSocket

%/NisseService.o:			FILE_WARNING_FLAGS	+= -Wno-unknown-pragmas

include $(THORSANVIL_ROOT)/build/tools/Makefile




