AC_INIT([hops], [1.0.0], [tpw@google.com])

AM_INIT_AUTOMAKE([foreign no-define no-dist])

AC_PROG_CXX
AC_LANG([C++])

AC_HEADER_STDBOOL
AC_CHECK_FUNCS([pow])
AC_CHECK_HEADERS([glog/logging.h], [],
                 AC_MSG_ERROR([google-glog is required]))
AC_CHECK_HEADERS([gflags/gflags.h], [],
                 AC_MSG_ERROR([google-gflags is required]))
AC_CHECK_HEADERS([google/protobuf/text_format.h], [],
                 AC_MSG_ERROR([protobuf is required]))
AC_PATH_PROG(PROTOC, protoc,
             [AC_MSG_ERROR([protoc was not found in your path])])

AM_CONDITIONAL(GCC, test "$GCC" = yes)

AC_CONFIG_FILES([Makefile])
AC_CONFIG_HEADERS([config.h])

AC_OUTPUT
