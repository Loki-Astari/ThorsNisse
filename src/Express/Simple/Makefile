
THORSANVIL_ROOT				?= $(realpath ../../../)

TARGET						= ThorsNisseExpressSimple.defer
DEFER_NAME					= ThorsNisseExpress
LDLIBS_EXTERN_BUILD			+= event boost dl ThorsDB ThorSerialize
LINK_LIBS					+= ThorsNisseCore ThorsSocket

include $(THORSANVIL_ROOT)/build/tools/Makefile

