
THORSANVIL_ROOT				?= $(realpath ../../../)

PRETEST						= TestLoadableLibBuild
TARGET						= ThorsNisseProtocolHTTP.a $(TARGET_$(TARGET_MODE))
CXXFLAGS					+= -I$(event_ROOT_DIR)/include
LDLIBS						+= -L$(event_ROOT_DIR)/lib -l$(event_ROOT_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_COROUTINE_LIB) $(BOOST_CONTEXT_LIB)
LINK_LIBS					= ThorsNisseCoreService ThorsNisseCoreSocket
TEST_IGNORE					= http_parser.cpp http_parser.h
UNITTEST_LDLIBS				= -ldl
UNITTEST_LINK_LIBS			= ThorsNisseCoreService ThorsNisseCoreSocket ThorSerialize

IGNORE_BOOST_ISSUES_2	= -Wno-extra -Wno-terminate
IGNORE_BOOST_ISSUES		= $(IGNORE_BOOST_ISSUES_$(BOOST_COROUTINE_VERSION))

%/HTTPProtocol.o:	FILE_WARNING_FLAGS	+= $(IGNORE_BOOST_ISSUES) 
%/HttpScanner.o:	FILE_WARNING_FLAGS	+= $(IGNORE_BOOST_ISSUES) 
%/Binder.o:			FILE_WARNING_FLAGS	+= $(IGNORE_BOOST_ISSUES)
%/Route.o:			FILE_WARNING_FLAGS	+= $(IGNORE_BOOST_ISSUES)
%/Types.o:			FILE_WARNING_FLAGS	+= $(IGNORE_BOOST_ISSUES)


include $(THORSANVIL_ROOT)/build/tools/Makefile

.PHONEY:	TestLoadableLibBuild
TestLoadableLibBuild:
	@make -C TestLib/Loadable all
	@make -C TestLib/NoGet    all
	@make -C TestLib/GetNull  all



