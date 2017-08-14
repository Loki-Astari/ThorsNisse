
THORSANVIL_ROOT				?= $(realpath ../../)

TARGET						= NodeCXX.app
CXXFLAGS					+= -I$(event_ROOT_DIR)/include  -DBOOST_COROUTINE_VERSION=$(BOOST_COROUTINE_VERSION)
LDLIBS						+= -L$(event_ROOT_DIR)/lib -l$(event_ROOT_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_CONTEXT_LIB) $(BOOST_COROUTINE_LIB)
LINK_LIBS					= ThorsNisse ThorsTroll ThorsNisseSocket ThorsNisseSQL ThorSQL ThorMySQL

include $(THORSANVIL_ROOT)/build/tools/Makefile




