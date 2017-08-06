
THORSANVIL_ROOT				?= $(realpath ../../)

TARGET						= ThorsNisse.slib
LDLIBS						= -levent $(BOOST_SYSTEM_LIB) $(BOOST_COROUTINE_LIB)
LINK_LIBS					= ThorsNisseSocket
UNITTEST_LINK_LIBS			= ThorsNisseSocket

%/NisseService.o:			FILE_WARNING_FLAGS	+= -Wno-unknown-pragmas

include $(THORSANVIL_ROOT)/build/tools/Makefile




