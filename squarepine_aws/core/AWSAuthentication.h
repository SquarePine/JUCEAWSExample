/** */
class AWSUserAuthentication final
{
public:
    /** */
    AWSUserAuthentication (const String& username, const String& password,
                           const String& userPool, const String& clientId,
                           const String& regionId);

    /** */
    ~AWSUserAuthentication();

    //==============================================================================
    /** */
    void generateSRPAValues();

    /** */
    bool initiateAuthentication();

    /** */
    Aws::String generateChallengeParameters (const String& salt,
                                             const String& srp_b,
                                             const String& secret_block,
                                             const String& time_str);

    /** */
    Array<int8> getPasswordAuthenticationKey (const String& salt, const String& srp_b);

private:
    //==============================================================================
    String username, password, userPool,
           clientId, regionId,
           srpa, srpA;

    BIGNUM* bn_N_ = BN_new();
    BIGNUM* bn_g_ = BN_new();
    BIGNUM* bn_k_ = BN_new();
    BIGNUM* bn_random_ = BN_new();
    BIGNUM* bn_a_ = BN_new();
    BIGNUM* bn_A_ = BN_new();

    std::shared_ptr<Aws::CognitoIdentityProvider::CognitoIdentityProviderClient> client;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AWSUserAuthentication)
};
