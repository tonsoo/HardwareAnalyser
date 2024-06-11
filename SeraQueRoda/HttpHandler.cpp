#include "HttpHandler.h"

#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <iomanip>
#include <sstream>
#include <regex>

namespace Http {

    bool HttpHandler::IsReserved(char c){
        return (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' || c == '=' || c == '&' || c == '/');
    }

    bool HttpHandler::Initialize() {
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            std::cerr << "WSAStartup failed: " << result << "\n";
            return false;
        }
        return true;
    }

    SOCKET HttpHandler::Connect(const std::string& host, const std::string& port) {
        struct addrinfo hints = {}, * res = nullptr;
        SOCKET sockfd = INVALID_SOCKET;

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        if (getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0) {
            std::cerr << "getaddrinfo error\n";
            return INVALID_SOCKET;
        }

        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd == INVALID_SOCKET) {
            std::cerr << "socket creation error\n";
            freeaddrinfo(res);
            return INVALID_SOCKET;
        }

        if (connect(sockfd, res->ai_addr, static_cast<int>(res->ai_addrlen)) == SOCKET_ERROR) {
            std::cerr << "connection error\n";
            closesocket(sockfd);
            freeaddrinfo(res);
            return INVALID_SOCKET;
        }

        freeaddrinfo(res);

        return sockfd;
    }

    std::string HttpHandler::Fetch(const std::string& initial_url, const std::string& port) {
        std::string host = "";
        std::string path = "/";

        std::regex urlRegex(R"(^(http[s]{0,1}:\/\/)([^\/\?\#]+){1}(\/[^\?\#]+){0,1}(\?[^#]+){0,1}(\#.*){0,1})");
        std::smatch urlMatch;

        if (std::regex_match(initial_url, urlMatch, urlRegex)) {
            std::string scheme = urlMatch[1].str();
            host = urlMatch[2].str();
            path = urlMatch[3].str();

            path = path + "?" + this->query;
        }

        std::string falseRet;

        if (!this->Initialize()) {
            return falseRet;
        }

        SOCKET sockfd = this->Connect(host, port);
        if (sockfd == INVALID_SOCKET) {
            WSACleanup();
            return falseRet;
        }

        std::string request = "GET " + path + " HTTP/1.1\r\n";
        request += "Host: " + host + "\r\n";
        request += "Connection: close\r\n\r\n";

        if (send(sockfd, request.c_str(), static_cast<int>(request.size()), 0) == SOCKET_ERROR) {
            std::cerr << "send error\n";
            closesocket(sockfd);
            WSACleanup();
            return falseRet;
        }

        char buffer[4096];
        std::string response;
        int bytes_received;
        while ((bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[bytes_received] = '\0';
            response += buffer;
        }

        if (bytes_received == SOCKET_ERROR) {
            std::cerr << "recv error\n";
        }

        closesocket(sockfd);
        WSACleanup();

        size_t header_end_pos = response.find("\r\n\r\n");
        if (header_end_pos == std::string::npos) {
            std::cerr << "Invalid HTTP response: no header end found\n";
            return "";
        }

        this->ResponseBody = response.substr(header_end_pos + 4);

        std::istringstream response_stream(response);
        std::string status_line;
        std::getline(response_stream, status_line);

        // Extract the status code from the status line
        int status_code = -1;
        if (status_line.substr(0, 5) == "HTTP/") {
            size_t code_start = status_line.find(' ');
            size_t code_end = status_line.find(' ', code_start + 1);
            if (code_start != std::string::npos && code_end != std::string::npos && code_end > code_start + 1) {
                status_code = std::stoi(status_line.substr(code_start + 1, code_end - code_start - 1));
            }
        }

        this->ResponseCode = status_code;

        return this->ResponseBody;
    }

    std::string HttpHandler::UrlEncode(const std::string& url){
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;

        for (char c : url) {
            if (isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << c;
            }
            else {
                escaped << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
            }
        }

        return escaped.str();
    }

    void HttpHandler::AddParameter(const std::string& name, const std::string& value) {

        if (this->query != "") {
            this->query += "&";
        }

        this->query += this->UrlEncode(name) + "=" + this->UrlEncode(value);
    }

    void HttpHandler::AddParameter(const std::string& name, const double& value) {
        std::ostringstream ss;
        ss << value;

        this->AddParameter(name, ss.str());
    }

    void HttpHandler::AddParameter(const std::string& name, const int& value) {
        std::ostringstream ss;
        ss << value;

        this->AddParameter(name, ss.str());
    }

    void HttpHandler::AddParameter(const std::string& name, const char& value) {
        std::ostringstream ss;
        ss << value;

        this->AddParameter(name, ss.str());
    }

}