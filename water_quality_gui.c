#include <windows.h>
#include <stdio.h>
#include <math.h>

#define ID_VOLTAGE 101
#define ID_CELL    102
#define ID_CURRENT 103
#define ID_TEMP    104
#define ID_CALC    105
#define ID_CLEAR   106

HWND hVoltage, hCell, hCurrent, hTemp;
HWND hConductance, hEC, hTDS, hQuality;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            HFONT font = CreateFontA(
                20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");

            HFONT titleFont = CreateFontA(
                30, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");

            CreateWindowA("STATIC", "WATER QUALITY CHECKER",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                20, 20, 740, 45, hwnd, NULL, NULL, NULL);
            HWND title = GetDlgItem(hwnd, 0);
            SendMessageA(title, WM_SETFONT, (WPARAM)titleFont, TRUE);

            CreateWindowA("STATIC", "Electrical Conductivity & TDS Analysis",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                20, 65, 740, 30, hwnd, NULL, NULL, NULL);

            CreateWindowA("STATIC", "INPUT VALUES",
                WS_VISIBLE | WS_CHILD,
                45, 120, 300, 30, hwnd, NULL, NULL, NULL);

            CreateWindowA("STATIC", "Supply Voltage (V)",
                WS_VISIBLE | WS_CHILD,
                50, 165, 220, 28, hwnd, NULL, NULL, NULL);
            hVoltage = CreateWindowA("EDIT", "3.0",
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                280, 160, 180, 32, hwnd, (HMENU)ID_VOLTAGE, NULL, NULL);

            CreateWindowA("STATIC", "Cell Constant K (1/cm)",
                WS_VISIBLE | WS_CHILD,
                50, 210, 220, 28, hwnd, NULL, NULL, NULL);
            hCell = CreateWindowA("EDIT", "1.0",
                WS_VISIBLE | WS_CHILD | WS_BORDER,
                280, 205, 180, 32, hwnd, (HMENU)ID_CELL, NULL, NULL);

            CreateWindowA("STATIC", "Meter Current (uA)",
                WS_VISIBLE | WS_CHILD,
                50, 255, 220, 28, hwnd, NULL, NULL, NULL);
            hCurrent = CreateWindowA("EDIT", "100",
                WS_VISIBLE | WS_CHILD | WS_BORDER,
                280, 250, 180, 32, hwnd, (HMENU)ID_CURRENT, NULL, NULL);

            CreateWindowA("STATIC", "Water Temperature (C)",
                WS_VISIBLE | WS_CHILD,
                50, 300, 220, 28, hwnd, NULL, NULL, NULL);
            hTemp = CreateWindowA("EDIT", "25",
                WS_VISIBLE | WS_CHILD | WS_BORDER,
                280, 295, 180, 32, hwnd, (HMENU)ID_TEMP, NULL, NULL);

            CreateWindowA("BUTTON", "CALCULATE",
                WS_VISIBLE | WS_CHILD,
                500, 195, 180, 50, hwnd, (HMENU)ID_CALC, NULL, NULL);

            CreateWindowA("BUTTON", "CLEAR",
                WS_VISIBLE | WS_CHILD,
                500, 255, 180, 45, hwnd, (HMENU)ID_CLEAR, NULL, NULL);

            CreateWindowA("STATIC", "RESULTS",
                WS_VISIBLE | WS_CHILD,
                45, 370, 300, 30, hwnd, NULL, NULL, NULL);

            CreateWindowA("STATIC", "Conductance:",
                WS_VISIBLE | WS_CHILD, 50, 415, 180, 28, hwnd, NULL, NULL, NULL);
            hConductance = CreateWindowA("STATIC", "---",
                WS_VISIBLE | WS_CHILD, 250, 415, 300, 28, hwnd, NULL, NULL, NULL);

            CreateWindowA("STATIC", "EC at 25 C:",
                WS_VISIBLE | WS_CHILD, 50, 455, 180, 28, hwnd, NULL, NULL, NULL);
            hEC = CreateWindowA("STATIC", "---",
                WS_VISIBLE | WS_CHILD, 250, 455, 300, 28, hwnd, NULL, NULL, NULL);

            CreateWindowA("STATIC", "TDS:",
                WS_VISIBLE | WS_CHILD, 50, 495, 180, 28, hwnd, NULL, NULL, NULL);
            hTDS = CreateWindowA("STATIC", "---",
                WS_VISIBLE | WS_CHILD, 250, 495, 300, 28, hwnd, NULL, NULL, NULL);

            CreateWindowA("STATIC", "Water Quality:",
                WS_VISIBLE | WS_CHILD, 50, 535, 180, 28, hwnd, NULL, NULL, NULL);
            hQuality = CreateWindowA("STATIC", "---",
                WS_VISIBLE | WS_CHILD, 250, 535, 400, 28, hwnd, NULL, NULL, NULL);

            CreateWindowA("STATIC",
                "Temperature compensation: 2%/C    |    TDS factor: 0.5",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                50, 585, 680, 25, hwnd, NULL, NULL, NULL);

            SendMessageA(hVoltage, WM_SETFONT, (WPARAM)font, TRUE);
            SendMessageA(hCell, WM_SETFONT, (WPARAM)font, TRUE);
            SendMessageA(hCurrent, WM_SETFONT, (WPARAM)font, TRUE);
            SendMessageA(hTemp, WM_SETFONT, (WPARAM)font, TRUE);
            SendMessageA(hConductance, WM_SETFONT, (WPARAM)font, TRUE);
            SendMessageA(hEC, WM_SETFONT, (WPARAM)font, TRUE);
            SendMessageA(hTDS, WM_SETFONT, (WPARAM)font, TRUE);
            SendMessageA(hQuality, WM_SETFONT, (WPARAM)font, TRUE);
        }
        break;

        case WM_COMMAND:
        {
            if (LOWORD(wParam) == ID_CALC)
            {
                char s[64];
                double v, k, ua, temp;
                double g, ec, ec25, tds;

                GetWindowTextA(hVoltage, s, sizeof(s));
                v = atof(s);
                GetWindowTextA(hCell, s, sizeof(s));
                k = atof(s);
                GetWindowTextA(hCurrent, s, sizeof(s));
                ua = atof(s);
                GetWindowTextA(hTemp, s, sizeof(s));
                temp = atof(s);

                if (v <= 0 || k <= 0 || ua <= 0)
                {
                    MessageBoxA(hwnd,
                        "Please enter positive values for voltage, cell constant and current.",
                        "Invalid Input", MB_OK | MB_ICONWARNING);
                    break;
                }

                g = (ua / 1000000.0) / v;
                ec = g * k * 1000000.0;
                ec25 = ec / (1 + 0.02 * (temp - 25));
                tds = ec25 * 0.5;

                sprintf(s, "%.2f uS", g * 1000000.0);
                SetWindowTextA(hConductance, s);

                sprintf(s, "%.1f uS/cm", ec25);
                SetWindowTextA(hEC, s);

                sprintf(s, "%.0f ppm", tds);
                SetWindowTextA(hTDS, s);

                if (tds < 300)
                    SetWindowTextA(hQuality, "Excellent");
                else if (tds < 600)
                    SetWindowTextA(hQuality, "Good");
                else if (tds < 900)
                    SetWindowTextA(hQuality, "Fair");
                else if (tds < 1200)
                    SetWindowTextA(hQuality, "Poor");
                else
                    SetWindowTextA(hQuality, "Unacceptable");
            }

            if (LOWORD(wParam) == ID_CLEAR)
            {
                SetWindowTextA(hVoltage, "");
                SetWindowTextA(hCell, "");
                SetWindowTextA(hCurrent, "");
                SetWindowTextA(hTemp, "");
                SetWindowTextA(hConductance, "---");
                SetWindowTextA(hEC, "---");
                SetWindowTextA(hTDS, "---");
                SetWindowTextA(hQuality, "---");
            }
        }
        break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProcA(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    const char CLASS_NAME[] = "WaterQualityChecker";

    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(
        0, CLASS_NAME, "Water Quality Checker",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 680,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
        return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return 0;
}
