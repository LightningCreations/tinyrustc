AC_INIT([install-sh])

AC_CONFIG_MACRO_DIR([m4/])

AC_PROG_INSTALL
AC_PROG_MKDIR_P
AC_PROG_LN_S
AC_PROG_SED

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET


if [[ "$build_alias" != "" ]]
then
    build_noncanonical="$build_alias"
else
    build_noncanonical="$build"
fi

if [[ "$host_alias" != "" ]]
then
    host_noncanonical="$host_alias"
else
    host_noncanonical="$build_noncanonical"
fi


if [[ "$target_alias" != "" ]]
then
    target_noncanonical="$target_alias"
else
    target_noncanonical="$host_noncanonical"
fi


AC_PROG_CC([lccc clang gcc cc])


AC_MSG_CHECKING([whether $CC supports C11])

AX_CHECK_COMPILE_FLAG([-std=c11],[
    AC_MSG_RESULT([yes])
],[
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([$CC does not support required C11])
])

AC_SYS_LARGEFILE
AC_PROG_CXX([lc++ clang++ g++ c++])

AC_LANG_PUSH([C++])

AX_CHECK_COMPILE_FLAG([-std=c++20],[
    CXXFLAGS="$CXXFLAGS"
],[
    AC_MSG_ERROR([$CXX does not support required C++20])
])

AC_LANG_POP()

AC_SUBST([build_noncanonical])
AC_SUBST([host_noncanonical])
AC_SUBST([target_noncanonical])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
