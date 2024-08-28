#include "EncryptUtils.h"
#include <CryptoPP/osrng.h>
#include <Cryptopp/modes.h>
#include <Cryptopp/sha3.h>
#include <Cryptopp/hex.h>



namespace Radish {
namespace Utils {


	SecByteBlock		EncryptUtils::m_desKey(0x00, DES_EDE2::DEFAULT_KEYLENGTH);
	byte				EncryptUtils::m_desIV[DES_EDE2::BLOCKSIZE];

	SecByteBlock        EncryptUtils::m_3desKey(0x00, DES_EDE3::DEFAULT_BLOCKSIZE);
	byte                EncryptUtils::m_3desIV[DES_EDE3::BLOCKSIZE];

	SecByteBlock        EncryptUtils::m_aesKey(0x00, AES::DEFAULT_KEYLENGTH);
	SecByteBlock        EncryptUtils::m_aesIV(AES::BLOCKSIZE);

	RSA::PublicKey      EncryptUtils::m_rsaPublicKey;
	RSA::PrivateKey		EncryptUtils::m_rsaPrivateKey;




void EncryptUtils::InitalizeDES()
{
	AutoSeededRandomPool rng;

	rng.GenerateBlock(m_desKey, m_desKey.size());
	rng.GenerateBlock(m_desIV, sizeof(m_desIV));
}

std::string EncryptUtils::GetDESKey()
{
	std::string key((char*)m_desKey.data(), m_desKey.size());
	return key;
}

void EncryptUtils::SetDESKey(const std::string& key)
{
	SecByteBlock s((const byte*)key.data(), key.size());
	m_desKey = s;
}

void EncryptUtils::EncryptDES(const std::string &plainData, std::string &cipherData)
{
	try {
		CBC_Mode<DES_EDE2>::Encryption e;
		e.SetKeyWithIV(m_desKey, m_desKey.size(), m_desIV);

		StringSource ss(plainData, true,
			new StreamTransformationFilter(e, new StringSink(cipherData))
		);
	}
	catch (const CryptoPP::Exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void EncryptUtils::DecryptDES(const std::string &cipherData, std::string &recoveredData)
{
	try {
		CBC_Mode<DES_EDE2>::Decryption d;
		d.SetKeyWithIV(m_desKey, m_desKey.size(), m_desIV);

		StringSource ss(cipherData, true,
			new StreamTransformationFilter(d, new StringSink(recoveredData))
		);
	}
	catch (const CryptoPP::Exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void EncryptUtils::Initalize3DES()
{
	AutoSeededRandomPool rng;

	rng.GenerateBlock(m_3desKey, m_3desKey.size());
	rng.GenerateBlock(m_3desIV, sizeof(m_3desIV));
}

std::string EncryptUtils::Get3DESKey()
{
	std::string key((char *)m_3desKey.data(), m_3desKey.size());
	return key;
}

void EncryptUtils::Set3DESKey(const std::string& key)
{
	SecByteBlock s((const byte*)key.data(), key.size());
	m_3desKey = s;
}

void EncryptUtils::Encrypt3DES(const std::string &plainData, std::string &cipherData)
{
	try {
		CBC_Mode<DES_EDE3>::Encryption e;
		e.SetKeyWithIV(m_3desKey, m_3desKey.size(), m_3desIV);

		StringSource ss(plainData, true,
			new StreamTransformationFilter(e, new StringSink(cipherData))
		);
	}
	catch (const CryptoPP::Exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void EncryptUtils::Decrypt3DES(const std::string &cipherData, std::string &recoveredData)
{
	try {
		CBC_Mode<DES_EDE3>::Decryption d;
		d.SetKeyWithIV(m_3desKey, m_3desKey.size(), m_3desIV);

		StringSource ss(cipherData, true,
			new StreamTransformationFilter(d, new StringSink(recoveredData))
		);
	}
	catch (const CryptoPP::Exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void EncryptUtils::InitalizeAES()
{
	AutoSeededRandomPool rng;

	// 生成随机密钥
	rng.GenerateBlock(m_aesKey, m_aesKey.size());

	// 生成随机IV
	rng.GenerateBlock(m_aesIV, m_aesIV.size());
}

std::string EncryptUtils::GetAESKey()
{
	std::string key((char*)m_aesKey.data(), m_aesKey.size());
	return key;
}

void EncryptUtils::SetAESKey(const std::string& key)
{
	SecByteBlock s((const byte*)key.data(), key.size());
	m_aesKey = s;
}

void EncryptUtils::EncryptAES(const std::string &plainData, std::string &cipherData)
{
	// 加密
	CFB_Mode<AES>::Encryption e(m_aesKey, m_aesKey.size(), m_aesIV);
	cipherData.resize(plainData.size());
	e.ProcessData((byte*)&cipherData[0], (byte*)plainData.data(), plainData.size());
}

void EncryptUtils::DecryptAES(const std::string &cipherData, std::string &recoveredData)
{
	// 解密
	CFB_Mode<AES>::Decryption d(m_aesKey, m_aesKey.size(), m_aesIV);
	recoveredData.resize(cipherData.size());
	d.ProcessData((byte*)&recoveredData[0], (byte*)cipherData.data(), cipherData.size());
}

void EncryptUtils::InitalizeRSA()
{
	// 伪随机数生成器
	AutoSeededRandomPool rng;

	// 生成参数
	InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rng, 1024);

	// 创建密钥对
	m_rsaPrivateKey = params;
	m_rsaPublicKey = params;
}

std::string EncryptUtils::GetRSAPublicKey()
{
	std::string pubKey;
	StringSink ss(pubKey);
	m_rsaPublicKey.Save(ss);

	return pubKey;
}

void EncryptUtils::SetRSAPublicKey(std::string& key)
{
	StringSink ss(key);
	m_rsaPublicKey.Load(ss);
}

void EncryptUtils::EncryptRSA(const std::string &plainData, std::string &cipherData)
{
	// 伪随机数生成器
	AutoSeededRandomPool rng;

	// 加密器
	RSAES_OAEP_SHA_Encryptor e(m_rsaPublicKey);

	StringSource ss(plainData, true,
		new PK_EncryptorFilter(rng, e, new StringSink(cipherData))
	);
}

void EncryptUtils::DecryptRSA(const std::string &cipherData, std::string &recoveredData)
{
	// 伪随机数生成器
	AutoSeededRandomPool rng;

	// 解密器
	RSAES_OAEP_SHA_Decryptor d(m_rsaPrivateKey);

	StringSource ss(cipherData, true,
		new PK_DecryptorFilter(rng, d, new StringSink(recoveredData))
	);
}

void EncryptUtils::EncrySHA256(const std::string &message, std::string &digest)
{
	SHA3_256 hash;
	hash.Update((const byte*)message.data(), message.size());
	digest.resize(hash.DigestSize());
	hash.Final((byte*)&digest[0]);
}

} // namespce Utils
} // namespace Radish

