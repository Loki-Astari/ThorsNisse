
THORSANVIL_ROOT				?= $(realpath ../../)

THOR_SQL_NEEDED_yes			= ThorSQL ThorMySQL
THOR_SERIALIZER_NEEDED_yes	= ThorSerialize
THOR_NOTTRAVIS_NEEDED		= $(THOR_SQL_NEEDED_$(HAVE_ThorsSQL)) $(THOR_SERIALIZER_NEEDED_$(HAVE_ThorsSerialize))

TARGET						= NodeCXX.app
FILE_WARNING_FLAGS			+= -Wno-unknown-pragmas
CXXFLAGS					+= -I$(event_ROOT_DIR)/include $(thorsSQL_ROOT_INCDIR)
LDLIBS						+= -L$(event_ROOT_DIR)/lib $(thorsSQL_ROOT_LIBDIR) $(thorsSerialize_ROOT_LIBDIR) -l$(event_ROOT_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_CONTEXT_LIB) $(BOOST_COROUTINE_LIB) -ldl
LINK_LIBS					= ThorsExpress $(THOR_NOTTRAVIS_NEEDED)

include $(THORSANVIL_ROOT)/build/tools/Makefile




