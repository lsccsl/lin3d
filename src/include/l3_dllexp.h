#ifndef __L3_DLLEXP_DEFINE__
#define __L3_DLLEXP_DEFINE__

#ifdef LIN3D_STATIC
	#define LIN3D_EX_PORT
#else
	#ifdef LIN3D_DLL_EXPORTS
		#define LIN3D_EX_PORT __declspec(dllexport)
	#else
		#define LIN3D_EX_PORT __declspec(dllimport)
	#endif
#endif

#endif