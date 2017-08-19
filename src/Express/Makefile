
THORSANVIL_ROOT				?= $(realpath ../../)

TARGET						= ThorsExpress.slib
LDLIBS						+= $(THOR_STATIC_LOAD_FLAG) -L$(event_ROOT_DIR)/lib $(thorsSQL_ROOT_LIBDIR) -l$(event_ROOT_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_COROUTINE_LIB) $(BOOST_CONTEXT_LIB)

SQL_LIB_yes					= ThorsNisseCoreNisseSQL ThorSQL ThorMySQL
LINK_SLIBS					= ThorsNisseProtocolHTTP ThorsNisseCoreService ThorsNisseCoreSocket $(SQL_LIB_$(HAVE_ThorsSQL))

include $(THORSANVIL_ROOT)/build/tools/Makefile



