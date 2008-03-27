#
# Run autotools for udis86
#
touch configure.ac

${BUILD_PFX}aclocal
${BUILD_PFX}libtoolize --force --copy
${BUILD_PFX}autoheader
${BUILD_PFX}automake --add-missing --copy --gnu --foreign
${BUILD_PFX}autoconf
