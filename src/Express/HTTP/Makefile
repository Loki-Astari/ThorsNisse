
THORSANVIL_ROOT				?= $(realpath ../../../)
COVERAGE_REQUIRED			= 40
DYLD_LIBRARY_PATH			= $(shell pwd)

PRETEST						= TestLoadableLibBuild
TARGET						= ThorsNisseExpressHTTP.defer
DEFER_NAME					= ThorsNisseExpress
LDLIBS_EXTERN_BUILD			+= event boost ThorSerialize ThorsDB

LDLIBS						+= -ldl
LINK_LIBS					+= ThorsNisseCore

%/DeveloperHandler.o:		FILE_WARNING_FLAGS=-Wno-unused-private-field

include $(THORSANVIL_ROOT)/build/tools/Makefile

.PHONEY:	TestLoadableLibBuild
TestLoadableLibBuild:
	@make -C TestLib/Loadable all
	@make -C TestLib/NoGet    all
	@make -C TestLib/GetNull  all

