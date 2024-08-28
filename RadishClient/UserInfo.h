#pragma once


namespace Radish {


	//////////////////////////////////////////////////////////////////////////
	// class UserInfo
	// 用户信息类
	//////////////////////////////////////////////////////////////////////////
	class UserInfo
	{
	public:
		UserInfo();
		~UserInfo();

		// 获取/设置账号
		void			 SetAccount(const char* account);
		const char*		 GetAccount() const;

		// 获取/设置名称
		void	         SetName(const char* name);
		const char*      GetName() const;

		// 获取/设置密码
		void             SetPassword(const char* psw);
		const char*      GetPassword() const;

		// 获取/设置签名
		void			 SetWhatsUp(const char* whatsUp);
		const char*		 GetWhatsUp() const;

		// 获取/设置头像
		void			 SetProfilePicture(const char* path);
		const char*		 GetProfilePicture() const;

		// 获取/设置地区
		void			 SetRegion(const char* region);
		const char*		 GetRegion() const;

		// 获取/设置性别
		void			 SetGender(const char* gender);
		const char*		 GetGender() const;

		// 加密解密
		void EncodeDES(const char* key, const char* data);
		void DecodeDES(const char* key, const char* data);
		
		void EncodeAES(const char* key, const char* data);
		void DecodeAES(const char* key, const char* data);
		
		// ...waiting
		void EncodeRSA();
		void DecodeRSA();


	private:
		enum { MAX_LENGTH = 20 };
		enum { MAX_PSW_LENGTH = 32 };
		enum { MAX_WHATSUP_LENGTH = 30 };
		enum { MAX_FILE_PATH = 256 };
		char m_szAccount[MAX_LENGTH];                               // 账号
		char m_szPassword[MAX_PSW_LENGTH];                          // 密码
		char m_szName[MAX_LENGTH];                                  // 名称
		char m_szProfilePicture[MAX_FILE_PATH];                     // 头像
		char m_szWhatsUp[MAX_WHATSUP_LENGTH];				        // 签名
		char m_szRegion[MAX_LENGTH];                                // 地区
		short m_nGender;                                            // 性别
	};

}

