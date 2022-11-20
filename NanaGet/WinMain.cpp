﻿#include <Windows.h>

#include "pch.h"

#include "App.h"
#include "MainPage.h"

#define _ATL_NO_AUTOMATIC_NAMESPACE
#include <atlbase.h>
#include <atlwin.h>
#include <atltypes.h>

#define _WTL_NO_AUTOMATIC_NAMESPACE
#include <atlapp.h>
#include <atlcrack.h>

#include "NanaGetResources.h"

//namespace
//{
//    // {CCD5AA13-D01B-494F-A5E1-3E373234857B}
//    const GUID NotifyIconGuid =
//    {
//        0xccd5aa13,
//        0xd01b,
//        0x494f,
//        { 0xa5, 0xe1, 0x3e, 0x37, 0x32, 0x34, 0x85, 0x7b }
//    };
//
//    const UINT NotifyIconCallbackMessage = WM_APP + 0x100;
//}

namespace NanaGet
{
    class MainWindow :
        public ATL::CWindowImpl<MainWindow>,
        public WTL::CMessageFilter
    {
    public:

        DECLARE_WND_SUPERCLASS(
            L"NanaGetMainWindow",
            L"Mile.Xaml.ContentWindow")

        BEGIN_MSG_MAP(MainWindow)
            MSG_WM_CREATE(OnCreate)
            //MESSAGE_HANDLER_EX(NotifyIconCallbackMessage, OnNotifyIcon)
            MSG_WM_DESTROY(OnDestroy)           
        END_MSG_MAP()

        virtual BOOL PreTranslateMessage(
            MSG* pMsg);

        int OnCreate(
            LPCREATESTRUCT lpCreateStruct);

        /*LRESULT OnNotifyIcon(
            UINT uMsg,
            WPARAM wParam,
            LPARAM lParam);*/

        void OnDestroy();

    private:

        WTL::CIcon m_ApplicationIcon;
    };
}

BOOL NanaGet::MainWindow::PreTranslateMessage(
    MSG* pMsg)
{
    // Workaround for capturing Alt+F4 in applications with XAML Islands.
    // Reference: https://github.com/microsoft/microsoft-ui-xaml/issues/2408
    if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4)
    {
        ::SendMessageW(
            ::GetAncestor(pMsg->hwnd, GA_ROOT),
            pMsg->message,
            pMsg->wParam,
            pMsg->lParam);
        return TRUE;
    }
    return FALSE;
}

int NanaGet::MainWindow::OnCreate(
    LPCREATESTRUCT lpCreateStruct)
{
    UNREFERENCED_PARAMETER(lpCreateStruct);

    if (this->DefWindowProcW() != 0)
    {
        return -1;
    }

    this->m_ApplicationIcon.LoadIconW(
        MAKEINTRESOURCE(IDI_NANAGET),
        256,
        256,
        LR_SHARED);

    this->SetIcon(this->m_ApplicationIcon, TRUE);
    this->SetIcon(this->m_ApplicationIcon, FALSE);

    UINT DpiValue = ::GetDpiForWindow(this->m_hWnd);

    this->SetWindowPos(
        nullptr,
        0,
        0,
        ::MulDiv(720, DpiValue, USER_DEFAULT_SCREEN_DPI),
        ::MulDiv(540, DpiValue, USER_DEFAULT_SCREEN_DPI),
        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    /*NOTIFYICONDATAW NotifyIconData = { 0 };
    NotifyIconData.cbSize = sizeof(NOTIFYICONDATAW);
    NotifyIconData.hWnd = this->m_hWnd;
    NotifyIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    NotifyIconData.uFlags |= NIF_GUID | NIF_SHOWTIP;
    NotifyIconData.uCallbackMessage = NotifyIconCallbackMessage;
    NotifyIconData.hIcon = this->m_ApplicationIcon.m_hIcon;
    ::wcscpy_s(NotifyIconData.szTip, L"NanaGet");
    NotifyIconData.guidItem = NotifyIconGuid;
    winrt::check_bool(::Shell_NotifyIconW(NIM_DELETE, &NotifyIconData));
    winrt::check_bool(::Shell_NotifyIconW(NIM_ADD, &NotifyIconData));

    NotifyIconData.uVersion = NOTIFYICON_VERSION_4;
    winrt::check_bool(::Shell_NotifyIconW(NIM_SETVERSION, &NotifyIconData));*/
    
    return 0;
}

//LRESULT NanaGet::MainWindow::OnNotifyIcon(
//    UINT uMsg,
//    WPARAM wParam,
//    LPARAM lParam)
//{
//    UNREFERENCED_PARAMETER(uMsg);
//    UNREFERENCED_PARAMETER(wParam);
//
//    switch (LOWORD(lParam))
//    {
//    case NIN_SELECT:
//    {
//        this->ShowWindow(
//            ::IsWindowVisible(this->m_hWnd)
//            ? SW_HIDE
//            : SW_SHOW);
//
//        break;
//    }
//    case WM_CONTEXTMENU:
//    {
//        ::SetForegroundWindow(this->m_hWnd);
//
//        break;
//    }
//    default:
//        break;
//    }
//
//    return 0;
//}

void NanaGet::MainWindow::OnDestroy()
{
    /*NOTIFYICONDATAW NotifyIconData = { 0 };
    NotifyIconData.cbSize = sizeof(NOTIFYICONDATAW);
    NotifyIconData.uFlags = NIF_GUID;
    NotifyIconData.guidItem = NotifyIconGuid;
    ::Shell_NotifyIconW(NIM_DELETE, &NotifyIconData);*/

    ::PostQuitMessage(0);
}

namespace
{
    WTL::CAppModule g_Module;
}

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    winrt::init_apartment(winrt::apartment_type::single_threaded);

    winrt::NanaGet::App app =
        winrt::make<winrt::NanaGet::implementation::App>();  

    g_Module.Init(nullptr, hInstance);

    using MainPageInstance = winrt::NanaGet::implementation::MainPage;

    winrt::NanaGet::MainPage Control = winrt::make<MainPageInstance>();

    NanaGet::MainWindow Window;
    if (!Window.Create(
        nullptr,
        Window.rcDefault,
        L"NanaBox",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        WS_EX_CLIENTEDGE,
        nullptr,
        winrt::get_abi(Control)))
    {
        return -1;
    }
    Window.ShowWindow(nShowCmd);
    Window.UpdateWindow();

    WTL::CMessageLoop MessageLoop;
    MessageLoop.AddMessageFilter(&Window);
    g_Module.AddMessageLoop(&MessageLoop);
    int Result = MessageLoop.Run();

    g_Module.RemoveMessageLoop();
    g_Module.Term();

    app.Close();

    return Result;
}
