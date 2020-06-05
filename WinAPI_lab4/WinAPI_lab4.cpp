// WinAPI_lab4.cpp : Определяет точку входа для приложения.
//

#define _CRT_SECURE_NO_WARNINGS
#define BCKGND_COLOR RGB(240, 240, 240)

#include "framework.h"
#include "WinAPI_lab4.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

DWORD WINAPI ThreadProc1(const LPVOID);
DWORD WINAPI ThreadProc2(const LPVOID);
void ShowTime(HWND, bool, bool, RECT*, HFONT, HDC);
void DrawTime(HWND, bool, bool);
ATOM RegWndClass_lab(HINSTANCE, LPCTSTR);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
    RegWndClass_lab(hInstance, TEXT("Lab_4"));
    RECT screen;
    GetWindowRect(GetDesktopWindow(), &screen);
    int screenSizex = 900, screenSizey = 500;
    HWND hwndParent = CreateWindow(TEXT("Lab_4"), TEXT("Lab4"),
        WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        (screen.right - screenSizex) / 2, (screen.bottom - screenSizey) / 2 - 1,
        screenSizex, screenSizey,
        nullptr, nullptr, hInstance, nullptr);
    MSG msg = { 0 };    //структура сообщения
    int iGetOk = 0;

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
    static HANDLE mutex1, thread1, mutex2, thread2;
    switch (message)
    {
    case WM_CREATE:
        SetClassLongW(hWnd, GCLP_HBRBACKGROUND, (LONG)CreateSolidBrush(BCKGND_COLOR));
        mutex1 = CreateMutex(NULL, false, NULL);
        thread1 = CreateThread(NULL, 0, &ThreadProc1, &mutex1, 0, NULL);
        mutex2 = CreateMutex(NULL, false, NULL);
        thread2 = CreateThread(NULL, 0, &ThreadProc2, &mutex2, 0, NULL);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

DWORD WINAPI ThreadProc1(const LPVOID lpParam) {
    int time = GetTickCount();
    int sintime = GetTickCount() / 1000;
    HWND hwnd = FindWindowA("Lab_4", "Lab4");
    if (hwnd == NULL) {
        MessageBox(hwnd, TEXT("Окно не найдено"), TEXT(":("), MB_OK);
    }
    RECT* rect = new RECT{ 150, 50, 250, 250 };
    RECT* eraseRect = new RECT { 0, 50, 150, 250 };
    int currentTime;
    float temp;
    float left = 50, right = 90;
    HDC hdc = GetDC(hwnd);
    HBRUSH hBrush1 = CreateSolidBrush(BCKGND_COLOR);
    HBRUSH hBrush2 = CreateSolidBrush(RGB(255, 255, 0));
    while (true) {
        currentTime = (GetTickCount() - sintime) / 100;
        temp = sin(currentTime);
        left = 50 + 8 * (1 - temp);
        right = 90 + 8 * (1 + temp);
        Rectangle(hdc, eraseRect->left, eraseRect->top, eraseRect->right, eraseRect->bottom);
        FillRect(hdc, eraseRect, hBrush1);
        SelectObject(hdc, hBrush2);
        Ellipse(hdc, left, 50, 250, 250);
        SelectObject(hdc, hBrush1);
        Ellipse(hdc, right, 50, 210, 250);
        Rectangle(hdc, rect->left, rect->top, rect->right, rect->bottom);
        FillRect(hdc, rect, hBrush1);
        Sleep(50);
    }
    ExitThread(0);
}

DWORD WINAPI ThreadProc2(const LPVOID lpParam) {
    int time = GetTickCount();
    char totalMem[4], avaMem[4];
    HFONT font = CreateFont(72, 0, 0, 0, 0,
        NULL, NULL, NULL, DEFAULT_CHARSET,
        OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("Times New Roman"));
    RECT* rect1 = new RECT { 400, 100, 550, 200 };
    RECT* rect2 = new RECT { 500, 300, 620, 400 };
    MEMORYSTATUS* mem = new MEMORYSTATUS;
    int available, total;
    HWND hwnd = FindWindowA("Lab_4", "Lab4");
    HDC hdc = GetDC(hwnd);
    SetBkColor(hdc, BCKGND_COLOR);
    SetTextColor(hdc, RGB(0, 0, 0));
    SelectObject(hdc, font);
    while (true) {
        GlobalMemoryStatus(mem);
        available = mem->dwAvailPhys / 1024 / 1024;
        total = mem->dwTotalPhys / 1024 / 1024;
        totalMem[0] = total / 1000 + 48;
        totalMem[1] = total % 1000 / 100 + 48;
        totalMem[2] = total % 100 / 10 + 48;
        totalMem[3] = total % 10 + 48;
        avaMem[0] = available / 1000 + 48;
        avaMem[1] = available % 1000 / 100 + 48;
        avaMem[2] = available % 100 / 10 + 48;
        avaMem[3] = available % 10 + 48;
        DrawTextA(hdc, avaMem, 4, rect2, DT_CENTER);
        DrawTextA(hdc, totalMem, 4, rect1, DT_CENTER);
        Sleep(50);
    }

    ExitThread(0);
}