//// RichListDemo.cpp : 定义应用程序的入口点。
////
//
//#include "stdafx.h"
//#include <exdisp.h>
//#include <comdef.h>
//#include "Common.h"
//#include "RichListWnd.h"
//
//int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
//{
//	CPaintManagerUI::SetInstance(hInstance);
//	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + L"skin");
//	//CPaintManagerUI::SetResourceZip(_T("RichListRes.zip"));
//
//	//HRESULT Hr = ::CoInitialize(NULL);
//	//if( FAILED(Hr) ) return 0;
//
//	//CRichListWnd* pFrame = new CRichListWnd();
//	//if( pFrame == NULL ) return 0;
//	//pFrame->Create(NULL, _T("DuiLib Demo from mbstudio.cn / meineson"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 500, 400);
//	//pFrame->CenterWindow();
//	////::ShowWindow(*pFrame, SW_SHOW);
//	//pFrame->ShowModal();
//
//
//	////::MessageBox(NULL, _T("alert!"), _T("Duilib Demo"), MB_OK);
//
//	//CPaintManagerUI::MessageLoop();
//
//	//::CoUninitialize();
//
//	auto pFrame = std::make_unique<CRichListWnd>();
//	pFrame->Create(nullptr, L"DuiLib Demo", UI_WNDSTYLE_FRAME, 0L, 0, 0, 500, 400);
//	pFrame->CenterWindow();
//	pFrame->ShowModal();
//
//	return 0;
//}
