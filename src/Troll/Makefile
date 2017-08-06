
THORSANVIL_ROOT				?= $(realpath ../../)

TARGET						= ThorsTroll.slib
LDLIBS						= -levent $(BOOST_SYSTEM_LIB) $(BOOST_CONTEXT_LIB) $(BOOST_COROUTINE_LIB)
LINK_LIBS					= ThorsNisse ThorsNisseSocket
TEST_IGNORE					= http_parser.cpp http_parser.h
UNITTEST_LINK_LIBS			= ThorsNisse ThorsNisseSocket

include $(THORSANVIL_ROOT)/build/tools/Makefile




