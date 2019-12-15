#ifndef SLSWINDOW_H
#define SLSWINDOW_H
#include <Windows.h>
#include <string>

ATOM                registerClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
WCHAR szWindowClass[100] = L"EME.LauncherWnd";
std::string gameString = "";

ATOM registerClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex;

    ZeroMemory( &wcex, sizeof(WNDCLASSEX) );
    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszClassName  = szWindowClass;

    return RegisterClassExW(&wcex);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PCOPYDATASTRUCT pMyCDS;
    pMyCDS = (PCOPYDATASTRUCT)lParam;
    LPCSTR readMessage = "";

    switch (message)
    {
    case WM_COPYDATA:
        if(pMyCDS) {
            readMessage = (LPCSTR)(pMyCDS->lpData);
        }
        if(strcmp(readMessage, "Hello!!") == 0) {
            ReplyMessage(1);
            SendMessageA((HWND)wParam, WM_COPYDATA, (WPARAM)hWnd, lParam);
        } else if(strcmp(readMessage, "slsurl") == 0) {
            ReplyMessage(1);
            std::string serverListURL = "http://sls.service.enmasse.com:8080/servers/list.en";
            pMyCDS->lpData = (PVOID)serverListURL.c_str();
            pMyCDS->cbData = serverListURL.length() + 1;
            SendMessageA((HWND)wParam, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)pMyCDS);
        } else if(strcmp(readMessage, "gamestr") == 0) {
            ReplyMessage(1);
            pMyCDS->lpData = (PVOID)gameString.c_str();
            pMyCDS->cbData = gameString.length() + 1;
            SendMessageA((HWND)wParam, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)pMyCDS);
        } else {
            ReplyMessage(1);
        }

        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void messageThread(HACCEL hAccelTable) {
    MSG msg;

    while(GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

#endif // SLSWINDOW_H
