
AC_INIT([ThorsNisse], [0.1], [Loki.Astari+ThorsAnvil@gmail.com])
AC_PREREQ([2.65])

# Set up the build system
if [[ ! -e build/Notes ]]; then
    git submodule init
    git submodule update
fi
AC_CONFIG_MACRO_DIR([build/autotools/m4])
AC_CONFIG_AUX_DIR([build/autotools/build])
AX_THOR_FUNC_BUILD

AC_CONFIG_SRCDIR([src/Core/Service/Server.cpp])
AC_PROG_CXX
AX_THOR_PROG_COV

AX_BOOST_BASE([1.56], [], [AC_MSG_ERROR([Nisse uses boost Please install it.])])
AX_BOOST_SYSTEM
AX_BOOST_COROUTINE
CXXFLAGS="${CXXFLAGS} -std=c++11"
AX_BOOST_CONTEXT
AX_BOOST_THREAD
AX_THOR_FUNC_TEST_BOOST_COROUTINE_VERSION

AX_THOR_FUNC_USE_EVENT
AX_THOR_LIB_SELECT
AX_THOR_STATIC_LOAD_CHECK

LT_INIT

AX_CHECK_COMPILE_FLAG([-ansi], [AC_SUBST([CXXFLAGS], ["$CXXFLAGS -ansi"])])

AX_THOR_FUNC_LANG_FLAG(17)
AX_THOR_FUNC_USE_THORS_LIB_SQL(17)
AX_THOR_FUC_USE_THORS_LIB_SERIALIZE(17)

AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile.extra Makefile.config:build/autotools/build/Makefile.config.in:Makefile.config.in src/Core/Service/CoreServiceConfig.h:LibConfig.h.in])
AC_OUTPUT
