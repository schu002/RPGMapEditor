#ifndef commondef_h
#define commondef_h 1

#include "global.h"

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#if	L_OS_WINDOWS
#include <windows.h>
#include <io.h>
#include <errno.h>
#include <direct.h>
#include <sys/utime.h>
#endif

#include "commonpack.h"

#endif
