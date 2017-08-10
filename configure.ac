
AC_INIT([ThorsAnvil], [0.1], [Loki.Astari+ThorsAnvil@gmail.com])
AC_PREREQ([2.65])

# Set up the build system
AC_CONFIG_MACRO_DIR([build/autotools/m4])
AC_CONFIG_AUX_DIR([build/autotools/build])
AX_THOR_FUNC_BUILD

AX_THOR_BUILD_ON_TRAVIS_OPTION

AX_BOOST_BASE([1.54], [], [AC_MSG_ERROR([Nisse uses boost Please install it.])])
AX_BOOST_SYSTEM
AX_BOOST_COROUTINE
CXXFLAGS="${CXXFLAGS} -std=c++11"
AX_BOOST_CONTEXT
AX_THOR_FUNC_TEST_BOOST_COROUTINE_VERSION

AX_THOR_FUNC_USE_EVENT

AX_THOR_FUNC_BUILD

AC_CONFIG_SRCDIR([src/Nisse/NisseService.cpp])
AC_PROG_CXX
AX_THOR_PROG_COV
LT_INIT([disable-static])

AX_THOR_LIB_SELECT

AX_CHECK_COMPILE_FLAG([-ansi], [AC_SUBST([CXXFLAGS], ["$CXXFLAGS -ansi"])])

AX_THOR_FUNC_LANG_FLAG(17)


AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_HEADERS([config.h src/Nisse/NisseConfig.h:config.h.in src/Imp/ImpConfig.h:config.h.in src/Troll/TrollConfig.h:config.h.in ])
AC_CONFIG_FILES([Makefile.extra Makefile.config:build/autotools/build/Makefile.config.in ])
AC_OUTPUT
