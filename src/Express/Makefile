
THORSANVIL_ROOT				?= $(realpath ../../)

TARGET						= ThorsExpress.slib
CXXFLAGS					+= -I$(event_ROOT_DIR)/include
LDLIBS						+= $(THOR_STATIC_LOAD_FLAG) -L$(event_ROOT_DIR)/lib -l$(event_ROOT_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_COROUTINE_LIB) $(BOOST_CONTEXT_LIB)
LINK_SLIBS					= ThorSQL ThorsTroll ThorsNisseCoreService ThorsNisseSocket ThorMySQL ThorsNisseSQL

include $(THORSANVIL_ROOT)/build/tools/Makefile



