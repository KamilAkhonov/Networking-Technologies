#include "framework.h"
#include "Server.h"
#include <winsock2.h>
#include "stdio.h"
#include <String>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <filesystem>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

using namespace std;
namespace fs = std::filesystem;

#define MAX_LOADSTRING 100

#define WSA_ACCEPT (WM_USER+1)
#define WSA_NETEVENT (WM_USER+2)

#define SERV_PORT 5000

HWND TextEditBox;

SOCKET server_socket; // Сокет клиента
SOCKET rab_socket; // Рабочий сокет сервера

SOCKADDR_IN local_sin; // Локальный сокет
SOCKADDR_IN acc_sin; // Адрес сервера

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL OnCreate();
void ServerStart(HWND hWnd);
void ServerStop(HWND hWnd);
void OnWSAAccept(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void OnWSANetEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ReadRequest(SOCKET wParam);
string file_name;
// string* Split(string str, string delimiter);

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
    LoadStringW(hInstance, IDC_SERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERVER));

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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERVER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SERVER);
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
        150, 150, 350, 400, nullptr, nullptr, hInstance, nullptr);

    TextEditBox = CreateWindow(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
        10, 10, 320, 320, hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

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
    switch (message)
    {
    case WM_CREATE:
        OnCreate();
        return 0;

    case WSA_ACCEPT:
        OnWSAAccept(hWnd, message, wParam, lParam);
        return 0;

    case WSA_NETEVENT:
    {
        switch (WSAGETSELECTEVENT(lParam))
        {
        case FD_READ:
            ReadRequest((SOCKET)wParam);
            WSAAsyncSelect((SOCKET)wParam, hWnd, WSA_NETEVENT, FD_READ | FD_CLOSE);
            break;
        case FD_CLOSE:
            SetWindowText(TextEditBox, L"The client is disconnected");

            break;
        }
    }
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {
        case IDM_START:
            ServerStart(hWnd);
            break;
        case IDM_STOP:
            ServerStop(hWnd);
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
    TCHAR szTemp[128] = L"Библиотека инициализирована"; rc = WSAStartup(MAKEWORD(1, 1), &WSAData);
    if (rc != 0)
    {
        MessageBox(NULL, L"WSAStartup Error", L"Error", MB_OK);
        return FALSE;
    }
    return TRUE;

}

/* Процедура запуска сервера */

void ServerStart(HWND hWnd)
{
    struct sockaddr_in srv_address;
    int rc;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET)
    {
        MessageBox(NULL, L"socket Error", L"Error", MB_OK);
        return;
    }

    /* Устанавливаем адрес и порт */
    srv_address.sin_family = AF_INET;
    srv_address.sin_addr.s_addr = INADDR_ANY;
    srv_address.sin_port = htons(SERV_PORT);


    /* Связываем адрес IP с сокетом */
    if (bind(server_socket, (LPSOCKADDR)&srv_address,
        sizeof(srv_address)) == SOCKET_ERROR)
    {
        closesocket(server_socket);
        MessageBox(NULL, L"Bind Error", L"Error", MB_OK);
        return;
    }

    /* Переводим сокет в режим ожидания соединения с клиентом */
    if (listen(server_socket, 1) == SOCKET_ERROR)
    {
        closesocket(server_socket);
        MessageBox(NULL, L"Listen Error", L"Error", MB_OK);
        return;
    }


    /* При попытке установления соединения посылаем
    сообщение WSA_ACCEPT в главное окно */
    rc = WSAAsyncSelect(server_socket, hWnd, WSA_ACCEPT, FD_ACCEPT);
    if (rc > 0)
    {
        closesocket(server_socket);
        MessageBox(NULL, L"WSAAsyncSelect Error", L"Error", MB_OK);
        return;
    }

    /* Вывод сообщения о запуске сервера */
    SetWindowText(TextEditBox, L"The server is running");

} /* Конец функции ServerStart */

void ServerStop(HWND hWnd)
{
    /* Отмена прихода сообщений */
    WSAAsyncSelect(server_socket, hWnd, 0, 0);

    /* Закрытие сокета */
    if (server_socket != INVALID_SOCKET)
    {
        closesocket(server_socket);
    }

    /* Вывод сообщения об остановке сервера */
    SetWindowText(TextEditBox, L"The server is stopped");

} /* Конец функции ServerStop */

void OnWSAAccept(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    int rc;

    /* При ошибке - отмена сообщений */
    if (WSAGETSELECTERROR(lParam) != 0)
    {
        MessageBox(NULL, L"Accept Error", L"Error", MB_OK);
        WSAAsyncSelect(server_socket, hWnd, 0, 0);
        return;
    }


    /* Определение размера адреса сокета */
    int acc_sin_len = sizeof(acc_sin);

    /* Установка соединения */
    rab_socket = accept(server_socket, (LPSOCKADDR)&acc_sin, (int FAR*) & acc_sin_len);

    if (rab_socket == INVALID_SOCKET)
    {
        MessageBox(NULL, L"Accept Error, invalid socket", L"Error", MB_OK);
        return;
    }

    SetWindowText(TextEditBox, L"The client is connected");

    /* Подготовка к посылке сообщение о передаче данных
    или разрыве соединения */
    rc = WSAAsyncSelect(rab_socket, hWnd, WSA_NETEVENT, FD_READ | FD_CLOSE);

    if (rc > 0)
    {
        closesocket(rab_socket);
        MessageBox(NULL, L"WSAAsyncSelect Error", L"Error", MB_OK);
        return;
    }



} /* Конец функции OnWSAAccept */

map<string, int> products;
string server_request = "";
void ReadRequest(SOCKET wParam)
{
    int rc;
    char request[100];
    string dir_name;
    SOCKET sock;
    string text;
    ifstream file;
    string line;
    int i = 0;
    int price = 0;
    size_t pos = 0;
    string t;

    string ans;
    fs::path dir_path = "C:/Users/kamil/Source/Repos";

    /* Обработка события - приход данных */
    sock = (SOCKET)wParam;
    rc = recv(sock, request, 100, 0);

    switch (request[0])
    {
    case '1':
        SetWindowText(TextEditBox, L"Program search...");
        text = request;
        text.erase(0, 1);
        text.erase(text.find(";"), 1);
        file_name = text;
        dir_name = text;
        ans = "Fail not found";
        for (const auto& entry : fs::directory_iterator(dir_path))
        {
            if (entry.is_directory() && entry.path().filename().string() == dir_name)
            {
                ans = "File found";
            }
        }
        if (ans == "File found") {
            server_request = ans + "\r\n" + "Enter the parameters via ';' for the program" + " " + text;
        }
        else {
            server_request = ans;
        }
        send(sock, server_request.data(), 100, NULL);
        break;
    case '2':
        SetWindowText(TextEditBox, L"Program execution...");
        text = request;
        text.erase(0, 1);
        string input = text; // исходная строка
        char delimiter = ';'; // разделитель
        int num1 = 0, num2 = 0; // переменные для хранения чисел
        string buffer = ""; // буфер для хранения текущего числа
        bool first_num_done = false; // флаг, который показывает, что первое число уже получено
        for (char c : input) {
            if (c == delimiter) { // если встретили разделитель
                if (!first_num_done) { // если первое число еще не получено
                    num1 = stoi(buffer); // конвертируем буфер в целое число
                    buffer = ""; // очищаем буфер
                    first_num_done = true; // устанавливаем флаг, что первое число получено
                }
                else { // если второе число
                    num2 = stoi(buffer); // конвертируем буфер в целое число
                    buffer = ""; // очищаем буфер
                }
            }
            else {
                buffer += c; // добавляем текущий символ в буфер
            }
        }
        string suffix = ".exe"; // подстрока, которую нужно удалить
        input = file_name;
        // ищем позицию первого вхождения подстроки
        size_t pos = input.find(suffix);
        // если подстрока найдена, удаляем ее
        if (pos != string::npos) {
            input.erase(pos, suffix.length());
        }
        string directory = "C:/Users/kamil/Source/Repos/" + input + "/" + "Debug/"; // директория, в которую нужно записать файл
        string filename = "indata.txt"; // имя файла
        // создаем объект ofstream для записи в файл
        ofstream file(filename);
        // проверяем, удалось ли открыть файл для записи
        if (file.is_open()) {
            // записываем числа в файл
            file << num1 << " " << num2;
        }
        // закрываем файл
        file.close();
        directory = "C:/Users/kamil/Source/Repos/" + input + "/" + "Debug/"; // замените на свою директорию
        string executable = input + ".exe"; // замените на свой исполняемый файл
        string fullpath = directory + executable;
        int result = system(fullpath.c_str());
        if (result == 0) {
        }
        else {
            MessageBox(NULL, L"Program is down", L"Error", MB_OK);
            return;
        }
        int res;
        ifstream fin("outdata.txt");
        fin.is_open();
        fin >> res;
        server_request = "Summary: " + to_string(res);
        send(sock, server_request.data(), 100, NULL);
        break;
    }
}

