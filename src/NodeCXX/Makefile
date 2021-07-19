
THORSANVIL_ROOT				?= $(realpath ../../)

TARGET						= NodeCXX.app
LDLIBS_EXTERN_BUILD			+= event boost ThorsSerialize ThorsDB
LDLIBS						+= -ldl
LINK_LIBS					+= ThorsExpress

include $(THORSANVIL_ROOT)/build/tools/Makefile

