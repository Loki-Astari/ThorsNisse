
THORSANVIL_ROOT				?= $(realpath ../../)

TARGET						= ThorsImp.slib
CXXFLAGS					+= -I$(event_ROOT_DIR)/include -DBOOST_COROUTINE_VERSION=$(BOOST_COROUTINE_VERSION)
LDLIBS						+= -L$(event_ROOT_DIR)/lib -l$(event_ROOT_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_COROUTINE_LIB) $(BOOST_CONTEXT_LIB)
LINK_LIBS					= ThorsNisse ThorsNisseSocket
UNITTEST_LINK_LIBS			= ThorsNisse ThorsNisseSocket

IGNORE_BOOST_ISSUES_2	= -Wno-extra -Wno-terminate
IGNORE_BOOST_ISSUES		= $(IGNORE_BOOST_ISSUES_$(BOOST_COROUTINE_VERSION))

%/ProtocolSimpleStream.o:	FILE_WARNING_FLAGS	+= $(IGNORE_BOOST_ISSUES) 

include $(THORSANVIL_ROOT)/build/tools/Makefile




