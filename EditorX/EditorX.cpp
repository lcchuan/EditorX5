// EditorX.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "EditorX.h"
#include "EditorXFrame.h"
#include "./directui/uibase.h"
#include "GdiplusHelp.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	std::wstring filepath = EditorXFrame::ParseCmdLine(lpCmdLine);

	//如果已有该实例，则显示前一个实例
	const HWND hPre = ::FindWindow(EditorXFrame::GetMainWndClassName().c_str(),NULL);
	if (hPre)
	{
		::PostMessage(hPre,WM_SYSCOMMAND, SC_RESTORE,0);
		if (filepath.length() > 0) {
			/* 两个进程由于使用的是相互独立的两个虚拟内存空间，
			 * 同一地址对不同的进程来说并不一定指向同一物理内存，内容也就不一定一样，
			 * 因此不同进程无法通过传地址的方式传递字符串（但是同一进程下的不同线程是可以的）
			 */
			//::SendMessage(hPre,EditorXFrame::WM_OPENFILE,0,LPARAM(filepath.c_str()));
			COPYDATASTRUCT data;
			data.dwData = 1;
			data.lpData = (wchar_t*)filepath.data();
			data.cbData = (DWORD)(filepath.length()*2);
			::SendMessage(hPre,WM_COPYDATA,0,LPARAM(&data));
		} else {
			//新建一个tab页
			::SendMessage(hPre,WM_COMMAND,MAKELONG(IDM_NEWFILE,0),0);
		}
		
		return FALSE;
	}

	/*
	 * 声明程序支持DPI缩放，否则高分辨机器上所有窗口的字体会显示锯齿，调用::GetDeviceCaps(hdc,LOGPIXELSX)时返回的也是96
	 * 该问题困扰了好久，猜测MFC环境没问题应该是自动调用了该函数
	 */
	SetProcessDPIAware();

	// Initialize GDI+.
	GdiplusHelp::LoadGdiplus();

	// 执行应用程序初始化:
	if (!EditorXFrame::GetInstance().InitInstance(hInstance, nCmdShow))
	{
		GdiplusHelp::UnloadGdiplus();
		return FALSE;
	}
	if (filepath.length() > 0) {
		EditorXFrame::GetInstance().OpenFile(filepath.c_str());
	}

	// 主消息循环:
	MSG msg;
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EDITORX));
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	GdiplusHelp::UnloadGdiplus();
	return (int) msg.wParam;
}