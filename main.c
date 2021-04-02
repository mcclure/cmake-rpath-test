#include <stdio.h>
#include "phonon.h"

// Late-linking machinery
#ifdef _WIN32
#include <windows.h>
#define PHONON_LIBRARY "phonon.dll"
static void* phonon_dlopen(const char* library) { return LoadLibraryA(library); }
static FARPROC phonon_dlsym(void* library, const char* symbol) { return GetProcAddress(library, symbol); }
static void phonon_dlclose(void* library) { FreeLibrary(library); }
#else
#include <dlfcn.h>
#if __APPLE__
#define PHONON_LIBRARY "libphonon.dylib"
#else
#define PHONON_LIBRARY "libphonon.so"
#endif
static void* phonon_dlopen(const char* library) { return dlopen(library, RTLD_NOW | RTLD_LOCAL); }
static void* phonon_dlsym(void* library, const char* symbol) { return dlsym(library, symbol); }
static void phonon_dlclose(void* library) { dlclose(library); }
#endif

typedef IPLerror fn_iplCreateContext(IPLLogFunction logCallback, IPLAllocateFunction allocateCallback, IPLFreeFunction freeCallback, IPLhandle* context);
#define PHONON_DECLARE(f) static fn_##f* phonon_##f;
#define PHONON_LOAD(f) phonon_##f = (fn_##f*) phonon_dlsym(library, #f);
#define PHONON_FOREACH(X)\
  X(iplCreateContext)

PHONON_FOREACH(PHONON_DECLARE)

int main() {
    // Load CreateContext function
    void* library = phonon_dlopen(PHONON_LIBRARY);
    if (!library) {
        printf("Failed to load Steam Audio\n");
        return 1;
    }
    PHONON_FOREACH(PHONON_LOAD)

    // Execute CreateContext function
    IPLhandle phononContext;
    IPLerror status;
    status = phonon_iplCreateContext(NULL, NULL, NULL, &phononContext);
    if (status != IPL_STATUS_SUCCESS) {
      printf( "Failed to initialize Steam Audio\n" );
      return 1;
    }
    printf( "Initialized Steam Audio\n" );

	return 0;
}
