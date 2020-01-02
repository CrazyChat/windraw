#include <windows.h>
#include <windowsx.h>

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
#define FILL_BTN_NULL 20
// 功能按键ID
#define LINE_BTN 31
#define RECT_BTN 32
#define Ellipse_BTN 33
#define CLEAN_BTN 34
#define BACK_BTN 35
#define FORWARD_BTN 36
// 画笔类型
#define SOLID_BTN PS_SOLID
#define DASH_BTN PS_DASH

// 保存所有操作, todo改成动态数组
#define OPMAX 100
struct Point {
    int x;
    int y;
}startPoint, endPoint, startPoints[OPMAX], endPoints[OPMAX];  // 所有起点、终点

int nowPoint = 0;                        // 当前操作索引
int OPLENGTH = 0;                        // 已有操作最大值
COLORREF nowPenColors[OPMAX];            // 画笔颜色
COLORREF nowFillColor[OPMAX];            // 填充颜色

int nowPenStyle[OPMAX];                  // 画笔类型
int nowIsFill[OPMAX];                    // 是否填充
int nowPenDo[OPMAX];                     // 画笔功能


// 全局变量
COLORREF PAINTCOLOR = RGB(0, 0, 0);                 // 全局画笔颜色
COLORREF FILLCOLOR = RGB(255, 255, 255);            // 全局填充颜色
COLORREF oldPenColor, oldFillColor;
int PENSTYLE = SOLID_BTN, oldPenStyle;                             // 全局画笔类型
int ISFILL = 0, oldIsFill;                                     // 是否填充颜色判断
int PENWIDTH = 1;                                   // 钢笔粗细
int DO_WHAT =  LINE_BTN, oldDo_What;                            // 画笔功能
int isMouseHold = 0;                                // 鼠标是否按下
int wmId, wmEvent;
PAINTSTRUCT ps;
HDC hdc;

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
    if(color != FILL_BTN_NULL) {
        ISFILL = 1;
        FILLCOLOR = getColorRGB(color);
        return;
    }
    ISFILL = 0;
    return;
}
// 修改鼠标事件
void changeDrawWhat(int dowhat) {
    DO_WHAT = dowhat;
}
// 修改画笔类型
void changePenStyle(int style) {
    PENSTYLE = style;
}


