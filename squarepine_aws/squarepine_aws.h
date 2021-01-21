#ifndef SQUAREPINE_AWS_H
#define SQUAREPINE_AWS_H

/** BEGIN_JUCE_MODULE_DECLARATION

    ID:                 squarepine_aws
    vendor:             SquarePine
    version:            0.1.0
    name:               SquarePine AWS
    description:        A small set of AWS wrappers for JUCE projects.
    website:            https://www.squarepine.io
    license:            MIT
    minimumCppStandard: 11
    dependencies:       juce_data_structures

    END_JUCE_MODULE_DECLARATION
*/

//==============================================================================
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <locale>
#include <random>
#include <regex>
#include <sstream>

#include <juce_data_structures/juce_data_structures.h>

extern "C"
{
    #include <openssl/hmac.h>
    #include <openssl/kdf.h>
    #include <openssl/sha.h>
    #include <openssl/bio.h>
    #include <openssl/bn.h>
}

#include <aws/s3/S3Client.h> // this is required to resolve errors with the AWSError, shouldn't be required here
#include <aws/cognito-idp/CognitoIdentityProviderClient.h>
#include <aws/cognito-idp/model/InitiateAuthRequest.h>
#include <aws/cognito-idp/model/InitiateAuthResult.h>
#include <aws/cognito-idp/model/RespondToAuthChallengeRequest.h>
#include <aws/cognito-idp/model/RespondToAuthChallengeResult.h>
#include <aws/cognito-idp/model/AdminInitiateAuthRequest.h>
#include <aws/cognito-idp/model/SignUpRequest.h>
#include <aws/cognito-idp/model/ConfirmSignUpRequest.h>
#include <aws/cognito-idp/model/ListUsersRequest.h>
#include <aws/cognito-identity/model/GetIdRequest.h>
#include <aws/cognito-identity/model/GetOpenIdTokenRequest.h>
#include <aws/cognito-identity/model/GetCredentialsForIdentityRequest.h>
#include <aws/core/Aws.h>

//==============================================================================
namespace sp
{
    using namespace juce;

    #include "core/OpenSSLUtilities.h"
    #include "core/AWSAuthentication.h"
}

#endif //SQUAREPINE_AWS_H
