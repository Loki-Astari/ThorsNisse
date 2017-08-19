
THORSANVIL_ROOT				?= $(realpath ../../)

TARGET						= NodeCXX.app
CXXFLAGS					+= -I$(event_ROOT_DIR)/include
LDLIBS						+= -L$(event_ROOT_DIR)/lib -l$(event_ROOT_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_CONTEXT_LIB) $(BOOST_COROUTINE_LIB) -ldl
LINK_LIBS					= ThorsExpress

include $(THORSANVIL_ROOT)/build/tools/Makefile




