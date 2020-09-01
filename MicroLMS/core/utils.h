/* INTEL CONFIDENTIAL
 * Copyright 2011 - 2019 Intel Corporation.
 * This software and the related documents are Intel copyrighted materials, and your use of them is governed by the express license under which they were provided to you ("License"). Unless the License provides otherwise, you may not use, modify, copy, publish, distribute, disclose or transmit this software or the related documents without Intel's prior written permission.
 * This software and the related documents are provided as is, with no express or implied warranties, other than those that are expressly stated in the License.
 */

#if !defined(__MeshUtils__)
#define __MeshUtils__

#include "../microstack/ILibParsers.h"

#if !defined(WIN32) 
#define __fastcall
#endif

// Debugging features
#if defined(_DEBUG)
	// Display & log
    //char spareDebugMemory[4000];
    //int  spareDebugLen;
    //#define MSG(x) printf("%s",x);//mdb_addevent(x, (int)strlen(x));
	//#define MSG(...) spareDebugLen = snprintf(spareDebugMemory, 4000, __VA_ARGS__);printf("%s",spareDebugMemory);//mdb_addevent(spareDebugMemory, spareDebugLen);

	// Display only
	#define MSG(...)  printf(__VA_ARGS__); fflush(NULL)
	#define DEBUGSTATEMENT(x) x
#else
	#ifndef MSG
		#define MSG(...)
	#endif
	#define DEBUGSTATEMENT(x)
#endif

char* __fastcall util_tohex_lower(char* data, int len, char* out);

#ifdef WIN32
int   __fastcall util_crc(unsigned char *buffer, int len, int initial_value);
BOOL util_MoveFile(_In_ LPCSTR lpExistingFileName, _In_  LPCSTR lpNewFileName);
BOOL util_CopyFile(_In_ LPCSTR lpExistingFileName, _In_ LPCSTR lpNewFileName, _In_ BOOL bFailIfExists);
BOOL GetWindowsDllPath(const char* dllName, wchar_t* out);
#endif

#endif
