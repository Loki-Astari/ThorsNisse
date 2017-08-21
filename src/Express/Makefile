
THORSANVIL_ROOT				?= $(realpath ../../)

TARGET						= ThorsExpress.slib
SQL_LIB_yes					= ThorSQL ThorMySQL
LINK_LIBS					= $(SQL_LIB_$(HAVE_ThorsSQL)) ThorSerialize
LDLIBS						+= -L$(event_ROOT_DIR)/lib $(thorsSQL_ROOT_LIBDIR) -l$(event_ROOT_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_COROUTINE_LIB) $(BOOST_CONTEXT_LIB)

SQL_SLIB_yes				= ThorsNisseCoreSQL
LINK_SLIBS					= ThorsNisseProtocolHTTP ThorsNisseCoreService ThorsNisseCoreSocket $(SQL_SLIB_$(HAVE_ThorsSQL))

include $(THORSANVIL_ROOT)/build/tools/Makefile



