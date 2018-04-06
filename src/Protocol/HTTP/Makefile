
THORSANVIL_ROOT				?= $(realpath ../../../)

HAVE_DEVLOADER_yes			= -DHAVE_DEVLOADER
HAVE_DEVLOADER				= $(HAVE_DEVLOADER_$(HAVE_ThorsSerialize))
THOR_SERIALIZER_NEEDED_yes	= ThorSerialize
THOR_SERIALIZER_NEEDED		= $(THOR_SERIALIZER_NEEDED_$(HAVE_ThorsSerialize))

PRETEST						= TestLoadableLibBuild
TARGET						= ThorsNisseProtocolHTTP.a
CXXFLAGS					+= -I$(event_ROOT_DIR)/include $(thorsSerialize_ROOT_INCDIR) $(HAVE_DEVLOADER) -Wno-ignored-attributes
LDLIBS						+= -L$(event_ROOT_DIR)/lib -l$(event_ROOT_LIB) $(thorsSerialize_ROOT_LIBDIR) $(BOOST_SYSTEM_LIB) $(BOOST_COROUTINE_LIB) $(BOOST_CONTEXT_LIB) -ldl
LINK_LIBS					= ThorsNisseCoreService ThorsNisseCoreSocket ThorsSerialize
TEST_IGNORE					= http_parser.cpp http_parser.h
UNITTEST_LDLIBS				= -ldl $(thorsSerialize_ROOT_LIBDIR)
UNITTEST_LINK_LIBS			= ThorsNisseCoreService ThorsNisseCoreSocket $(THOR_SERIALIZER_NEEDED)

IGNORE_BOOST_ISSUES_2	= -Wno-extra -Wno-terminate
IGNORE_BOOST_ISSUES		= $(IGNORE_BOOST_ISSUES_$(BOOST_COROUTINE_VERSION))

FILE_WARNING_FLAGS	+= -Wno-unknown-pragmas
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



