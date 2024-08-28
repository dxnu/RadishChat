#include "UserInfo.h"
#include <xutility>
#include <regex>


namespace Radish {


	UserInfo::UserInfo()
		: m_nGender(-1)
	{
		memset(m_szAccount, 0, MAX_LENGTH);
		memset(m_szName, 0, MAX_LENGTH);
		memset(m_szWhatsUp, 0, MAX_WHATSUP_LENGTH);
		memset(m_szProfilePicture, 0, MAX_FILE_PATH);
		memset(m_szPassword, 0, MAX_PSW_LENGTH);
		memset(m_szSalt, 0, SALT_LENGTH);
		memset(m_szEmail, 0, MAX_EMAIL_LENGTH);
	}

	UserInfo::~UserInfo()
	{

	}

	void UserInfo::SetName(const char* name)
	{
		auto len = strlen(name);
		if (len > MAX_LENGTH)
		{
			throw std::runtime_error("Error: too long name!");
		}

		memcpy(m_szName, name, len);
	}

	const char* UserInfo::GetName() const
	{
		return m_szName;
	}

	void UserInfo::SetAccount(const char* account)
	{
		auto len = strlen(account);
		if (len > MAX_LENGTH)
		{
			throw std::runtime_error("Error: 账号的最大长度为20!");
		}

		// 匹配账号ID，长度<5-20>
		std::regex reg("[\\da-zA-Z]\\w{4,20}");
		auto ret = std::regex_match(account, reg);
		if (ret)
		{
			memcpy(m_szAccount, account, len);
		}
		else
		{
			throw std::runtime_error("Error: 账号只能由数字或字母开头，除首位可使用下划线!");
		}
	}

	const char* UserInfo::GetAccount() const
	{
		return m_szAccount;
	}

	void UserInfo::SetPassword(const char* psw)
	{
		// 匹配密码，长度<6,18>
		std::regex reg("[\\w_\\.\\+-^*@=]{6,18}");
		auto ret = std::regex_match(psw, reg);
		if (ret)
		{
			memcpy(m_szPassword, psw, strlen(psw));
		}
		else
		{
			throw std::runtime_error("Error: 密码支持长度<6,18>，可包含的特殊字符有“_.+-^*@=”。");
		}
	}

	const char* UserInfo::GetPassword() const
	{
		return m_szPassword;
	}

	void UserInfo::SetWhatsUp(const char* whatsUp)
	{
		auto len = strlen(whatsUp);
		if (len > MAX_WHATSUP_LENGTH)
		{
			throw std::runtime_error("Error: 签名长度最大为30!");
		}

		memcpy(m_szWhatsUp, whatsUp, len);
	}

	const char* UserInfo::GetWhatsUp() const
	{
		return m_szWhatsUp;
	}

	void UserInfo::SetProfilePicture(const char* path)
	{
		auto len = strlen(path);
		if (len > MAX_FILE_PATH)
		{
			throw std::runtime_error("Error: too long path");
		}

		memcpy(m_szProfilePicture, path, len);
	}

	const char* UserInfo::GetProfilePicture() const
	{
		return m_szProfilePicture;
	}

	void UserInfo::SetRegion(const char* region)
	{
		auto len = strlen(region);
		if (len > MAX_LENGTH)
		{
			throw std::runtime_error("Error: too long region");
		}

		memcpy(m_szRegion, region, strlen(region));
	}

	const char* UserInfo::GetRegion() const
	{
		return m_szRegion;
	}

	void UserInfo::SetGender(const char* gender)
	{
		if (gender == "男")
			m_nGender = 0;
		else if (gender == "女")
			m_nGender = 1;
		else if (gender == "保密")
			m_nGender = -1;
		else
			throw std::runtime_error("Error: 无效的性别");
	}

	const short UserInfo::GetGender() const
	{
		return m_nGender;
	}

	void UserInfo::SetEmail(const char* email)
	{
		memcpy(m_szEmail, email, strlen(email));
	}

	const char* UserInfo::GetEmail() const
	{
		return m_szEmail;
	}

	void UserInfo::SetSalt(const char* salt)
	{
		memcpy(m_szSalt, salt, strlen(salt));
	}

	const char* UserInfo::GetSalt() const
	{
		return m_szSalt;
	}

} // namespace Radish