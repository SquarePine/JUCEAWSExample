class Aws::CognitoIdentityProvider::CognitoIdentityProviderClient;

class AWSUserAuthentication
{
public:
    AWSUserAuthentication (const std::string &username, const std::string &password,
                           const std::string &user_pool, const std::string client_id,
                           const std::string region_id);

    ~AWSUserAuthentication();

    void GenerateSRPAValues();
    bool InitiateAuthentication();

    Aws::String GenerateChallengeParameters(const Aws::String &salt,
                                            const Aws::String &srp_b,
                                            const Aws::String &secret_block,
                                            const Aws::String &time_str);

    void AWSUserAuthentication::getPasswordAuthenticationKey(const Aws::String &salt, const Aws::String &srp_b, std::vector<unsigned char> &key);

private:
    std::string username_;
    std::string password_;
    std::string user_pool_;
    std::string client_id_;
    std::string region_id_;

    BIGNUM* bn_N_ = nullptr;
    BIGNUM* bn_g_ = nullptr;
    BIGNUM* bn_k_ = nullptr;
    BIGNUM* bn_random_ = nullptr;
    BIGNUM* bn_a_ = nullptr;
    BIGNUM* bn_A_ = nullptr;

    std::string srp_a_string_;
    std::string srp_A_string_;

    std::shared_ptr<Aws::CognitoIdentityProvider::CognitoIdentityProviderClient> cognito_client_;
};
