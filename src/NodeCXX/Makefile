
THORSANVIL_ROOT				?= $(realpath ../../)

TARGET						= NodeCXX.app
LDLIBS_EXTERN_BUILD			+= event boost ThorsSerialize ThorsDB
LDLIBS						+= -ldl
LINK_LIBS					+= ThorsNisseExpress ThorsNisseCore

include $(THORSANVIL_ROOT)/build/tools/Makefile

