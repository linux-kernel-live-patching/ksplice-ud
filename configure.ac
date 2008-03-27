#
#
#AC_PREREQ(2.13)
AC_INIT([udis86], [1.6], [vivek@sig9.com])

# Determine the build host; we notify automake of Windows builds, so it can
# pass proper parameters for building DLLs to the linker.
AC_CANONICAL_HOST
case "$host_os" in
	mingw32*)
		TARGET_OS=windows
		AC_LIBTOOL_WIN32_DLL
		;;
esac

# Initialize the automake subsystem.
# In case we have a Windows build, we pass a TARGET_WINDOWS conditional to
# automake.
AM_INIT_AUTOMAKE
AM_CONDITIONAL(TARGET_WINDOWS, test "$TARGET_OS" = windows)

AC_PROG_CC
AC_PROG_LIBTOOL
AC_DISABLE_SHARED

# If this is a gnu compiler, pass -Wall
if test "$ac_cv_c_compiler_gnu" = "yes"; then
	CFLAGS="$CFLAGS -Wall"
fi

AC_CONFIG_HEADERS(config.h)
AC_CONFIG_FILES([
    Makefile
    libudis86/Makefile
    udcli/Makefile
     tests/Makefile
])
AC_OUTPUT
