
THORSANVIL_ROOT				?= $(realpath ../../)

TARGET						= ThorsNisseSQL.slib
CXXFLAGS					+= -I$(event_ROOT_DIR)/include -DBOOST_COROUTINE_VERSION=$(BOOST_COROUTINE_VERSION)
LDLIBS						+= -L$(event_ROOT_DIR)/lib -l$(event_ROOT_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_COROUTINE_LIB) $(BOOST_CONTEXT_LIB)
LINK_LIBS					+= ThorsNisse ThorSQL ThorMySQL

include $(THORSANVIL_ROOT)/build/tools/Makefile

