/*
	semaphore.h  
*/

#ifndef __NICK_COMMON__ 
#define __NICK_COMMON__ 


#if _MSC_VER > 1000
#pragma once
#endif


#include <stdio.h>
///////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
///////////////////////////////////////////////////////////////////////////////////
#include <windows.h>


#ifdef _MSC_VER
//forcing value to bool 'true' or 'false' (performance warning)
#pragma warning( disable : 4800 )
//identifier was truncated to '255' characters in the debug information
#pragma warning( disable : 4786 )
//used in base member initializer list
#pragma warning( disable : 4355 )
//needs to have dll-interface to be used by clients of class ...
#pragma warning( disable : 4251 )
//inherits ... via dominance
#pragma warning( disable : 4250 )
//non dll-interface class ... used as base for dll-interface class ...
#pragma warning( disable : 4275 )
//decorated name length exceeded, name was truncated
#pragma warning( disable : 4503 )  
#endif

///////////////////////////////////////////////////////////////////////////////////
#elif __IOS__
///////////////////////////////////////////////////////////////////////////////////
#include "ios_generic.h"


///////////////////////////////////////////////////////////////////////////////////
#else  //linux
///////////////////////////////////////////////////////////////////////////////////
typedef unsigned long       DWORD;
typedef int                 BOOL;

#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>		//mkdir, access

#endif


#endif