#pragma once
#define WIN32_LEAN_AND_MEAN
#include <UIlib.h>
#ifdef _DEBUG
#pragma comment(lib, "DuiLib_d.lib")
#else
#pragma comment(lib, "DuiLib.lib")
#endif // _DEBUG

using namespace DuiLib;

//////////////////////////////////////////////////////////////////////////
// C++ header

#include <memory>

//////////////////////////////////////////////////////////////////////////
// boost header
