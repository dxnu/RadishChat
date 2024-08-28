#pragma once


namespace Radish {


	//////////////////////////////////////////////////////////////////////////
	// class UserInfo
	// �û���Ϣ��
	//////////////////////////////////////////////////////////////////////////
	class UserInfo
	{
	public:
		UserInfo();
		~UserInfo();

		// ��ȡ/�����˺�
		void			 SetAccount(const char* account);
		const char*		 GetAccount() const;

		// ��ȡ/��������
		void	         SetName(const char* name);
		const char*      GetName() const;

		// ��ȡ/��������
		void             SetPassword(const char* psw);
		const char*      GetPassword() const;

		// ��ȡ/����ǩ��
		void			 SetWhatsUp(const char* whatsUp);
		const char*		 GetWhatsUp() const;

		// ��ȡ/����ͷ��
		void			 SetProfilePicture(const char* path);
		const char*		 GetProfilePicture() const;

		// ��ȡ/���õ���
		void			 SetRegion(const char* region);
		const char*		 GetRegion() const;

		// ��ȡ/�����Ա�
		void			 SetGender(const char* gender);
		const char*		 GetGender() const;

		// ���ܽ���
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
		char m_szAccount[MAX_LENGTH];                               // �˺�
		char m_szPassword[MAX_PSW_LENGTH];                          // ����
		char m_szName[MAX_LENGTH];                                  // ����
		char m_szProfilePicture[MAX_FILE_PATH];                     // ͷ��
		char m_szWhatsUp[MAX_WHATSUP_LENGTH];				        // ǩ��
		char m_szRegion[MAX_LENGTH];                                // ����
		short m_nGender;                                            // �Ա�
	};

}

