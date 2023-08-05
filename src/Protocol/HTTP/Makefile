
THORSANVIL_ROOT				?= $(realpath ../../../)
COVERAGE_REQUIRED			= 40
DYLD_LIBRARY_PATH			= $(shell pwd)

PRETEST						= TestLoadableLibBuild
TARGET						= ThorsNisseProtocolHTTP.a
LDLIBS_EXTERN_BUILD			+= event boost ThorSerialize

LDLIBS						+= -ldl
LINK_LIBS					+= ThorsNisseCoreService ThorsNisseCoreSocket

%/DeveloperHandler.o:		FILE_WARNING_FLAGS=-Wno-unused-private-field

include $(THORSANVIL_ROOT)/build/tools/Makefile

.PHONEY:	TestLoadableLibBuild
TestLoadableLibBuild:
	@make -C TestLib/Loadable all
	@make -C TestLib/NoGet    all
	@make -C TestLib/GetNull  all

