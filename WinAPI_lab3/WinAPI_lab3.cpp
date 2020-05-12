// WinAPI_lab3.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "WinAPI_lab3.h"
#include <windows.h>

ATOM RegWndClass_lab(HINSTANCE, LPCTSTR);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
    LPCTSTR wndClassName = TEXT("Lab_3");
    RegWndClass_lab(hInstance, wndClassName);
    RECT screen;
    GetWindowRect(GetDesktopWindow(), &screen);
    screen.right /= 2;
    screen.bottom /= 2;
    int screenSize = 800;
    CreateWindow(wndClassName, TEXT("Lab3"), WS_VISIBLE | WS_OVERLAPPEDWINDOW,
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
    windowClass.lpfnWndProc = (WNDPROC)WndProc;
    windowClass.hInstance = hInst;
    windowClass.lpszClassName = lpzClassName;
    windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    return RegisterClass(&windowClass);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case WM_PAINT:
    {

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        HFONT hfont = CreateFontA(48, 0, 0, 0, FW_REGULAR, NULL, NULL, NULL, RUSSIAN_CHARSET, NULL, NULL, NULL, NULL, "Times New Roman");
        SelectObject(ps.hdc, hfont);
        SYSTEMTIME time; 
        GetLocalTime(&time);
        LPCSTR str;
        TextOutA(hdc, 100, 100, , 8);
        EndPaint(hWnd, &ps);
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