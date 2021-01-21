//==============================================================================
namespace
{
    static const char* aws_secure_N =
        "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1"
        "29024E088A67CC74020BBEA63B139B22514A08798E3404DD"
        "EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245"
        "E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"
        "EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE45B3D"
        "C2007CB8A163BF0598DA48361C55D39A69163FA8FD24CF5F"
        "83655D23DCA3AD961C62F356208552BB9ED529077096966D"
        "670C354E4ABC9804F1746C08CA18217C32905E462E36CE3B"
        "E39E772C180E86039B2783A2EC07A28FB5C55DF06F4C52C9"
        "DE2BCBF6955817183995497CEA956AE515D2261898FA0510"
        "15728E5A8AAAC42DAD33170D04507A33A85521ABDF1CBA64"
        "ECFB850458DBEF0A8AEA71575D060C7DB3970F85A6E1E4C7"
        "ABF5AE8CDB0933D71E8C94E04A25619DCEE3D2261AD2EE6B"
        "F12FFA06D98A0864D87602733EC86A64521F2B18177B200C"
        "BBE117577A615D6C770988C0BAD946E208E24FA074E5AB31"
        "43DB5BFCE0FD108E4B82D120A93AD2CAFFFFFFFFFFFFFFFF";
}

//==============================================================================
AWSUserAuthentication::AWSUserAuthentication (const String& username_, const String& password_,
                                              const String& userPool_, const String& clientId_,
                                              const String& regionId_) :
    username (username_),
    password (password_),
    userPool (userPool_),
    clientId (clientId_),
    regionId (regionId_)
{
}

AWSUserAuthentication::~AWSUserAuthentication()
{
    BN_free (bn_N_);
    BN_free (bn_g_);
    BN_free (bn_k_);
    BN_free (bn_random_);
    BN_free (bn_a_);
    BN_free (bn_A_);
}

void AWSUserAuthentication::generateSRPAValues()
{
    BN_rand (bn_random_, 256, 1, 1); // 256-bit random word
    BN_hex2bn (&bn_N_, aws_secure_N);
    BN_hex2bn (&bn_g_, "2");

    auto ng_block = convertHexStringToCharVector (String ("00") + aws_secure_N + "02");

    // NB: k actually seems to be fixed, shouldn't really need to regenerate all the time
    auto k_vector = digestMessage (ng_block);
    BN_bin2bn ((const uint8*) k_vector.data(), k_vector.size(), bn_k_);

    auto* bn_ctx = BN_CTX_new();
    BN_mod (bn_a_, bn_random_, bn_N_, bn_ctx);

    // r = (a ^ b) mod p
    BN_mod_exp(bn_A_, bn_g_, bn_a_, bn_N_, bn_ctx);

    srpa = BN_bn2hex(bn_a_);
    srpA = BN_bn2hex(bn_A_);

    // TODO add a do while loop here for safety, unlikely scenerio of A.Mod(N).Equals(0));

    BN_CTX_free (bn_ctx);
}

