
THORSANVIL_ROOT				?= $(realpath ../../../)

TARGET						= ThorsNisseCore.lib
DEFER_LIBS					= ThorsNisseCoreService ThorsNisseCoreSQL
LDLIBS_EXTERN_BUILD			+= crypto event boost ThorsDB ThorSerialize
LINK_LIBS					+= ThorsSocket

include $(THORSANVIL_ROOT)/build/tools/Makefile

