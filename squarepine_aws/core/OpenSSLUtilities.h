/** */
inline String binaryToHexString (const char* data, size_t bytes)
{
    if (data == nullptr || bytes == 0)
        return {};

    std::stringstream ss;
    ss << std::hex;

    for (size_t i = 0; i < (bytes / 2); ++i)
        ss << std::setw (2) << std::setfill ('0') << (int) data[i];

    return ss.str();
}

/** */
inline Array<int8> convertHexStringToCharVector (const String& hexString)
{
    if (hexString.isEmpty())
        return {};

    const auto s = hexString.length();

    Array<int8> result;
    result.resize ((s + 1) / 2);

    for (int i = 0; i < s; i += 2)
        result.setUnchecked (i, static_cast<int8> (hexString.substring (i, 2).getIntValue()));

    return result;
}

/** */
inline String base64Encode (const Array<int8>& binary)
{
    if (binary.isEmpty())
        return {};

    auto* b64 = BIO_new (BIO_f_base64());
    BIO_set_flags (b64, BIO_FLAGS_BASE64_NO_NL);
    auto* sink = BIO_new (BIO_s_mem());
    BIO_push (b64, sink);
    BIO_write (b64, binary.data(), int (binary.size()));
    BIO_flush (b64);
    const char* encoded;
    const long len = BIO_get_mem_data (sink,& encoded);
    return String (encoded, len);
}

/**
    @note Assumes no newlines or extra characters in encoded string
*/
inline Array<int8> base64Decode (const char* encoded)
{
    if (encoded == nullptr)
        return {};

    auto* b64 = BIO_new (BIO_f_base64());
    BIO_set_flags (b64, BIO_FLAGS_BASE64_NO_NL);

    auto* source = BIO_new_mem_buf (encoded, -1); // read-only source
    BIO_push (b64, source);

    const auto maxlen = strlen (encoded) / 4 * 3 + 1;
    Array<int8> decoded;
    decoded.resize ((int) maxlen);

    const auto len = BIO_read (b64, decoded.data(), (int) maxlen);
    decoded.resize (len);
    return decoded;
}

/** */
inline String padHexStringWithLeadingZero (const String& hexString)
{
    if ((hexString.length() % 2) == 1)
        return "0" + hexString;

    const auto top = hexString[0];
    if (top == '8' || top == '9'
     || top == 'a' || top == 'b' || top == 'c'
     || top == 'd' || top == 'e' || top == 'f'
     || top == 'A' || top == 'B' || top == 'C'
     || top == 'D' || top == 'E' || top == 'F')
        return "00" + hexString;

    return hexString;
}

/** */
inline Array<int8> padCharVector (const Array<int8>& digest)
{
    if (digest.isEmpty())
        return {};

    //auto r = binaryToHexString (digest.data(), digest.size() * 2);
    //r = padHexStringWithLeadingZero (r);
    //return convertHexStringToCharVector (r);

    return {};
}

/** */
inline Array<int8> digestMessage (const Array<int8>& message)
{
    if (message.isEmpty())
        return {};

    Array<int8> digest;

    auto* mdctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex (mdctx, EVP_sha256(), nullptr);
    EVP_DigestUpdate (mdctx, message.data(), message.size());
    digest.resize (EVP_MD_size (EVP_sha256()));
    unsigned int digest_len;
    EVP_DigestFinal_ex (mdctx, (unsigned char*) digest.data(), &digest_len);
    EVP_MD_CTX_free (mdctx);

    return digest;
}

/** */
inline Array<int8> digestMessage (const String& message)
{
    if (message.isEmpty())
        return {};

    Array<int8> digest;

    auto* mdctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex (mdctx, EVP_sha256(), nullptr);
    EVP_DigestUpdate (mdctx, message.toRawUTF8(), (size_t) message.length());
    digest.resize (EVP_MD_size (EVP_sha256()));
    unsigned int digest_len;
    EVP_DigestFinal_ex (mdctx, (unsigned char*) digest.data(), &digest_len);
    EVP_MD_CTX_free (mdctx);

    return digest;
}

/** */
inline Array<int8> HKDF (const Array<int8>& salt,
                         const Array<int8>& secret,
                         const Array<int8>& label)
{
    Array<int8> result;
    result.resize (16);

    auto* pctx = EVP_PKEY_CTX_new_id (EVP_PKEY_HKDF, nullptr);
    EVP_PKEY_derive_init (pctx);
    EVP_PKEY_CTX_set_hkdf_md (pctx, EVP_sha256());
    EVP_PKEY_CTX_set1_hkdf_salt (pctx, salt.data(),  salt.size());
    EVP_PKEY_CTX_set1_hkdf_key (pctx, secret.data(), secret.size());
    EVP_PKEY_CTX_add1_hkdf_info (pctx, label.data(), label.size());

    auto outlen = (size_t) result.size();
    EVP_PKEY_derive (pctx, (unsigned char*) result.data(), &outlen);

    return result;
}

inline Aws::String toAWSString (const String& juceString)
{
    Aws::String s;
    s.operator= (juceString.toRawUTF8());
    return s;
}
