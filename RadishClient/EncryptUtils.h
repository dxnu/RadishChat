#pragma once
#include <string>
#include <Cryptopp/des.h>
#include <Cryptopp/aes.h>
#include <Cryptopp/rsa.h>
#pragma comment(lib, "cryptlib.lib")

using namespace CryptoPP;



namespace Radish {
namespace Utils {


class EncryptUtils
{
public:

	// DES
	static void InitalizeDES();
	static std::string GetDESKey();
	static void SetDESKey(const std::string& key);
	static void EncryptDES(const std::string &plainData, std::string &cipherData);
	static void DecryptDES(const std::string &cipherData, std::string &recoveredData);

	// 3DES
	static void Initalize3DES();
	static std::string Get3DESKey();
	static void Set3DESKey(const std::string& key);
	static void Encrypt3DES(const std::string &plainData, std::string &cipherData);
	static void Decrypt3DES(const std::string &cipherData, std::string &recoveredData);

	// AES
	static void InitalizeAES();
	static std::string GetAESKey();
	static void SetAESKey(const std::string& key);
	static void EncryptAES(const std::string &plainData, std::string &cipherData);
	static void DecryptAES(const std::string &cipherData, std::string &recoveredData);

	// RSA
	static void InitalizeRSA();
	static std::string GetRSAPublicKey();
	static void SetRSAPublicKey(std::string& key);
	static void EncryptRSA(const std::string &plainData, std::string &cipherData);
	static void DecryptRSA(const std::string &cipherData, std::string &recoveredData);

	// SHA3
	static void EncrySHA256(const std::string &message, std::string &digest);


private:
	static SecByteBlock m_desKey;
	static byte m_desIV[DES_EDE2::BLOCKSIZE];

	static SecByteBlock m_3desKey;
	static byte m_3desIV[DES_EDE3::BLOCKSIZE];

	static SecByteBlock m_aesKey;
	static SecByteBlock m_aesIV;

	static RSA::PublicKey m_rsaPublicKey;
	static RSA::PrivateKey m_rsaPrivateKey;
};

} // namespace Utils
} // namespace Radish
