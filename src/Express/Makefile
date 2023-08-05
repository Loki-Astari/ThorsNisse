
THORSANVIL_ROOT				?= $(realpath ../../)

TARGET						= ThorsExpress.slib
LDLIBS_EXTERN_BUILD			+= event boost ThorSerialize ThorsDB
LDLIBS						+= -ldl

LINK_SLIBS					+= ThorsNisseProtocolHTTP ThorsNisseCoreService ThorsNisseCoreSocket ThorsNisseCoreSQL

include $(THORSANVIL_ROOT)/build/tools/Makefile

