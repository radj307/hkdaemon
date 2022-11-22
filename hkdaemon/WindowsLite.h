#pragma once
/**
 * @file	WindowsLite.h
 * @author	radj307
 * @brief	Includes the <Windows.h> header after disabling most of it via preprocessor definitions.
 */

#ifdef _WINDOWS_
#error The WIN32 API was included prior to the inclusion of <WindowsLite.h> - this header is not functioning!
#endif

#define WIN32_LEAN_AND_MEAN
// Specific macros mentioned in <Windows.h>:
#define NOGDICAPMASKS
#define NOWINSTYLES
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOSHOWWINDOW
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOGDI
#define NOKERNEL
#define NONLS
#define NOMB
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX

#include <Windows.h>
