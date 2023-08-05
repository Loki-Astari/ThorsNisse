
THORSANVIL_ROOT				?= $(realpath ../../../)

TARGET						= ThorsNisseExpressAddSite.defer
COVERAGE_REQUIRED			= 0
DEFER_NAME					= ThorsNisseExpress
LDLIBS_EXTERN_BUILD			+= event boost ThorSerialize ThorsDB
LDLIBS						+= -ldl

UNITTEST_LINK_LIBS			+= UnitTestThorsNisseExpressHTTP

include $(THORSANVIL_ROOT)/build/tools/Makefile