// 画直线, 参数：HDC, 起点，终点
void CreateLine(HDC hdc, struct Point start, struct Point end) {
    HPEN hpen = CreatePen(PENSTYLE, PENWIDTH, PAINTCOLOR);  // 实线
    HPEN oldhpen = (HPEN)SelectObject(hdc, hpen);      // 选入画笔,并保存旧画笔
    MoveToEx(hdc, start.x, start.y, NULL);    // 起点
	LineTo(hdc, end.x, end.y);            // 终点
    SelectObject(hdc, oldhpen);     // 恢复先前画笔
	DeleteObject(hpen);            // 删除自创画笔
}
// 画矩形, 参数：HDC, 左顶点，右底终点
void CreateRectangle(HDC hdc, struct Point start, struct Point end) {
    HPEN hpen, oldhpen;
    HBRUSH hbrush, oldhbrush;
	hpen = CreatePen(PENSTYLE, PENWIDTH, PAINTCOLOR);          // 创建空画笔
    oldhpen = (HPEN)SelectObject(hdc, hpen);                    // 选入画笔, 并保存旧画笔
    // 判断是否填充颜色
    if(ISFILL) {
        hbrush = CreateSolidBrush(FILLCOLOR);                 // 填充颜色
    } else {
        hbrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    }
    oldhbrush = (HBRUSH)SelectObject(hdc, hbrush);              // 选入画刷，并保存旧画刷
    Rectangle(hdc, start.x, start.y, end.x, end.y);
    SelectObject(hdc, oldhpen);
	SelectObject(hdc, oldhbrush);
    DeleteObject(hpen);            // 删除自创画笔
    DeleteObject(hbrush);           //删除画刷
}
// 绘制椭圆, 参数：HDC, 外切矩形左上、右下坐标
void CreateEllipse(HDC hdc, struct Point start, struct Point end) {
    HPEN hpen, oldhpen;
    HBRUSH hbrush, oldhbrush;
    hpen = CreatePen(PENSTYLE, PENWIDTH, PAINTCOLOR);
    oldhpen = (HPEN)SelectObject(hdc, hpen);
    if(ISFILL) {
        hbrush = CreateSolidBrush(FILLCOLOR);
    } else {
        hbrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    }
    oldhbrush = (HBRUSH)SelectObject(hdc, hbrush);
    Ellipse(hdc, start.x, start.y, end.x, end.y);   // 外切矩形左上、右下坐标
    SelectObject(hdc, oldhpen);
    SelectObject(hdc, oldhbrush);
	DeleteObject(hbrush);
    DeleteObject(hpen);
}
// 橡皮擦, 参数：HDC, 左顶点，右底终点
void CreateEraser(HDC hdc, struct Point start, struct Point end) {
    HPEN hpen, oldhpen;
    HBRUSH hbrush, oldhbrush;
	hpen = CreatePen(PS_NULL, 0, RGB(255, 255, 255));
    oldhpen = (HPEN)SelectObject(hdc, hpen);
    hbrush = CreateSolidBrush(RGB(255, 255, 255));
    oldhbrush = (HBRUSH)SelectObject(hdc, hbrush);
    Rectangle(hdc, start.x, start.y, end.x, end.y);
    SelectObject(hdc, oldhpen);
	SelectObject(hdc, oldhbrush);
    DeleteObject(hpen);
    DeleteObject(hbrush);
}
// 保存所有旧样式
void SaveOldStyle() {
    oldPenColor = PAINTCOLOR;
    oldFillColor = FILLCOLOR;
    oldPenStyle = PENSTYLE;
    oldIsFill = ISFILL;
    oldDo_What = DO_WHAT;
}
// 恢复旧样式
void ReOldStyle() {
    PAINTCOLOR = oldPenColor;
    FILLCOLOR = oldFillColor;
    PENSTYLE = oldPenStyle;
    ISFILL = oldIsFill;
    DO_WHAT = oldDo_What;
}
// 调用事件, 参数：DO_WAHT
void DoFunc(int id) {
    switch(id)
    {
        case LINE_BTN:
            CreateLine(hdc, startPoint, endPoint);break;
        case RECT_BTN:
            CreateRectangle(hdc, startPoint, endPoint);break;
        case Ellipse_BTN:
            CreateEllipse(hdc, startPoint, endPoint);break;
        case CLEAN_BTN:
            CreateEraser(hdc, startPoint, endPoint);break;
        default:
            break;
    }
}
// 重绘
void ReDraw(HDC hdc, int i) {
    for (; i < nowPoint; i++)
    {
        startPoint.x = startPoints[i].x;
        startPoint.y = startPoints[i].y;
        endPoint.x = endPoints[i].x;
        endPoint.y = endPoints[i].y;
        PAINTCOLOR = nowPenColors[i];
        FILLCOLOR = nowFillColor[i];
        PENSTYLE = nowPenStyle[i];
        ISFILL = nowIsFill[i];
        DO_WHAT = nowPenDo[i];
        DoFunc(DO_WHAT);
    }
}
// 撤销操作
void backStep(HWND hwnd) {
    if(nowPoint > 0) {
        hdc = GetDC(hwnd);
        nowPoint--;
        startPoint.x = startPoints[nowPoint].x;
        startPoint.y = startPoints[nowPoint].y;
        endPoint.x = endPoints[nowPoint].x;
        endPoint.y = endPoints[nowPoint].y;
        SaveOldStyle();
        PAINTCOLOR = RGB(255, 255, 255);
        FILLCOLOR = RGB(255, 255, 255);
        PENSTYLE = nowPenStyle[nowPoint];
        ISFILL = nowIsFill[nowPoint];
        DO_WHAT = nowPenDo[nowPoint];
        DoFunc(DO_WHAT);
        ReDraw(hdc, 0);             // 重绘所有，以防被最后一步操作挡住的画面丢失
        // 恢复最后一步的操作样式
        ReOldStyle();
        ReleaseDC(hwnd, hdc);
    }
}
// 前进一步
void forwardStep(HWND hwnd) {
    if(nowPoint < OPLENGTH) {
        hdc = GetDC(hwnd);
        SaveOldStyle();
        // 开始恢复
        startPoint.x = startPoints[nowPoint].x;
        startPoint.y = startPoints[nowPoint].y;
        endPoint.x = endPoints[nowPoint].x;
        endPoint.y = endPoints[nowPoint].y;
        PAINTCOLOR = nowPenColors[nowPoint];
        FILLCOLOR = nowFillColor[nowPoint];
        PENSTYLE = nowPenStyle[nowPoint];
        ISFILL = nowIsFill[nowPoint];
        DO_WHAT = nowPenDo[nowPoint];
        DoFunc(DO_WHAT);
        // 恢复最后一步的操作样式
        ReOldStyle();
        nowPoint++;
        ReleaseDC(hwnd, hdc);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
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
            HWND null_fill_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("无"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                270 /*X坐标*/, 60 /*Y坐标*/, 20 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)FILL_BTN_NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
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
            HWND ellipse_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("椭圆"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                190 /*X坐标*/, 100 /*Y坐标*/, 40 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)Ellipse_BTN, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND clean_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("擦除"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                240 /*X坐标*/, 100 /*Y坐标*/, 40 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)CLEAN_BTN, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND back_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("撤销"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                290 /*X坐标*/, 100 /*Y坐标*/, 40 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)BACK_BTN, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND forward_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("前进"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                340 /*X坐标*/, 100 /*Y坐标*/, 40 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)FORWARD_BTN, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            // 创建画笔类型
            HWND pen_style = CreateWindow(TEXT("static"), 
                TEXT("画笔类型"),
                WS_CHILD | WS_VISIBLE | BS_CENTER,
                20, 140, 60, 30,
                hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND solid_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("实线"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                90 /*X坐标*/, 140 /*Y坐标*/, 40 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)SOLID_BTN, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            HWND bash_btn = CreateWindow(
                TEXT("BUTTON"), //按钮控件的类名
                TEXT("虚线"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                140 /*X坐标*/, 140 /*Y坐标*/, 40 /*宽度*/, 30/*高度*/,
                hwnd, (HMENU)DASH_BTN, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
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
                case FILL_BTN_NULL:
                    changeFillColor(FILL_BTN_NULL); break;
                // 改变画图事件
                case LINE_BTN:
                    changeDrawWhat(LINE_BTN);break;
                case RECT_BTN:
                    changeDrawWhat(RECT_BTN);break;
                case Ellipse_BTN:
                    changeDrawWhat(Ellipse_BTN);break;
                case CLEAN_BTN:
                    changeDrawWhat(CLEAN_BTN);break;
                // 改变画笔类型
                case SOLID_BTN:
                    changePenStyle(SOLID_BTN);break;
                case DASH_BTN:
                    changePenStyle(DASH_BTN);break;
                // 撤销与前进
                case BACK_BTN:
                    backStep(hwnd);break;
                case FORWARD_BTN:
                    forwardStep(hwnd);break;
                default:
                    break;
            }
            break;
        }
        case WM_LBUTTONDOWN:        // 鼠标左键按下
        {
            isMouseHold = 1;
            startPoint.x = GET_X_LPARAM(lParam);
            startPoint.y = GET_Y_LPARAM(lParam);
            endPoint.x = startPoint.x;
            endPoint.y = startPoint.y;
            break;
        }
        // case WM_MOUSEMOVE:          // 鼠标不松开移动
        // {
        //     if(isMouseHold)          // bMouseDown记录操作过程中是否按下鼠标
        //     {
        //         hdc = GetDC(hwnd);
        //         // 覆盖上一条
        //         SetROP2(hdc,R2_NOT);
        //         DoFunc(DO_WHAT);
        //         // 绘制新操作
        //         SetROP2(hdc, R2_COPYPEN);
        //         endPoint.x = GET_X_LPARAM(lParam);
        //         endPoint.y = GET_Y_LPARAM(lParam);
        //         DoFunc(DO_WHAT);
        //         // 重新绘制前面所有操作防止因为新操作移动挡住而丢失
        //         // ReDraw(hdc, 0);
        //         ReleaseDC(hwnd, hdc);
        //     }
        //     break;
        // }
        case WM_LBUTTONUP:          // 鼠标左键松开
        {
            if(isMouseHold) {
                isMouseHold = 0;
                hdc = GetDC(hwnd);
                endPoint.x = GET_X_LPARAM(lParam);
                endPoint.y = GET_Y_LPARAM(lParam);
                DoFunc(DO_WHAT);
                ReleaseDC(hwnd,hdc);
                // 记录所有操作
                startPoints[nowPoint].x = startPoint.x;
                startPoints[nowPoint].y = startPoint.y;
                endPoints[nowPoint].x = endPoint.x;
                endPoints[nowPoint].y = endPoint.y;
                nowPenColors[nowPoint] = PAINTCOLOR;
                nowFillColor[nowPoint] = FILLCOLOR;
                nowPenStyle[nowPoint] = PENSTYLE;
                nowIsFill[nowPoint] = ISFILL;
                nowPenDo[nowPoint] = DO_WHAT;
                nowPoint++;
                OPLENGTH = nowPoint;
            }
            break;
        }
        case WM_PAINT:
        {
            hdc = BeginPaint(hwnd, &ps);
            // 重绘
            ReDraw(hdc, 0);
            EndPaint(hwnd, &ps);
            break;
        }
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
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 700,
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