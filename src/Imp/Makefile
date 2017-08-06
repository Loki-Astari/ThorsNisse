
THORSANVIL_ROOT				?= $(realpath ../../)

TARGET						= ThorsImp.slib
XLDLIBS						= -levent $(BOOST_SYSTEM_LIB) $(BOOST_CONTEXT_LIB) $(BOOST_COROUTINE_LIB)
LDLIBS						= -levent $(BOOST_CONTEXT_LIB)
LINK_LIBS					= ThorsNisse ThorsNisseSocket
UNITTEST_LINK_LIBS			= ThorsNisse ThorsNisseSocket

include $(THORSANVIL_ROOT)/build/tools/Makefile




