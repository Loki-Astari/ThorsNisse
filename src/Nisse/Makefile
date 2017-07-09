
THORSANVIL_ROOT				?= $(realpath ../../)

TARGET						= Nisse.app
LDLIBS						= -levent -lboost_system -lboost_coroutine
LINK_LIBS					= ThorsNisseSocket
VERA_IGNORE					= http_parser.cpp http_parser.h

%/NisseService.o:			FILE_WARNING_FLAGS	+= -Wno-unknown-pragmas

include $(THORSANVIL_ROOT)/build/tools/Makefile




