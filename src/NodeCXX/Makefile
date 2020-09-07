
THORSANVIL_ROOT				?= $(realpath ../../)

THOR_SQL_NEEDED_yes			= ThorMySQL
THOR_NOTTRAVIS_NEEDED		= $(THOR_SQL_NEEDED_$(HAVE_ThorsDB))

TARGET						= NodeCXX.app
LDLIBS_EXTERN_BUILD			= event ThorsSerialize ThorsDB
FILE_WARNING_FLAGS			+= -Wno-unknown-pragmas
CXXFLAGS					+= $(thorsSQL_ROOT_INCDIR)
LDLIBS						+= -l$(event_ROOT_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_CONTEXT_LIB) $(BOOST_COROUTINE_LIB) -ldl
LINK_LIBS					= ThorsExpress $(THOR_NOTTRAVIS_NEEDED)

include $(THORSANVIL_ROOT)/build/tools/Makefile




