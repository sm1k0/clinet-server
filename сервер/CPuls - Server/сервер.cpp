#define WIN32_LEAN_AND_MEAN // макрос для исключения редко используемых заголовков Windows, ускоряя компиляцию и уменьшая размер конечного исполняемого файла.

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
using namespace std;

int main() {
    setlocale(LC_ALL, "RUS");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(0, "");
    WSADATA wsaData; // Структура для хранения данных инициализации Windows Sockets
    SOCKET ConnectSocket = INVALID_SOCKET; // Сокет для связи
    SOCKET ListenSocket = INVALID_SOCKET; // Сокет для прослушивания входящих соединений
    ADDRINFO hints; // Подсказки для получения информации о адресе
    ADDRINFO* addrResult = NULL; // Указатель для хранения результатов функции getaddrinfo
    const char* sendBuffer = "привет на сервере"; // Буфер для отправки данных
    char recvBuffer[512]; // Буфер для приема данных

    // Инициализация библиотеки Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup не удалось, результат: " << result << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints)); // Обнуляем память для подсказок
    hints.ai_family = AF_INET; // Используем IPv4 адреса
    hints.ai_socktype = SOCK_STREAM; // Потоковый сокет
    hints.ai_protocol = IPPROTO_TCP; // Протокол TCP
    hints.ai_flags = AI_PASSIVE; // Пассивный сокет для прослушивания

    // Получаем локальный адрес и порт для использования сервером
    result = getaddrinfo(NULL, "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo не удалось с ошибкой: " << result << endl;
        WSACleanup();
        return 1;
    }

    // Создаем сокет для прослушивания входящих соединений
    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "Создание сокета не удалось" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Настраиваем TCP сокет для прослушивания
    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Привязка сокета не удалась" << endl;
        freeaddrinfo(addrResult);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(addrResult); // Освобождаем память, выделенную для информации о адресе

    // Начинаем прослушивание входящих соединений
    result = listen(ListenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        cout << "Прослушивание не удалось" << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Принимаем клиентское соединение
    ConnectSocket = accept(ListenSocket, NULL, NULL);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Принятие соединения не удалось" << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    closesocket(ListenSocket); // Больше не нуждаемся в сокете для прослушивания

    // Получаем данные до тех пор, пока клиент не закроет соединение
    do {
        ZeroMemory(recvBuffer, 512); // Очищаем буфер приема
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "Получено байт: " << result << endl;
            cout << "Полученные данные: " << recvBuffer << endl;

            // Отправляем полученные данные обратно клиенту
            result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
            if (result == SOCKET_ERROR) {
                cout << "Отправка не удалась с ошибкой: " << WSAGetLastError() << endl;
                closesocket(ConnectSocket);
                WSACleanup();
                return 1;
            }
            cout << "Отправлено байт: " << result << endl;
        }
        else if (result == 0) {
            cout << "Закрытие соединения..." << endl;
        }
        else {
            cout << "Прием данных не удался с ошибкой: " << WSAGetLastError() << endl;
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }
    } while (result > 0);

    // Завершаем соединение, так как больше не будем отправлять данные
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "Завершение соединения не удалось с ошибкой: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Очистка
    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}
