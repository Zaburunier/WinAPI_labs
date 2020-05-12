// WinAPI_lab2.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "WinAPI_lab2.h"
#include <windows.h>

ATOM RegWndClass_lab(HINSTANCE, LPCTSTR);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
    LPCTSTR wndClassName = TEXT("FIRST");
    RegWndClass_lab(hInstance, wndClassName);
    RECT screen;
    GetWindowRect(GetDesktopWindow(), &screen);
    screen.right /= 2;
    screen.bottom /= 2;
    int screenSize = 800;
    CreateWindow(wndClassName,  TEXT("First window"), WS_VISIBLE | WS_OVERLAPPEDWINDOW, 
        screen.right - screenSize / 2, screen.bottom - screenSize / 2, screenSize, screenSize, nullptr, nullptr, hInstance, nullptr);

    MSG msg = { 0 };    //структура сообщения
    int iGetOk = 0;   //переменная состояния
    while ((iGetOk = GetMessage(&msg, nullptr, 0, 0)) != 0) //цикл сообщений
    {
        //если GetMessage вернул ошибку - выход
        if (iGetOk == -1) return 3;
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return 0;
}



ATOM RegWndClass_lab(HINSTANCE hInst, LPCTSTR lpzClassName) {
    WNDCLASS windowClass = { 0 };
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = (WNDPROC) WndProc;
    windowClass.hInstance = hInst;
    windowClass.lpszClassName = lpzClassName;
    windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    return RegisterClass(&windowClass);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    HWND secondWindowDescriptor;
    secondWindowDescriptor = FindWindow(TEXT("SECOND"), TEXT("Second window"));
    switch (message)
    {
        case WM_LBUTTONUP:
            if (secondWindowDescriptor == nullptr)
                MessageBox(hWnd, TEXT("Second window not found!"), TEXT(":("), MB_OK);
            else { 
            MessageBox(hWnd, TEXT("Second window found!"), TEXT(":)"), MB_OK); 
            SendMessage(secondWindowDescriptor, WM_USER + 1, NULL, NULL);
            }
            break;
        case WM_RBUTTONUP:
            SendMessage(secondWindowDescriptor, WM_USER + 2, NULL, NULL);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}