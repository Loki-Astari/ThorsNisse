
THORSANVIL_ROOT				?= $(realpath ../../)

TARGET						= ThorsExpress.slib
SQL_LIB_yes					= ThorMySQL
LDLIBS_EXTERN_BUILD			= event ThorsDB ThorsSerialize
LINK_LIBS					= $(SQL_LIB_$(HAVE_ThorsDB))
LDLIBS						+= $(BOOST_SYSTEM_LIB) $(BOOST_COROUTINE_LIB) $(BOOST_CONTEXT_LIB) -ldl

SQL_SLIB_yes				= ThorsNisseCoreSQL
LINK_SLIBS					= ThorsNisseProtocolHTTP ThorsNisseCoreService ThorsNisseCoreSocket $(SQL_SLIB_$(HAVE_ThorsSQL))

include $(THORSANVIL_ROOT)/build/tools/Makefile



