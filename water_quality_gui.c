/*
 * Water Quality Checker - styled Win32 GUI
 * Build (MinGW):  gcc water_quality_gui.c -o water_quality_gui.exe -mwindows
 * Build (MSVC):   cl water_quality_gui.c user32.lib gdi32.lib
 */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ID_VOLTAGE 101
#define ID_CELL    102
#define ID_CURRENT 103
#define ID_TEMP    104
#define ID_CALC    105
#define ID_CLEAR   106

#define CLIENT_W 780
#define CLIENT_H 650

/* ---------- colour palette ---------- */
#define C_BG        RGB(15, 23, 42)
#define C_CARD      RGB(30, 41, 59)
#define C_CARD_EDGE RGB(51, 65, 85)
#define C_INPUT     RGB(15, 23, 42)
#define C_INPUT_EDGE RGB(71, 85, 105)
#define C_ACCENT    RGB(56, 189, 248)
#define C_TEXT      RGB(241, 245, 249)
#define C_MUTED     RGB(148, 163, 184)

HWND hVoltage, hCell, hCurrent, hTemp;
HWND hConductance, hEC, hTDS, hQuality;
HWND hBtnCalc, hBtnClear;

HFONT fLabel, fEdit, fValue, fTitle, fSub, fBtn, fSmall, fHead;
HBRUSH brCard, brInput, brQuality = NULL;
COLORREF qualityColor = C_CARD;
double g_tds = -1;      /* -1 = no result yet */
int g_hot = 0;          /* 1 = calc hovered, 2 = clear hovered */