Array<int8> AWSUserAuthentication::getPasswordAuthenticationKey (const String &salt,
                                                                 const String &srp_b)
{
    // generate the key required for hkdf, TODO can probably remove the copy here and create the concatanated string first
    auto srpApad = padHexStringWithLeadingZero (srpA);
    auto srp_b_string_pad = padHexStringWithLeadingZero (srp_b); // can probably just add the hex strings... and convert to char
    auto srp_A_srp_b = srpApad + srp_b_string_pad;

    auto array_srp_A_srp_b = convertHexStringToCharVector (srp_A_srp_b);
    auto u_digest = digestMessage (array_srp_A_srp_b);
    u_digest = padCharVector (u_digest);

    // TODO check for digest == zero

    auto userid_digest = digestMessage (userPool + username + ":" + password);

    auto salt_array= convertHexStringToCharVector (salt);

    Array<int8> x_array;
    x_array.resize (salt_array.size() + userid_digest.size());

    std::copy (salt_array.begin(), salt_array.end(), x_array.begin());
    std::copy (userid_digest.begin(), userid_digest.end(), x_array.begin() + salt_array.size());

    auto x_digest = digestMessage (x_array);
    x_digest = padCharVector (x_digest);

    auto* bn_x = BN_new();
    auto* bn_u = BN_new();
    auto* bn_B = BN_new();

    BN_bin2bn ((const uint8*) x_digest.data(), x_digest.size(), bn_x);
    BN_bin2bn ((const uint8*) u_digest.data(), u_digest.size(), bn_u);
    BN_hex2bn (&bn_B, srp_b.toRawUTF8());

    auto* bn_g_mod_xn = BN_new();
    auto* bn_k_mult = BN_new();
    auto* bn_b_sub = BN_new();
    auto* bn_u_x = BN_new();
    auto* bn_a_add = BN_new();
    auto* bn_b_sub_modpow = BN_new();
    auto* bn_S = BN_new();

    // now do the math...
    auto* bn_ctx = BN_CTX_new();
    BN_div (nullptr, bn_a_, bn_random_, bn_N_, bn_ctx);

    BN_mod_exp (bn_g_mod_xn, bn_g_, bn_x, bn_N_, bn_ctx);
    BN_mul (bn_k_mult, bn_k_, bn_g_mod_xn, bn_ctx);
    BN_sub (bn_b_sub, bn_B, bn_k_mult);

    BN_mul (bn_u_x, bn_u, bn_x, bn_ctx);
    BN_add (bn_a_add, bn_a_, bn_u_x);
    BN_mod_exp (bn_b_sub_modpow, bn_b_sub, bn_a_add, bn_N_, bn_ctx);

    BN_div (nullptr, bn_S, bn_b_sub_modpow, bn_N_, bn_ctx);

    auto s_char = convertHexStringToCharVector ({ BN_bn2hex (bn_S) });
    s_char = padCharVector (s_char);

    Array<int8> dervied_key_info;
    for (auto v : "Caldera Derived Key")
        dervied_key_info.add (v);

    // now get the hashed key
    auto key = HKDF (u_digest, s_char, dervied_key_info);

    BN_free (bn_x);
    BN_free (bn_u);
    BN_free (bn_B);
    BN_free (bn_g_mod_xn);
    BN_free (bn_k_mult);
    BN_free (bn_b_sub);
    BN_free (bn_u_x);
    BN_free (bn_a_add);
    BN_free (bn_b_sub_modpow);
    BN_free (bn_S);

    BN_CTX_free (bn_ctx);

    //Debug information, remove when confident this is all working as required
    //std::cout << "u_content: " << BinaryToHexString(content.data(), content.size() * 2) << std::endl;
    //std::cout << "u_digest: " << BinaryToHexString(u_digest.data(), u_digest.size() * 2) << std::endl;
    //std::cout << "s_char: " << BinaryToHexString(s_char.data(), s_char.size() * 2) << std::endl;
    //std::cout << "dervied_key_info: " << BinaryToHexString(dervied_key_info.data(), dervied_key_info.size() * 2) << std::endl;

    return key;
}

inline void copyStringToArray (String::CharPointerType first,
                               String::CharPointerType last,
                               int8* destFirst)
{
    while (first != last)
        *destFirst++ = static_cast<int8> (*first++);
}

Aws::String AWSUserAuthentication::generateChallengeParameters (const String &salt,
                                                                const String &srp_b,
                                                                const String &secret_block,
                                                                const String &time_str)
{
    // convert the secret block from base64 into hex
    auto secret_block_char = base64Decode (secret_block.toRawUTF8());

    auto key = getPasswordAuthenticationKey (salt, srp_b);

    // hmac
    Array<int8> content;

    content.resize (userPool.length() + username.length() + secret_block_char.size() + time_str.length());

    copyStringToArray (userPool.begin(), userPool.end(),
                       content.begin());

    copyStringToArray (username.begin(), username.end(),
                       content.begin() + userPool.length());

    std::copy (secret_block_char.begin(), secret_block_char.end(),
               content.begin() + userPool.length() + username.length());

    copyStringToArray (time_str.begin(), time_str.end(),
                       content.begin() + userPool.length() + username.length() + secret_block_char.size());

    Array<int8> hmac; // sha256 will make produce a 32 byte array
    hmac.resize (32);

    HMAC (EVP_sha256(),
          key.data(),
          key.size(),
          (const unsigned char*) content.data(),
          (size_t) content.size(),
          (unsigned char*) hmac.data(),
          nullptr);

    // now base64 encode the hmac to create the claim to return to aws
    auto claim = base64Encode (hmac);

    //DEBUG information, leave here in case errors need to be tracked down
    //std::cout << "secbloc: " << BinaryToHexString(secret_block_char.size(), secret_block_char.size() * 2) << std::endl;
    //std::cout << "key: " << BinaryToHexString(key.size(), key.size() * 2) << std::endl;
    //std::cout << "content: " << BinaryToHexString(content.size(), content.size() * 2) << std::endl;
    //std::cout << "hmac: " << BinaryToHexString(hmac.size(), hmac.size() * 2) << std::endl;
    //std::cout << "claim : " << claim << std::endl;

    return toAWSString (claim);
}

