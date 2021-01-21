#include "squarepine_aws.h"

#if JUCE_WINDOWS
   #if ! _MT
    #error "Sorry, not having MT is not supported with this module!"
   #endif

    /** If the app doesn't build because it can't find the OpenSSL libs,
        you simply need to install OpenSSL.

        And if the app can't find the AWS libs, you need to tell your project's libraries path
        about the aws-sdk-cpp folder! And you will need to build the libraries using the provided
        AWS .sln for MSVC, which is located at the root of the aws-sdk-cpp repository.
        You'll need to use cmake (ie: 'cmake .') to generate the MSVC solution and subsequent projects.
    */

    #if _MT
        #if JUCE_64BIT
            #if JUCE_DEBUG
                #pragma comment (lib, "C:/Program Files/OpenSSL-Win64/lib/VC/libcrypto64MTd.lib")
                #pragma comment (lib, "C:/Program Files/OpenSSL-Win64/lib/VC/libssl64MDd.lib")

                #pragma comment (lib, "aws-cpp-sdk-cognito-identity/Debug/aws-cpp-sdk-cognito-identity.lib")
                #pragma comment (lib, "aws-cpp-sdk-cognito-idp/Debug/aws-cpp-sdk-cognito-idp.lib")
                #pragma comment (lib, "aws-cpp-sdk-cognito-sync/Debug/aws-cpp-sdk-cognito-sync.lib")
                #pragma comment (lib, "aws-cpp-sdk-core/Debug/aws-cpp-sdk-core.lib")
            #else
                #pragma comment (lib, "C:/Program Files/OpenSSL-Win64/lib/VC/libcrypto64MT.lib")
                #pragma comment (lib, "C:/Program Files/OpenSSL-Win64/lib/VC/libssl64MD.lib")

                #pragma comment (lib, "aws-cpp-sdk-cognito-identity/Release/aws-cpp-sdk-cognito-identity.lib")
                #pragma comment (lib, "aws-cpp-sdk-cognito-idp/Release/aws-cpp-sdk-cognito-idp.lib")
                #pragma comment (lib, "aws-cpp-sdk-cognito-sync/Release/aws-cpp-sdk-cognito-sync.lib")
                #pragma comment (lib, "aws-cpp-sdk-core/Release/aws-cpp-sdk-core.lib")
            #endif
        #else
            #if JUCE_DEBUG
                #pragma comment (lib, "C:/Program Files (x86)/OpenSSL-Win32/lib/VC/libcrypto32MTd.lib")
                #pragma comment (lib, "C:/Program Files (x86)/OpenSSL-Win32/lib/VC/libssl32MTd.lib")

                //#pragma comment (lib, "aws-cpp-sdk-cognito-identity/Debug/aws-cpp-sdk-cognito-identity.lib")
                //#pragma comment (lib, "aws-cpp-sdk-cognito-idp/Debug/aws-cpp-sdk-cognito-idp.lib")
                //#pragma comment (lib, "aws-cpp-sdk-cognito-sync/Debug/aws-cpp-sdk-cognito-sync.lib")
                //#pragma comment (lib, "aws-cpp-sdk-core/Debug/aws-cpp-sdk-core.lib")

                #pragma comment (lib, "I:/Work/SquarePine/JUCEAWSExample/submodules/aws-sdk-cpp/aws-cpp-sdk-core/Debug/aws-cpp-sdk-core.lib")
            #else
                #pragma comment (lib, "C:/Program Files (x86)/OpenSSL-Win32/lib/VC/libcrypto32MTd.lib")
                #pragma comment (lib, "C:/Program Files (x86)/OpenSSL-Win32/lib/VC/libcrypto32MTd.lib")

                #pragma comment (lib, "aws-cpp-sdk-cognito-identity/Release/aws-cpp-sdk-cognito-identity.lib")
                #pragma comment (lib, "aws-cpp-sdk-cognito-idp/Release/aws-cpp-sdk-cognito-idp.lib")
                #pragma comment (lib, "aws-cpp-sdk-cognito-sync/Release/aws-cpp-sdk-cognito-sync.lib")
                #pragma comment (lib, "aws-cpp-sdk-core/Release/aws-cpp-sdk-core.lib")
            #endif
        #endif
    #endif
#endif

namespace sp
{
    using namespace juce;

    #include "core/AWSAuthentication.cpp"
}
