#ifndef __DEFINES_H__
#define __DEFINES_H__


#ifdef _WIN32
#ifdef _WINDLL
#define __VSNC_P2P_PORT __declspec(dllexport)
#else
#define __VSNC_P2P_PORT __declspec(dllimport)
#endif // WIN32_DLL
#else
#define __VSNC_P2P_PORT
#endif // _WIN32


#endif // !__DEFINES_H__

