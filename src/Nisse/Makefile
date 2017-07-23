
THORSANVIL_ROOT				?= $(realpath ../../)

TARGET						= ThorsNisse.slib
LDLIBS						= -levent -lboost_system -lboost_coroutine
LINK_LIBS					= ThorsNisseSocket
UNITTEST_LINK_LIBS			= ThorsNisseSocket

%/NisseService.o:			FILE_WARNING_FLAGS	+= -Wno-unknown-pragmas

include $(THORSANVIL_ROOT)/build/tools/Makefile




