
THORSANVIL_ROOT				?= $(realpath ../../../)

TARGET						= ThorsNisseProtocolHTTP.a
CXXFLAGS					+= -I$(event_ROOT_DIR)/include
LDLIBS						+= -L$(event_ROOT_DIR)/lib -l$(event_ROOT_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_COROUTINE_LIB) $(BOOST_CONTEXT_LIB)
LINK_LIBS					= ThorsNisseCoreService ThorsNisseCoreSocket
TEST_IGNORE					= http_parser.cpp http_parser.h
UNITTEST_LDLIBS				= -ldl
UNITTEST_LINK_LIBS			= ThorsNisseCoreService ThorsNisseCoreSocket

IGNORE_BOOST_ISSUES_2	= -Wno-extra -Wno-terminate
IGNORE_BOOST_ISSUES		= $(IGNORE_BOOST_ISSUES_$(BOOST_COROUTINE_VERSION))

%/HTTPProtocol.o:	FILE_WARNING_FLAGS	+= $(IGNORE_BOOST_ISSUES) 
%/HttpScanner.o:	FILE_WARNING_FLAGS	+= $(IGNORE_BOOST_ISSUES) 
%/Binder.o:			FILE_WARNING_FLAGS	+= $(IGNORE_BOOST_ISSUES)
%/Route.o:			FILE_WARNING_FLAGS	+= $(IGNORE_BOOST_ISSUES)
%/Types.o:			FILE_WARNING_FLAGS	+= $(IGNORE_BOOST_ISSUES)


include $(THORSANVIL_ROOT)/build/tools/Makefile
COVERAGE_REQUIRED			= 50



