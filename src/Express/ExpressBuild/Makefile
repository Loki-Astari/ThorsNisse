
THORSANVIL_ROOT				?= $(realpath ../../../)

TARGET						= ThorsNisseExpress.lib
DEFER_LIBS					= ThorsNisseExpressSimple ThorsNisseExpressHTTP ThorsNisseExpressAddSitea
LINK_LIBS					+= ThorsNisseCore
LDLIBS_EXTERN_BUILD			+= boost ThorSerialize ThorsDB
LINK_LIBS					+= ThorsSocket

include $(THORSANVIL_ROOT)/build/tools/Makefile

