
THORSANVIL_ROOT				?= $(realpath ../../)

TARGET						= NodeCXX.app
CXXFLAGS					+= -I$(event_ROOT_DIR)/include $(thorsSQL_ROOT_INCDIR)
LDLIBS						+= -L$(event_ROOT_DIR)/lib $(thorsSQL_ROOT_LIBDIR) -l$(event_ROOT_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_CONTEXT_LIB) $(BOOST_COROUTINE_LIB) -ldl
LINK_LIBS					= ThorsExpress

include $(THORSANVIL_ROOT)/build/tools/Makefile




