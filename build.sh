#!/bin/bash
exec clang --target=wasm32-wasip1 --sysroot=/tmp/claude-0/sysroot -O2 -DM_CORE_GBA -DENABLE_VFS -DENABLE_VFS_FD -DDISABLE_THREADING -DMINIMAL_CORE=2 -DHAVE_STRDUP -DHAVE_STRLCPY -DHAVE_LOCALTIME_R -DHAVE_LOCALE -DHAVE_NEWLOCALE -DHAVE_USELOCALE -DHAVE_FREELOCALE -I /tmp/claude-0/wbuild/include -I /tmp/claude-0/mgba-celio-web/include -I /tmp/claude-0/mgba-celio-web/src "$@"
