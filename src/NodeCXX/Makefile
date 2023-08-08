
THORSANVIL_ROOT				?= $(realpath ../../)

TARGET						= NodeCXX.app
LDLIBS_EXTERN_BUILD			+= event boost ThorsSerialize ThorsDB
LDLIBS						+= -ldl
LINK_LIBS					+= ThorsNisseExpress ThorsNisseCore ThorsSocket

include $(THORSANVIL_ROOT)/build/tools/Makefile

