// Client.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Client.h"
#include <winsock2.h>
#include "stdio.h"
#include <regex>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#pragma warning(disable: 4996)

#define MAX_LOADSTRING 100
#define newline L"\r\n"

#define SERV_PORT 5000

HWND ServerTextEditBox;
HWND ClientTextEditBox;
HWND ButtonTakeProducts;
HWND ButtonBuyProducts;
HWND Label_Money;

SOCKET server_socket; // Сокет клиента
SOCKADDR_IN local_sin; // Локальный сокет
SOCKADDR_IN dest_sin; // Адрес сервера

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

BOOL OnCreate();
void SetConnection(HWND hWnd);
// BOOL SendRequest();
BOOL TakeProductRequest();
BOOL TakeProdRequest();


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CLIENT);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        550, 150, 390, 500, nullptr, nullptr, hInstance, nullptr);

    ServerTextEditBox = CreateWindow(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
        25, 10, 330, 300, hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

    ClientTextEditBox = CreateWindow(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOVSCROLL,
        25, 320, 330, 30, hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

    ButtonTakeProducts = CreateWindowA("BUTTON", "Поиск программы", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        25, 370, 130, 30, hWnd, (HMENU)IDB_BUTTON_TAKE_PRODUCTS, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

    ButtonBuyProducts = CreateWindowA("BUTTON", "Отправить данные", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        205, 370, 150, 30, hWnd, (HMENU)IDB_BUTTON_BUY_PRODUCTS, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    std::wstring wstr;

    switch (message)
    {
    case WM_CREATE:
        OnCreate();
        return 0;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {
        case IDM_CONNECT:
            SetConnection(hWnd);
            break;
        case IDB_BUTTON_TAKE_PRODUCTS:
            TakeProductRequest();
            break;
        case IDB_BUTTON_BUY_PRODUCTS:
            TakeProdRequest();
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
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

BOOL OnCreate()
{
    int rc;
    WSADATA WSAData;

    rc = WSAStartup(MAKEWORD(1, 1), &WSAData);
    if (rc != 0)
    {
        MessageBox(NULL, L"WSAStartup Error", L"Error", MB_OK);
        return FALSE;
    }

    return TRUE;

}

void SetConnection(HWND hWnd)
{
    PHOSTENT phe;

    /* Создание сокета */
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET)
    {
        MessageBox(NULL, L"Socket Error", L"Error", MB_OK);
        return;
    }

    /* Установка адреса IP и номера порта */
    dest_sin.sin_family = AF_INET;
    phe = gethostbyname("localhost");
    if (phe == NULL)
    {
        closesocket(server_socket);
        MessageBox(NULL, L"Gethostbyname Error", L"Error", MB_OK);
        return;
    }
    memcpy((char FAR*) & (dest_sin.sin_addr), phe->h_addr, phe->h_length);
    dest_sin.sin_port = htons(SERV_PORT);

    /* Установка соединения */
    if (connect(server_socket, (PSOCKADDR)&dest_sin, sizeof(dest_sin)) < 0)
    {
        closesocket(server_socket);
        MessageBox(NULL, L"Connect Error", L"Error", MB_OK);
        return;
    }

    SetWindowText(ServerTextEditBox, L"Соединение установлено");

} /* Конец функции SetConnect */

BOOL TakeProductRequest()
{
    auto buffer_size = GetWindowTextLength(ClientTextEditBox) + 1;
    wchar_t* buffer = new wchar_t[buffer_size];
    GetWindowText(ClientTextEditBox, buffer, buffer_size + 1);
    std::string request = "1";
    std::wstring ws(buffer);
    std::string str(ws.begin(), ws.end());
    request += str;
    send(server_socket, request.data(), buffer_size + 1, 0);
    char buf[100];
    recv(server_socket, buf, 100, 0);
    str = buf;
    std::wstring text(str.begin(), str.end());
    SetWindowText(ServerTextEditBox, text.data());
    SetWindowText(ClientTextEditBox, L"");
    return TRUE;
}

BOOL TakeProdRequest()
{
    auto buffer_size = GetWindowTextLength(ClientTextEditBox) + 1;
    wchar_t* buffer = new wchar_t[buffer_size];
    GetWindowText(ClientTextEditBox, buffer, buffer_size + 1);
    std::string request = "2";
    std::wstring ws(buffer);
    std::string str(ws.begin(), ws.end());
    request += str;
    send(server_socket, request.data(), buffer_size + 1, 0);
    char buf[100];
    recv(server_socket, buf, 100, 0);
    str = buf;
    std::wstring text(str.begin(), str.end());
    SetWindowText(ServerTextEditBox, text.data());
    SetWindowText(ClientTextEditBox, L"");
    return TRUE;
}
