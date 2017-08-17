
THORSANVIL_ROOT				?= $(realpath ../../../)

TARGET						= ThorsNisseSQL.a
CXXFLAGS					+= -I$(event_ROOT_DIR)/include
LDLIBS						+= -L$(event_ROOT_DIR)/lib -l$(event_ROOT_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_COROUTINE_LIB) $(BOOST_CONTEXT_LIB)
LINK_LIBS					+= ThorsNisse ThorSQL ThorMySQL

include $(THORSANVIL_ROOT)/build/tools/Makefile

