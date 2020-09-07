
THORSANVIL_ROOT				?= $(realpath ../../../)
COVERAGE_REQUIRED			= 40

PRETEST						= TestLoadableLibBuild
TARGET						= ThorsNisseProtocolHTTP.a
CXXFLAGS					+= $(HAVE_DEVLOADER) -Wno-ignored-attributes
LDLIBS						+= $(BOOST_SYSTEM_LIB) $(BOOST_COROUTINE_LIB) $(BOOST_CONTEXT_LIB) -ldl
LINK_LIBS					= ThorsNisseCoreService ThorsNisseCoreSocket ThorsSerialize
TEST_IGNORE					= http_parser.cpp http_parser.h
LDLIBS_EXTERN_BUILD			= event ThorsSerialize 
UNITTEST_LDLIBS				= -ldl $(thorsSerialize_ROOT_LIBDIR)
UNITTEST_LINK_LIBS			= ThorsNisseCoreService ThorsNisseCoreSocket $(THOR_SERIALIZER_NEEDED)

IGNORE_BOOST_ISSUES_2	= -Wno-extra -Wno-terminate
IGNORE_BOOST_ISSUES		= $(IGNORE_BOOST_ISSUES_$(BOOST_COROUTINE_VERSION))

UNITTEST_RUNTIME_PATH		= $(subst -L,,$(thorsSerialize_ROOT_LIBDIR))

FILE_WARNING_FLAGS	+= -Wno-unknown-pragmas
%/DeveloperHandler.o:	FILE_WARNING_FLAGS	+= -Wno-unused-private-field
%/HTTPProtocol.o:		FILE_WARNING_FLAGS	+= $(IGNORE_BOOST_ISSUES) 
%/HttpScanner.o:		FILE_WARNING_FLAGS	+= $(IGNORE_BOOST_ISSUES) 
%/Binder.o:				FILE_WARNING_FLAGS	+= $(IGNORE_BOOST_ISSUES)
%/Route.o:				FILE_WARNING_FLAGS	+= $(IGNORE_BOOST_ISSUES)
%/Types.o:				FILE_WARNING_FLAGS	+= $(IGNORE_BOOST_ISSUES)

include $(THORSANVIL_ROOT)/build/tools/Makefile

.PHONEY:	TestLoadableLibBuild
TestLoadableLibBuild:
	@make -C TestLib/Loadable all
	@make -C TestLib/NoGet    all
	@make -C TestLib/GetNull  all



