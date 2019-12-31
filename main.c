#include <windows.h>
#include <windowsx.h>
#include <stdio.h>

const char g_szClassName[] = "myWindowClass";
// 画笔颜色按钮ID
#define PAINT_BTN_WHITE 11
#define PAINT_BTN_BLACK 12
#define PAINT_BTN_RED 13
#define PAINT_BTN_GREEN 14
#define PAINT_BTN_BLUE 15
#define PAINT_BTN_YELLOW 16
// 填充颜色按钮ID
#define FILL_BTN_WHITE 21
#define FILL_BTN_BLACK 22
#define FILL_BTN_RED 23
#define FILL_BTN_GREEN 24
#define FILL_BTN_BLUE 25
#define FILL_BTN_YELLOW 26
// 功能按键ID
#define LINE_BTN 31
#define RECT_BTN 32
#define CRICLE_BTN 33

// 全局变量
COLORREF PAINTCOLOR = RGB(255, 0, 0);      // 全局画笔颜色
COLORREF FILLCOLOR = RGB(0, 0, 0);       // 全局填充颜色
int PENWIDTH = 3;
int DO_WHAT =  LINE_BTN;

// 获取颜色的rgb值
COLORREF getColorRGB(int color) {
    switch(color)
    {
        case PAINT_BTN_WHITE:
        case FILL_BTN_WHITE:
            return RGB(255, 255, 255);
        case PAINT_BTN_BLACK:
        case FILL_BTN_BLACK:
            return RGB(0, 0, 0);
        case PAINT_BTN_RED:
        case FILL_BTN_RED:
            return RGB(255, 0, 0);
        case PAINT_BTN_GREEN:
        case FILL_BTN_GREEN:
            return RGB(0, 255, 0);
        case PAINT_BTN_BLUE:
        case FILL_BTN_BLUE:
            return RGB(0, 0, 255);
        case PAINT_BTN_YELLOW:
        case FILL_BTN_YELLOW:
            return RGB(255, 255, 0);
        default:
            return RGB(255, 255, 255);
        
    }
}
// 改变画笔颜色
void changePaintColor(int color) {
    PAINTCOLOR = getColorRGB(color);
    return;
}
// 改变填充颜色
void changeFillColor(int color) {
    FILLCOLOR = getColorRGB(color);
    return;
}
// 修改鼠标事件
void changeDrawWhat(int dowhat) {
    DO_WHAT = dowhat;
}

