#include "Common.h"
#include "ChatWnd.h"
#include "LoginWnd.h"
#include "StringConvert.h"


INT WINAPI _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance, LPWSTR lpCmdLine, INT nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + L"skin");

	auto login = std::make_shared<LoginWnd>();
	login->Create(nullptr, L"login", UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	login->CenterWindow();
	login->ShowModal();

	/*auto client = std::make_shared<RadishClient>();
	auto login = std::make_unique<ChatWnd>(client);
	login->SetParam("default", "haha");
	login->Create(nullptr, L"login", UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	login->CenterWindow();
	login->ShowModal();*/

	return 0;
}