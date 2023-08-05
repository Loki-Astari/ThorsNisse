AC_INIT([ThorsNisse], [0.1], [Loki.Astari+ThorsAnvil@gmail.com])
AC_PREREQ([2.65])



AC_CONFIG_MACRO_DIR([build/autotools/m4])
AC_CONFIG_AUX_DIR([build/autotools/build])

AX_THOR_FUNC_INIT_BUILD([ThorsNisse], [src/NodeCXX/NodeCXX.cpp])


AX_THOR_CHECK_USE_BOOST([1.82], [], [AC_MSG_ERROR([Nisse uses boost Please install it.])])
AX_BOOST_SYSTEM
AX_BOOST_COROUTINE
AX_BOOST_CONTEXT
AX_BOOST_THREAD


AX_THOR_CHECK_USE_CRYPTO
AX_THOR_CHECK_USE_EVENT

#
# Non standard library flags
AX_THOR_CHECK_USE_STATIC_LOAD

AX_THOR_CHECK_USE_THORS_SERIALIZE
AX_THOR_CHECK_USE_THORS_DB

LT_INIT

AX_THOR_FUNC_TERM_BUILD([THORS_NISSE], [src/Core/Service/CoreServiceConfig.h])

AC_OUTPUT
