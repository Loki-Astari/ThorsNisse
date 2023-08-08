
THORSANVIL_ROOT				?= $(realpath ../../../)

TARGET						= ThorsNisseCore.lib
DEFER_LIBS					= ThorsNisseCoreSocket ThorsNisseCoreService ThorsNisseCoreSQL
LDLIBS_EXTERN_BUILD			+= crypto event boost ThorsDB ThorSerialize

include $(THORSANVIL_ROOT)/build/tools/Makefile

