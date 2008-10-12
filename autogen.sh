#!/bin/sh
if ! type libtoolize >/dev/null 2>&1 && type glibtoolize >/dev/null 2>&1; then
  echo "$0: libtoolize not found, trying glibtoolize"
  export LIBTOOLIZE=glibtoolize
fi
cd "`dirname "$0"`"
autoreconf --force -v --install
