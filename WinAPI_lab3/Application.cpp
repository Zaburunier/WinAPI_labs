// WinAPI_lab3.cpp : Определяет точку входа для приложения.
//
#define _CRT_SECURE_NO_WARNINGS
#define START_TIMER WM_USER + 1
#define RESET_TIMER WM_USER + 2
#define STOP_TIMER WM_USER + 3
#define BCKGND_COLOR RGB(0, 33, 55)
#define PI 3.1415926

#include "framework.h"
#include "WinAPI_lab3.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
void ShowTime(HWND, bool, bool, RECT*, HFONT, HDC);
void DrawTime(HWND, bool, bool);
ATOM RegWndClass_lab(HINSTANCE, LPCTSTR);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
    RegWndClass_lab(hInstance, TEXT("Lab_3"));
    RECT screen;
    GetWindowRect(GetDesktopWindow(), &screen);
    int screenSizex = 300, screenSizey = 700;
    HWND hwndParent = CreateWindow(TEXT("Lab_3"), TEXT("Lab3"),
        WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        (screen.right - screenSizex) / 2, (screen.bottom - screenSizey) / 2 - 1,
        screenSizex, screenSizey,
        nullptr, nullptr, hInstance, nullptr);
    int buttonSizex = 200, buttonSizey = 50;
    HWND okButton = CreateWindow(TEXT("BUTTON"), TEXT("START"),
        WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        (screenSizex - buttonSizex) / 2, screenSizey - buttonSizey * 5, buttonSizex, buttonSizey, 
        hwndParent, (HMENU)1, hInstance, nullptr);
    HWND stopButton = CreateWindow(L"BUTTON", L"STOP",
        WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        (screenSizex - buttonSizex) / 2, screenSizey - buttonSizey * 4, buttonSizex, buttonSizey, 
        hwndParent, (HMENU)2, hInstance, nullptr);
    HWND resetButton = CreateWindow(L"BUTTON", L"RESET",
        WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        (screenSizex - buttonSizex) / 2, screenSizey - buttonSizey * 3, buttonSizex, buttonSizey, 
        hwndParent, (HMENU)3, hInstance, nullptr);
    HFONT font = CreateFont(48, 0, 0, 0, 0,
        NULL, NULL, NULL, DEFAULT_CHARSET,
        OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("Times New Roman"));
    SendMessage(okButton, WM_SETFONT, (WPARAM) font, TRUE);
    SendMessage(stopButton, WM_SETFONT, (WPARAM)font, TRUE);
    SendMessage(resetButton, WM_SETFONT, (WPARAM)font, TRUE);
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
    static bool started = false, stopped = false;
    static RECT* timeRect;
    timeRect = (RECT*)malloc(sizeof(RECT));
    timeRect->left = 30;
    timeRect->right = 270;
    timeRect->top = 300;
    timeRect->bottom = 370;
    static HDC hdcText;
    hdcText = GetDC(hWnd);
    static HFONT font = CreateFont(72, 0, 0, 0, 0,
        NULL, NULL, NULL, DEFAULT_CHARSET,
        OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("Times New Roman"));
    SelectObject(hdcText, font);
    switch (message)
    {
    case WM_CREATE:
        SetClassLongW(hWnd, GCLP_HBRBACKGROUND, (LONG)CreateSolidBrush(BCKGND_COLOR));
        break;
    case WM_COMMAND:
        if (wParam == 1) SendMessage(hWnd, START_TIMER, NULL, NULL);
        if (wParam == 2) SendMessage(hWnd, STOP_TIMER, NULL, NULL);
        if (wParam == 3) SendMessage(hWnd, RESET_TIMER, NULL, NULL);
        break;
    case START_TIMER:
        if (!started) {
            started = true;
            SetTimer(hWnd, NULL, 1000, NULL);
            DrawTime(hWnd, false, false);
        }
        break;
    case RESET_TIMER:
        InvalidateRect(hWnd, timeRect, true);
        KillTimer(hWnd, NULL);
        SetTimer(hWnd, NULL, 1000, NULL);
        ShowTime(hWnd, true, false, timeRect, font, hdcText);
        DrawTime(hWnd, true, false);
        break;
    case STOP_TIMER:
        started = false;
        KillTimer(hWnd, NULL);
        ShowTime(hWnd, false, true, timeRect, font, hdcText);
        DrawTime(hWnd, false, true);
        break;
    case WM_TIMER:
        InvalidateRect(hWnd, timeRect, false);
        ShowTime(hWnd, false, false, timeRect, font, hdcText);
        DrawTime(hWnd, false, false);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void ShowTime(HWND hWnd, bool reset, bool stop, RECT* timeRect, HFONT font, HDC hdc) {
    static int counter = GetTickCount();
    static int currentTime;
    static char* minutes = new char[2] { '1', '0' };
    static char* seconds = (char*)malloc(2 * sizeof(char));
    static char* milliseconds = (char*)malloc(2 * sizeof(char));
    static char* time = (char*)malloc(8 * sizeof(char));
    static int min, sec, ms;
    static bool stopped = false;
    stopped = stop;
    if (!stopped) {
        if (reset) {
            counter = GetTickCount();
            DrawTextA(hdc, "00:00:00", 8, timeRect, DT_CENTER);
            return;
        }
        currentTime = GetTickCount() - counter;
        min = currentTime / 60000;
        sec = ((currentTime) / 1000) % 60;
        ms = currentTime % 1000;
        time[0] = (char)min / 10 + 48;
        time[1] = (char)min % 10 + 48;
        time[2] = ':';
        time[3] = (char)sec / 10 + 48;
        time[4] = (char)sec % 10 + 48;
        time[5] = ':';
        time[6] = (char)(ms / 100) + 48;
        time[7] = (char)(ms % 100 / 10) + 48;
        SetBkColor(hdc, BCKGND_COLOR);
        SetTextColor(hdc, RGB(255, 255, 255));
        DrawTextA(hdc, time, 8, timeRect, DT_CENTER);
    }

}

void DrawTime(HWND hwnd, bool reset, bool stop) {
    static int secondCounter = 0;
    static int currentTime;
    static int time = (GetTickCount() + 500);
    static bool stopped = false;
    static RECT twelveRect = { 140, 55, 160, 70 }, threeRect = { 235, 145, 245, 160 }, sixRect = { 140, 230, 160, 245 }, nineRect = {50, 145, 70, 160};
    stopped = stop;
    if (!stopped) {
        if (reset) {
            time = GetTickCount();
            secondCounter = 0;
            return;
        }
        HDC hdcDraw = GetDC(hwnd);
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
        SelectObject(hdcDraw, hBrush);
        Ellipse(hdcDraw, 50, 50, 250, 250);
        hBrush = CreateSolidBrush(BCKGND_COLOR);
        SelectObject(hdcDraw, hBrush);
        Ellipse(hdcDraw, 55, 55, 245, 245);
        SetBkColor(hdcDraw, BCKGND_COLOR);
        SetTextColor(hdcDraw, RGB(255, 255, 255));
        DrawTextA(hdcDraw, "12", 2, &twelveRect, DT_CENTER);
        DrawTextA(hdcDraw, "3", 1, &threeRect, DT_CENTER);
        DrawTextA(hdcDraw, "6", 1, &sixRect, DT_CENTER);
        DrawTextA(hdcDraw, "9", 1, &nineRect, DT_CENTER);
        HPEN hPen = CreatePen(PS_SOLID, 3, RGB(255, 255, 255));
        SelectObject(hdcDraw, hPen);
        MoveToEx(hdcDraw, 150, 150, NULL);
        currentTime = (GetTickCount() - time);
        if (currentTime >= 1000) {
            time = GetTickCount();
            secondCounter++;
        }
        int tempCounter = secondCounter % 15;
        if (secondCounter % 60 < 15) {
            int tempX = 150 + tempCounter * 100 / 15, tempY = 50 + tempCounter * 100 / 15;
            LineTo(hdcDraw, tempX, tempY);
        }
        else if (secondCounter % 60 >= 15 && secondCounter % 60 < 30) {
            LineTo(hdcDraw, 250 - tempCounter * 100 / 15, 150 + tempCounter * 100 / 15);
        }
        else if (secondCounter % 60 >= 30 && secondCounter % 60 < 45) {
            LineTo(hdcDraw, 150 - tempCounter * 100 / 15, 250 - tempCounter * 100 / 15);
        }
        else if (secondCounter % 60 >= 45) {
            LineTo(hdcDraw, 50 + tempCounter * 100 / 15, 150 - tempCounter * 100 / 15);
        }
        ReleaseDC(hwnd, hdcDraw);
    }
}