/* ---------- helpers ---------- */
static HFONT MkFont(int h, int weight)
{
    return CreateFontA(h, 0, 0, 0, weight, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
}

static HWND Label(HWND p, const char *t, int x, int y, int w, int h,
                  HFONT f, COLORREF c, DWORD extra)
{
    HWND s = CreateWindowA("STATIC", t, WS_VISIBLE | WS_CHILD | extra,
                           x, y, w, h, p, NULL, NULL, NULL);
    SendMessageA(s, WM_SETFONT, (WPARAM)f, TRUE);
    SetWindowLongPtrA(s, GWLP_USERDATA, (LONG_PTR)c);
    return s;
}

static HWND Edit(HWND p, const char *t, int x, int y, int w, int h, int id)
{
    HWND e = CreateWindowA("EDIT", t,
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL,
        x, y, w, h, p, (HMENU)(INT_PTR)id, NULL, NULL);
    SendMessageA(e, WM_SETFONT, (WPARAM)fEdit, TRUE);
    return e;
}

static COLORREF Lerp(COLORREF a, COLORREF b, int i, int n)
{
    return RGB(GetRValue(a) + (GetRValue(b) - GetRValue(a)) * i / n,
               GetGValue(a) + (GetGValue(b) - GetGValue(a)) * i / n,
               GetBValue(a) + (GetBValue(b) - GetBValue(a)) * i / n);
}

static void GradientH(HDC dc, int x, int y, int w, int h, COLORREF a, COLORREF b)
{
    int i;
    for (i = 0; i < w; i++)
    {
        RECT r = { x + i, y, x + i + 1, y + h };
        SetDCBrushColor(dc, Lerp(a, b, i, w));
        FillRect(dc, &r, (HBRUSH)GetStockObject(DC_BRUSH));
    }
}

static void Card(HDC dc, int l, int t, int r, int b)
{
    HPEN pen = CreatePen(PS_SOLID, 1, C_CARD_EDGE);
    HPEN oldP = (HPEN)SelectObject(dc, pen);
    HBRUSH oldB = (HBRUSH)SelectObject(dc, brCard);
    RoundRect(dc, l, t, r, b, 22, 22);
    SelectObject(dc, oldP);
    SelectObject(dc, oldB);
    DeleteObject(pen);
}

static void GetQuality(double tds, const char **name, COLORREF *col)
{
    if      (tds < 300)  { *name = "Excellent";    *col = RGB(34, 197, 94);  }
    else if (tds < 600)  { *name = "Good";         *col = RGB(132, 204, 22); }
    else if (tds < 900)  { *name = "Fair";         *col = RGB(250, 204, 21); }
    else if (tds < 1200) { *name = "Poor";         *col = RGB(249, 115, 22); }
    else                 { *name = "Unacceptable"; *col = RGB(239, 68, 68);  }
}

/* ---------- painting ---------- */
static void PaintAll(HWND hwnd, HDC dc)
{
    HWND edits[4];
    int i;
    RECT all = { 0, 0, CLIENT_W, CLIENT_H };
    HBRUSH bg = CreateSolidBrush(C_BG);
    FillRect(dc, &all, bg);
    DeleteObject(bg);

    /* header banner */
    GradientH(dc, 0, 0, CLIENT_W, 110, RGB(8, 145, 178), RGB(99, 60, 220));
    {
        RECT stripe = { 0, 110, CLIENT_W, 114 };
        SetDCBrushColor(dc, C_ACCENT);
        FillRect(dc, &stripe, (HBRUSH)GetStockObject(DC_BRUSH));
    }

    /* water droplet logo */
    {
        HBRUSH white = CreateSolidBrush(RGB(255, 255, 255));
        HBRUSH shine = CreateSolidBrush(RGB(186, 230, 253));
        HBRUSH oB = (HBRUSH)SelectObject(dc, white);
        HPEN oP = (HPEN)SelectObject(dc, GetStockObject(NULL_PEN));
        POINT tri[3] = { {64, 22}, {46, 55}, {82, 55} };
        Ellipse(dc, 44, 42, 85, 83);
        Polygon(dc, tri, 3);
        SelectObject(dc, shine);
        Ellipse(dc, 52, 58, 60, 70);
        SelectObject(dc, oB);
        SelectObject(dc, oP);
        DeleteObject(white);
        DeleteObject(shine);
    }

    SetBkMode(dc, TRANSPARENT);
    SelectObject(dc, fTitle);
    SetTextColor(dc, RGB(255, 255, 255));
    TextOutA(dc, 105, 16, "WATER QUALITY CHECKER", 21);
    SelectObject(dc, fSub);
    SetTextColor(dc, RGB(224, 242, 254));
    TextOutA(dc, 107, 64, "Electrical Conductivity & TDS Analysis", 38);

    /* cards */
    Card(dc, 30, 135, 750, 375);
    Card(dc, 30, 395, 750, 610);

    /* input boxes */
    edits[0] = hVoltage; edits[1] = hCell; edits[2] = hCurrent; edits[3] = hTemp;
    for (i = 0; i < 4; i++)
    {
        RECT r;
        HPEN pen;
        GetWindowRect(edits[i], &r);
        MapWindowPoints(NULL, hwnd, (POINT *)&r, 2);
        InflateRect(&r, 8, 6);
        pen = CreatePen(PS_SOLID, GetFocus() == edits[i] ? 2 : 1,
                        GetFocus() == edits[i] ? C_ACCENT : C_INPUT_EDGE);
        SelectObject(dc, pen);
        SelectObject(dc, brInput);
        RoundRect(dc, r.left, r.top, r.right, r.bottom, 12, 12);
        SelectObject(dc, GetStockObject(BLACK_PEN));
        DeleteObject(pen);
    }

    /* TDS scale meter */
    {
        static const COLORREF zone[5] = {
            RGB(34,197,94), RGB(132,204,22), RGB(250,204,21),
            RGB(249,115,22), RGB(239,68,68) };
        static const char *tick[5] = { "0", "300", "600", "900", "1200" };
        const int x0 = 470, bw = 260, by = 485, bh = 18, zw = bw / 5;

        SelectObject(dc, fHead);
        SetTextColor(dc, C_ACCENT);
        TextOutA(dc, x0, 408, "TDS SCALE (ppm)", 15);

        for (i = 0; i < 5; i++)
        {
            RECT z = { x0 + i * zw, by, x0 + (i + 1) * zw, by + bh };
            SetDCBrushColor(dc, zone[i]);
            FillRect(dc, &z, (HBRUSH)GetStockObject(DC_BRUSH));
        }
        SelectObject(dc, fSmall);
        SetTextColor(dc, C_MUTED);
        for (i = 0; i < 5; i++)
        {
            RECT t = { x0 + i * zw - 20, by + bh + 4, x0 + i * zw + 20, by + bh + 24 };
            DrawTextA(dc, tick[i], -1, &t, DT_CENTER | DT_SINGLELINE);
        }
        if (g_tds >= 0)
        {
            double c = g_tds > 1500 ? 1500 : g_tds;
            int mx = x0 + (int)(c / 1500.0 * bw);
            POINT m[3] = { {mx, by - 2}, {mx - 8, by - 16}, {mx + 8, by - 16} };
            HBRUSH white = CreateSolidBrush(RGB(255, 255, 255));
            HBRUSH oB = (HBRUSH)SelectObject(dc, white);
            HPEN oP = (HPEN)SelectObject(dc, GetStockObject(NULL_PEN));
            Polygon(dc, m, 3);
            SelectObject(dc, oB);
            SelectObject(dc, oP);
            DeleteObject(white);
        }
    }

    /* footer */
    {
        RECT f = { 0, 618, CLIENT_W, 642 };
        SelectObject(dc, fSmall);
        SetTextColor(dc, C_MUTED);
        DrawTextA(dc, "Temperature compensation: 2%/C    |    TDS factor: 0.5",
                  -1, &f, DT_CENTER | DT_SINGLELINE);
    }
}

static void DrawButton(DRAWITEMSTRUCT *d)
{
    int isCalc = (d->CtlID == ID_CALC);
    int hot = (g_hot == (isCalc ? 1 : 2));
    int down = (d->itemState & ODS_SELECTED);
    COLORREF col;
    HBRUSH bg = CreateSolidBrush(C_CARD);
    HBRUSH br;
    HPEN pen = (HPEN)GetStockObject(NULL_PEN);
    RECT r = d->rcItem;
    const char *txt = isCalc ? "CALCULATE" : "CLEAR";

    if (isCalc) col = down ? RGB(2,132,199) : hot ? RGB(56,189,248) : RGB(14,165,233);
    else        col = down ? RGB(30,41,59)  : hot ? RGB(71,85,105)  : RGB(51,65,85);

    FillRect(d->hDC, &r, bg);
    br = CreateSolidBrush(col);
    SelectObject(d->hDC, br);
    SelectObject(d->hDC, pen);
    RoundRect(d->hDC, r.left, r.top, r.right, r.bottom, 16, 16);

    SetBkMode(d->hDC, TRANSPARENT);
    SetTextColor(d->hDC, isCalc ? RGB(7, 20, 40) : C_TEXT);
    SelectObject(d->hDC, fBtn);
    if (down) OffsetRect(&r, 0, 1);
    DrawTextA(d->hDC, txt, -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    DeleteObject(bg);
    DeleteObject(br);
}

/* ---------- window procedure ---------- */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            fLabel = MkFont(20, FW_NORMAL);
            fEdit  = MkFont(22, FW_NORMAL);
            fValue = MkFont(22, FW_BOLD);
            fTitle = MkFont(36, FW_BOLD);
            fSub   = MkFont(20, FW_NORMAL);
            fBtn   = MkFont(22, FW_BOLD);
            fSmall = MkFont(16, FW_NORMAL);
            fHead  = MkFont(18, FW_BOLD);
            brCard  = CreateSolidBrush(C_CARD);
            brInput = CreateSolidBrush(C_INPUT);

            Label(hwnd, "INPUT VALUES", 52, 147, 300, 26, fHead, C_ACCENT, 0);

            Label(hwnd, "Supply Voltage (V)",       52, 190, 220, 28, fLabel, C_MUTED, 0);
            hVoltage = Edit(hwnd, "3.0", 300, 190, 150, 28, ID_VOLTAGE);
            Label(hwnd, "Cell Constant K (1/cm)",   52, 235, 220, 28, fLabel, C_MUTED, 0);
            hCell    = Edit(hwnd, "1.0", 300, 235, 150, 28, ID_CELL);
            Label(hwnd, "Meter Current (uA)",       52, 280, 220, 28, fLabel, C_MUTED, 0);
            hCurrent = Edit(hwnd, "100", 300, 280, 150, 28, ID_CURRENT);
            Label(hwnd, "Water Temperature (C)",    52, 325, 220, 28, fLabel, C_MUTED, 0);
            hTemp    = Edit(hwnd, "25",  300, 325, 150, 28, ID_TEMP);

            hBtnCalc = CreateWindowA("BUTTON", "CALCULATE",
                WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_OWNERDRAW,
                500, 195, 220, 58, hwnd, (HMENU)ID_CALC, NULL, NULL);
            hBtnClear = CreateWindowA("BUTTON", "CLEAR",
                WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_OWNERDRAW,
                500, 268, 220, 48, hwnd, (HMENU)ID_CLEAR, NULL, NULL);
            Label(hwnd, "Tip: use 25 if you did not measure the temperature.",
                  500, 328, 225, 40, fSmall, C_MUTED, 0);

            Label(hwnd, "RESULTS", 52, 407, 300, 26, fHead, C_ACCENT, 0);

            Label(hwnd, "Conductance:",  52, 450, 170, 28, fLabel, C_MUTED, 0);
            hConductance = Label(hwnd, "---", 230, 450, 220, 28, fValue, C_TEXT, 0);
            Label(hwnd, "EC at 25 C:",   52, 490, 170, 28, fLabel, C_MUTED, 0);
            hEC          = Label(hwnd, "---", 230, 490, 220, 28, fValue, C_TEXT, 0);
            Label(hwnd, "TDS:",          52, 530, 170, 28, fLabel, C_MUTED, 0);
            hTDS         = Label(hwnd, "---", 230, 530, 220, 28, fValue, C_TEXT, 0);
            Label(hwnd, "Water Quality:", 52, 568, 170, 28, fLabel, C_MUTED, 0);
            hQuality     = Label(hwnd, "---", 230, 566, 190, 30, fValue, C_TEXT, SS_CENTER);

            SetTimer(hwnd, 1, 40, NULL);   /* hover tracking for buttons */
        }
        return 0;

        case WM_TIMER:
        {
            POINT p;
            HWND h;
            int hot;
            GetCursorPos(&p);
            h = WindowFromPoint(p);
            hot = (h == hBtnCalc) ? 1 : (h == hBtnClear) ? 2 : 0;
            if (hot != g_hot)
            {
                g_hot = hot;
                InvalidateRect(hBtnCalc, NULL, FALSE);
                InvalidateRect(hBtnClear, NULL, FALSE);
            }
        }
        return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            HDC mem = CreateCompatibleDC(dc);
            HBITMAP bmp = CreateCompatibleBitmap(dc, CLIENT_W, CLIENT_H);
            HBITMAP old = (HBITMAP)SelectObject(mem, bmp);
            PaintAll(hwnd, mem);
            BitBlt(dc, 0, 0, CLIENT_W, CLIENT_H, mem, 0, 0, SRCCOPY);
            SelectObject(mem, old);
            DeleteObject(bmp);
            DeleteDC(mem);
            EndPaint(hwnd, &ps);
        }
        return 0;

        case WM_CTLCOLORSTATIC:
        {
            HDC dc = (HDC)wParam;
            HWND h = (HWND)lParam;
            if (h == hQuality && brQuality)
            {
                SetTextColor(dc, RGB(15, 23, 42));
                SetBkColor(dc, qualityColor);
                return (LRESULT)brQuality;
            }
            SetTextColor(dc, (COLORREF)GetWindowLongPtrA(h, GWLP_USERDATA));
            SetBkColor(dc, C_CARD);
            return (LRESULT)brCard;
        }

        case WM_CTLCOLOREDIT:
            SetTextColor((HDC)wParam, C_TEXT);
            SetBkColor((HDC)wParam, C_INPUT);
            return (LRESULT)brInput;

        case WM_DRAWITEM:
            DrawButton((DRAWITEMSTRUCT *)lParam);
            return TRUE;

        case WM_COMMAND:
        {
            int id = LOWORD(wParam);
            int code = HIWORD(wParam);

            if (code == EN_SETFOCUS || code == EN_KILLFOCUS)
                InvalidateRect(hwnd, NULL, FALSE);   /* redraw focus outline */

            if (id == ID_CALC && code == BN_CLICKED)
            {
                char s[64];
                double v, k, ua, temp, g, ec, ec25, tds;
                const char *name;

                GetWindowTextA(hVoltage, s, sizeof(s)); v    = atof(s);
                GetWindowTextA(hCell,    s, sizeof(s)); k    = atof(s);
                GetWindowTextA(hCurrent, s, sizeof(s)); ua   = atof(s);
                GetWindowTextA(hTemp,    s, sizeof(s)); temp = atof(s);

                if (v <= 0 || k <= 0 || ua <= 0)
                {
                    MessageBoxA(hwnd,
                        "Please enter positive values for voltage, cell constant and current.",
                        "Invalid Input", MB_OK | MB_ICONWARNING);
                    break;
                }

                g    = (ua / 1000000.0) / v;
                ec   = g * k * 1000000.0;
                ec25 = ec / (1 + 0.02 * (temp - 25));
                tds  = ec25 * 0.5;

                sprintf(s, "%.2f uS", g * 1000000.0);  SetWindowTextA(hConductance, s);
                sprintf(s, "%.1f uS/cm", ec25);        SetWindowTextA(hEC, s);
                sprintf(s, "%.0f ppm", tds);           SetWindowTextA(hTDS, s);

                GetQuality(tds, &name, &qualityColor);
                if (brQuality) DeleteObject(brQuality);
                brQuality = CreateSolidBrush(qualityColor);
                SetWindowTextA(hQuality, name);
                InvalidateRect(hQuality, NULL, TRUE);

                g_tds = tds;
                InvalidateRect(hwnd, NULL, FALSE);
            }

            if (id == ID_CLEAR && code == BN_CLICKED)
            {
                SetWindowTextA(hVoltage, "");
                SetWindowTextA(hCell, "");
                SetWindowTextA(hCurrent, "");
                SetWindowTextA(hTemp, "");
                SetWindowTextA(hConductance, "---");
                SetWindowTextA(hEC, "---");
                SetWindowTextA(hTDS, "---");
                SetWindowTextA(hQuality, "---");

                if (brQuality) { DeleteObject(brQuality); brQuality = NULL; }
                g_tds = -1;
                InvalidateRect(hQuality, NULL, TRUE);
                InvalidateRect(hwnd, NULL, FALSE);
                SetFocus(hVoltage);
            }
        }
        return 0;

        case WM_DESTROY:
            KillTimer(hwnd, 1);
            DeleteObject(fLabel); DeleteObject(fEdit);  DeleteObject(fValue);
            DeleteObject(fTitle); DeleteObject(fSub);   DeleteObject(fBtn);
            DeleteObject(fSmall); DeleteObject(fHead);
            DeleteObject(brCard); DeleteObject(brInput);
            if (brQuality) DeleteObject(brQuality);
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    const char CLASS_NAME[] = "WaterQualityChecker";
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
    RECT rc = { 0, 0, CLIENT_W, CLIENT_H };
    WNDCLASSA wc = {0};
    HWND hwnd;
    MSG msg;

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = NULL;                 /* we paint everything ourselves */
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassA(&wc);

    AdjustWindowRect(&rc, style, FALSE);     /* exact client size */

    hwnd = CreateWindowExA(0, CLASS_NAME, "Water Quality Checker", style,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
        NULL, NULL, hInstance, NULL);
    if (hwnd == NULL)
        return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessageA(&msg, NULL, 0, 0) > 0)
    {
        if (!IsDialogMessageA(hwnd, &msg))   /* Tab key moves between fields */
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }
    return 0;
}