// 画直线, 参数：HDC, 钢笔风格，起点，终点 (其中笔色，笔宽填充色，由全局变量决定)
void CreateLine(HDC hdc, int fnPenStyle, int oX, int oY, int nX, int nY) {
    HPEN hpen = CreatePen(fnPenStyle, PENWIDTH, PAINTCOLOR);  // 实线
    HPEN oldhpen = (HPEN)SelectObject(hdc, hpen);      // 选入画笔,并保存旧画笔
    MoveToEx(hdc, oX, oY, NULL);    // 起点
	LineTo(hdc, nX, nY);            // 终点
    SelectObject(hdc, oldhpen);     // 恢复先前画笔
	DeleteObject(hpen);            // 删除自创画笔
}
// 画矩形, 参数：HDC, 钢笔风格，左顶点，右底终点 (其中笔色，填充色由全局变量决定)
void CreateRectangle(HDC hdc, int fnPenStyle, int nWidth, int ltX, int ltY, int rbX, int rbY) {
    HPEN hpen, oldhpen;
    HBRUSH hbrush, oldhbrush;
	hpen = CreatePen(fnPenStyle, nWidth, PAINTCOLOR);          // 创建空画笔
    hbrush = CreateSolidBrush(FILLCOLOR);                    // 填充颜色
    oldhpen = (HPEN)SelectObject(hdc, hpen);                     // 选入画笔, 并保存旧画笔
    oldhbrush = (HBRUSH)SelectObject(hdc, hbrush);              // 选入画刷，并保存旧画刷
    Rectangle(hdc, ltX, ltY, rbX, rbY);
    SelectObject(hdc, oldhpen);
	SelectObject(hdc, oldhbrush);
    DeleteObject(hbrush);  //删除画刷
    DeleteObject(hpen);            // 删除自创画笔
}
// 绘制(椭)圆, 参数：HDC, 外切矩形左上、右下坐标
void CreateCricle(HDC hdc, int ltX, int ltY, int rbX, int rbY) {
    HBRUSH hbrush, oldhbrush;
    SetBkColor(hdc, FILLCOLOR);     // 设置当前背景颜色
    hbrush = CreateHatchBrush(HS_CROSS, PAINTCOLOR);
    oldhbrush = (HBRUSH)SelectObject(hdc, hbrush);
    Ellipse(hdc, ltX, ltY, rbX, rbY);   // 外切矩形左上、右下坐标
    SelectObject(hdc, oldhbrush);
	DeleteObject(hbrush);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc;    // 开始绘制
    int isMouseHold = 0;    // 鼠标是否按下
    int wmId, wmEvent;
    int start_x, start_y, end_x, end_y;
    switch (msg)
    {
        case WM_CREATE:
        {
            // 创建画笔颜色选择按钮
            HWND paint_color = CreateWindow(TEXT("static"), 
                TEXT("画笔颜色"),
                WS_CHILD | WS_VISIBLE | BS_CENTER,
                20, 20, 60, 30,
                hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND red_paint_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("红"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                90 /*X坐标*/, 20 /*Y坐标*/, 20 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)PAINT_BTN_RED, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND green_paint_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("绿"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                120 /*X坐标*/, 20 /*Y坐标*/, 20 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)PAINT_BTN_GREEN, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND blue_paint_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("蓝"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                150 /*X坐标*/, 20 /*Y坐标*/, 20 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)PAINT_BTN_BLUE, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND black_paint_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("黑"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                180 /*X坐标*/, 20 /*Y坐标*/, 20 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)PAINT_BTN_BLACK, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND white_paint_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("白"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                210 /*X坐标*/, 20 /*Y坐标*/, 20 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)PAINT_BTN_WHITE, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND yellow_paint_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("黄"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                240 /*X坐标*/, 20 /*Y坐标*/, 20 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)PAINT_BTN_YELLOW, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            // 创建填充颜色选择按钮
            HWND fill_color = CreateWindow(TEXT("static"), 
                TEXT("填充颜色"),
                WS_CHILD | WS_VISIBLE | BS_CENTER,
                20, 60, 60, 30,
                hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND red_fill_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("红"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                90 /*X坐标*/, 60 /*Y坐标*/, 20 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)FILL_BTN_RED, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND green_fill_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("绿"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                120 /*X坐标*/, 60 /*Y坐标*/, 20 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)FILL_BTN_GREEN, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND blue_fill_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("蓝"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                150 /*X坐标*/, 60 /*Y坐标*/, 20 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)FILL_BTN_BLUE, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND black_fill_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("黑"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                180 /*X坐标*/, 60 /*Y坐标*/, 20 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)FILL_BTN_BLACK, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND white_fill_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("白"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                210 /*X坐标*/, 60 /*Y坐标*/, 20 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)FILL_BTN_WHITE, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND yellow_fill_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("黄"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                240 /*X坐标*/, 60 /*Y坐标*/, 20 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)FILL_BTN_YELLOW, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            // 创建功能按钮
            HWND choice_shape = CreateWindow(TEXT("static"), 
                TEXT("选择形状"),
                WS_CHILD | WS_VISIBLE | BS_CENTER,
                20, 100, 60, 30,
                hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND line_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("直线"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                90 /*X坐标*/, 100 /*Y坐标*/, 40 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)LINE_BTN, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND rect_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("矩形"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                140 /*X坐标*/, 100 /*Y坐标*/, 40 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)RECT_BTN, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND cricle_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("椭圆"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                190 /*X坐标*/, 100 /*Y坐标*/, 40 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)CRICLE_BTN, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            break;
        }
        case WM_COMMAND:
        {
            wmId = GET_X_LPARAM(wParam);
            wmEvent = GET_Y_LPARAM(wParam);
            switch(wmId)
            {
                // 改变画笔颜色
                case PAINT_BTN_WHITE:
                    changePaintColor(PAINT_BTN_WHITE);break;
                case PAINT_BTN_BLACK:
                    changePaintColor(PAINT_BTN_BLACK);break;
                case PAINT_BTN_RED:
                    changePaintColor(PAINT_BTN_RED);break;
                case PAINT_BTN_GREEN:
                    changePaintColor(PAINT_BTN_GREEN);break;
                case PAINT_BTN_BLUE:
                    changePaintColor(PAINT_BTN_BLUE);break;
                case PAINT_BTN_YELLOW:
                    changePaintColor(PAINT_BTN_YELLOW); break;
                // 改变填充颜色
                case FILL_BTN_WHITE:
                    changeFillColor(FILL_BTN_WHITE);break;
                case FILL_BTN_BLACK:
                    changeFillColor(FILL_BTN_BLACK);break;
                case FILL_BTN_RED:
                    changeFillColor(FILL_BTN_RED);break;
                case FILL_BTN_GREEN:
                    changeFillColor(FILL_BTN_GREEN);break;
                case FILL_BTN_BLUE:
                    changeFillColor(FILL_BTN_BLUE);break;
                case FILL_BTN_YELLOW:
                    changeFillColor(FILL_BTN_YELLOW); break;
                // 改变画图事件
                case LINE_BTN:
                    changeDrawWhat(FILL_BTN_WHITE);break;
                case RECT_BTN:
                    changeDrawWhat(RECT_BTN);break;
                case CRICLE_BTN:
                    changeDrawWhat(CRICLE_BTN);break;
                default:
                    break;
            }
            break;
        }
        case WM_LBUTTONDOWN:        // 鼠标左键按下
        {
            isMouseHold = 1;
            start_x = GET_X_LPARAM(lParam); /*此处使用该方法是避免鼠标移除操作窗口，而使得获取位置发生错误*/
            start_y= GET_Y_LPARAM(lParam); /*使用该方法需要 包含头文件<windowsx.h>*/
            end_x = start_x;
            end_y = start_y;
            break;
        }
        case WM_MOUSEMOVE:          // 鼠标不松开移动
        {
            if(isMouseHold)          // bMouseDown记录操作过程中是否按下鼠标
            {
                hdc = GetDC(hwnd);
                SetROP2(hdc, R2_NOT);
                MoveToEx(hdc, start_x, start_y, NULL);
                LineTo(hdc, end_x, end_y);
                end_x = GET_X_LPARAM(lParam);
                end_y = GET_Y_LPARAM(lParam);
                MoveToEx(hdc, start_x, start_y, NULL);
                LineTo(hdc, end_x, end_y);
                ReleaseDC(hwnd, hdc);
            }
        }
        case WM_LBUTTONUP:          // 鼠标左键松开
        {
            isMouseHold = 0;
            hdc = GetDC(hwnd);
            end_x = GET_X_LPARAM(lParam);
            end_y = GET_Y_LPARAM(lParam);
            switch(DO_WHAT)
            {
                case LINE_BTN:
                    CreateLine(hdc, 4, start_x, start_y, end_x, end_y);break;
                case RECT_BTN:
                    CreateRectangle(hdc, PS_NULL, 1, start_x, start_y, end_x, end_y);break;
                case CRICLE_BTN:
                    CreateCricle(hdc, start_x, start_y, end_x, end_y);break;
                default:
                    break;
            }
            ReleaseDC(hwnd,hdc);
            break;
        }
        // case WM_PAINT:
        // {
        //     hdc = BeginPaint(hwnd, &ps);
        //     // 绘制直线
        //     CreateLine(hdc, PS_SOLID, 30, 200, 70, 220);
        //     // 绘制矩形
        //     CreateRectangle(hdc, PS_NULL, 1, 10, 240, 70, 260);
        //     // 绘制(椭)圆
        //     CreateCricle(hdc, 10, 260, 70, 280);
        //     EndPaint(hwnd, &ps);
		//     break;
        // }
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    //Step 1: Registering the Window Class
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "The title of my window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 1000,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}