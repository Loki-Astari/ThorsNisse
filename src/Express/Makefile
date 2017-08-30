
THORSANVIL_ROOT				?= $(realpath ../../)

THOR_SERIALIZER_NEEDED_yes▸ = ThorSerialize
THOR_SERIALIZER_NEEDED▸ ▸   = $(THOR_SERIALIZER_NEEDED_$(HAVE_ThorsSerialize))

TARGET						= ThorsExpress.slib
SQL_LIB_yes					= ThorSQL ThorMySQL
LINK_LIBS					= $(SQL_LIB_$(HAVE_ThorsSQL)) $(THOR_SERIALIZER_NEEDED)
LDLIBS						+= -L$(event_ROOT_DIR)/lib $(thorsSQL_ROOT_LIBDIR) $(thorsSerialize_ROOT_LIBDIR) -l$(event_ROOT_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_COROUTINE_LIB) $(BOOST_CONTEXT_LIB) -ldl

SQL_SLIB_yes				= ThorsNisseCoreSQL
LINK_SLIBS					= ThorsNisseProtocolHTTP ThorsNisseCoreService ThorsNisseCoreSocket $(SQL_SLIB_$(HAVE_ThorsSQL))

include $(THORSANVIL_ROOT)/build/tools/Makefile