bool AWSUserAuthentication::initiateAuthentication()
{
    Aws::SDKOptions options;
    Aws::InitAPI (options);

    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.region = toAWSString (regionId);

    Aws::Map<Aws::String, Aws::String> request_parameters;
    request_parameters["USERNAME"] = toAWSString (username);
    request_parameters["SRP_A"] = toAWSString (srpA);

    client = std::make_shared<Aws::CognitoIdentityProvider::CognitoIdentityProviderClient> (clientConfig);
    Aws::CognitoIdentityProvider::Model::InitiateAuthRequest auth_request;
    auth_request.SetClientId (toAWSString (clientId));
    auth_request.SetAuthFlow(Aws::CognitoIdentityProvider::Model::AuthFlowType::USER_SRP_AUTH);
    auth_request.SetAuthParameters(request_parameters);

    auto authRequestResult = client->InitiateAuth (auth_request);
    if (! authRequestResult.IsSuccess())
    {
        std::cout << "Failed to init the authenticate the user" << std::endl;
        std::cout << "Request error: " << authRequestResult.GetError().GetExceptionName() << " " << authRequestResult.GetError().GetMessage() << std::endl;
        return false;
    }

    // need time in the following format - Mon Jan 8 09:51:39 UTC 2018
    const auto now = std::chrono::system_clock::now();
    const auto now_c = std::chrono::system_clock::to_time_t (now);

    struct tm timeinfo;
    zerostruct (timeinfo);
    localtime_s (&timeinfo, &now_c); // using thread-safe localtime_s over localtime

    std::stringstream ss;
    ss << std::put_time (&timeinfo, "%a %b %e %H:%M:%S UTC %Y");

    const auto challengeParams      = authRequestResult.GetResult().GetChallengeParameters();

#if 0
    const auto salt&                = challengeParams.operator[] ("SALT");
    const auto srp_b&               = challengeParams["SRP_B"];
    const auto secret_block&        = challengeParams["SECRET_BLOCK"];
    const auto time_str             = ss.str();
    const auto claim64              = generateChallengeParameters (salt, srp_b, secret_block, time_str);

    // now set the return to the challenge
    Aws::CognitoIdentityProvider::Model::RespondToAuthChallengeRequest request;
    request.SetClientId(clientId);
    request.SetChallengeName(authRequestResult.GetResult().GetChallengeName());
    request.AddChallengeResponses ("PASSWORD_CLAIM_SECRET_BLOCK", secret_block);
    request.AddChallengeResponses ("PASSWORD_CLAIM_SIGNATURE", claim64);
    request.AddChallengeResponses ("USERNAME", username);
    request.AddChallengeResponses ("TIMESTAMP", time_str.c_str());

    const auto r = client->RespondToAuthChallenge (request);
    if (! r.IsSuccess())
    {
        std::cout << "Failed to respond to the challenge" << std::endl;
        std::cout << "Request error: " << request.GetError().GetExceptionName() << " " << r.GetError().GetMessage() << std::endl;
        return false;
    }

    std::cout << "User logged in!" << std::endl;
    std::cout << "The token can now be exchanged for AWS Credentials" << std::endl;
    std::cout << "The ID Token: " << request.GetResult().GetAuthenticationResult().GetIdToken() << std::endl;
#endif

    Aws::ShutdownAPI (options);
    return true;
}
