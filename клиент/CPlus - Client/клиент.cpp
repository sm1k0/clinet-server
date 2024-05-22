#define WIN32_LEAN_AND_MEAN // макрос для исключения редко используемых заголовков Windows, ускоряя компиляцию и уменьшая размер конечного исполняемого файла.
#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main() {
    setlocale(LC_ALL, "RUS");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(0, "");
    WSADATA wsaData; // Структура для хранения данных инициализации Windows Sockets
    ADDRINFO hints; // Подсказки для получения информации о адресе
    ADDRINFO* addrResult = NULL; // Указатель для хранения результатов функции getaddrinfo
    SOCKET ConnectSocket = INVALID_SOCKET; // Сокет для связи
    char recvBuffer[512]; // Буфер для приема данных

    const char* sendBuffer1 = "васаб бро"; // Первое сообщение для отправки
    const char* sendBuffer2 = "привет лох"; // Второе сообщение для отправки

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

    // Получаем адрес и порт сервера
    result = getaddrinfo("localhost", "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo не удалось с ошибкой: " << result << endl;
        WSACleanup();
        return 1;
    }

    // Создаем сокет для подключения к серверу
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Создание сокета не удалось" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Подключаемся к серверу
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Не удалось подключиться к серверу" << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Отправляем первое сообщение на сервер
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    if (result == SOCKET_ERROR) {
        cout << "Отправка не удалась с ошибкой: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Отправлено байт: " << result << " - Сообщение: " << sendBuffer1 << endl;

    // Отправляем второе сообщение на сервер
    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) {
        cout << "Отправка не удалась с ошибкой: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Отправлено байт: " << result << " - Сообщение: " << sendBuffer2 << endl;

    // Завершаем передачу данных, так как больше не будем отправлять данные
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "Завершение передачи данных не удалось с ошибкой: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Получаем данные от сервера до тех пор, пока соединение не будет закрыто
    do {
        ZeroMemory(recvBuffer, 512); // Очищаем буфер приема
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "Получено байт: " << result << endl;
            cout << "Полученные данные: " << recvBuffer << endl;
        }
        else if (result == 0) {
            cout << "Соединение закрыто" << endl;
        }
        else {
            cout << "Прием данных не удался с ошибкой: " << WSAGetLastError() << endl;
            closesocket(ConnectSocket);
            freeaddrinfo(addrResult);
            WSACleanup();
            return 1;
        }
    } while (result > 0);

    // Очистка
    closesocket(ConnectSocket);
    freeaddrinfo(addrResult);
    WSACleanup();
    return 0;
}
