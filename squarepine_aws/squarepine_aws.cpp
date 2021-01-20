#include "squarepine_aws.h"

#if JUCE_WINDOWS
    #if JUCE_64BIT
        #if JUCE_DEBUG
            #if _MT
                #pragma comment (lib, "C:/Program Files/OpenSSL-Win64/lib/VC/libcrypto64MTd.lib")
                #pragma comment (lib, "C:/Program Files/OpenSSL-Win64/lib/VC/libssl64MDd.lib")
            #else
                #error "Stop doing that."
            #endif
        #else
            #if _MT
                #pragma comment (lib, "C:/Program Files/OpenSSL-Win64/lib/VC/libcrypto64MT.lib")
                #pragma comment (lib, "C:/Program Files/OpenSSL-Win64/lib/VC/libssl64MD.lib")
            #else
                #error "Stop doing that."
            #endif
        #endif
    #else
        #if JUCE_DEBUG
            #if _MT
                #pragma comment (lib, "C:/Program Files (x86)/OpenSSL-Win32/lib/VC/libcrypto32MTd.lib")
                #pragma comment (lib, "C:/Program Files (x86)/OpenSSL-Win32/lib/VC/libcrypto32MTd.lib")
            #else
                #error "Stop doing that."
            #endif
        #else
            #if _MT
                #pragma comment (lib, "C:/Program Files (x86)/OpenSSL-Win32/lib/VC/libcrypto32MTd.lib")
                #pragma comment (lib, "C:/Program Files (x86)/OpenSSL-Win32/lib/VC/libcrypto32MTd.lib")
            #else
                #error "Stop doing that."
            #endif
        #endif
    #endif
#endif

namespace sp
{
    using namespace juce;
}
