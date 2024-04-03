//
// Created by zin on 4/2/24.
//

#include <iostream>
#include "RemoteServer.h"
#include "cpp/qrcodegen.hpp"
#include <sys/socket.h>
#include <netdb.h>
#include <cstdio>
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <climits>
#include <string>

#define PORT = 3621

int RemoteServer::fetchIP(char* ip) {
    char hostname[256];

    // Get the hostname
    if (gethostname(hostname, sizeof(hostname)) == -1) {
        perror("gethostname");
        return 1;
    }

    // Get address information from hostname
    struct addrinfo hints, *res, *p;
    int status;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;
    if ((status = getaddrinfo(hostname, NULL, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }

    // Loop through all the results and get the IP address
    for(p = res; p != NULL; p = p->ai_next) {
        void *addr;
        if (p->ai_family == AF_INET) { // IPv4
            *(ip - 19) = 0;
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
        } else { // IPv6
            *(ip - 19) = 1;
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }
        // Convert the IP to a string and print it
        inet_ntop(p->ai_family, addr, ip, INET6_ADDRSTRLEN);
        std::cout << "Local IP Address: " << ip<< std::endl;
    }

    freeaddrinfo(res); // Free the linked list

    return 0;
}

int RemoteServer::GetKey(void* data){ //This solution only generates an int-length key, needs to be improved.

    auto key = reinterpret_cast<uint64_t*>(data);
    srand (time(NULL));

    *key = rand() % UINT64_MAX;

    return 0;
}

//Format Key:
// ip type (0=ipv4 1=ipv6), 16 character code (randomly generated after every connection attempt), port (2 bytes), ip address.

char* RemoteServer::EncodeQRString(uint16_t port){

    char* data = new char[19 + INET6_ADDRSTRLEN];

    if(fetchIP(data + 19) != 0)
        throw std::exception();

    auto tmp = reinterpret_cast<uint16_t*>(data + 17);

    *tmp = port;

    GetKey(data + 1);

    return data;
}

void RemoteServer::printQr(const qrcodegen::QrCode &qr) {
    int border = 4;
    for (int y = -border; y < qr.getSize() + border; y++) {
        for (int x = -border; x < qr.getSize() + border; x++) {
            std::cout << (qr.getModule(x, y) ? "##" : "  ");
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

std::string RemoteServer::toSvgString(const qrcodegen::QrCode &qr, int border) {
    if (border < 0)
        throw std::domain_error("Border must be non-negative");
    if (border > INT_MAX / 2 || border * 2 > INT_MAX - qr.getSize())
        throw std::overflow_error("Border too large");

    std::ostringstream sb;
    sb << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    sb << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
    sb << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" viewBox=\"0 0 ";
    sb << (qr.getSize() + border * 2) << " " << (qr.getSize() + border * 2) << "\" stroke=\"none\">\n";
    sb << "\t<rect width=\"100%\" height=\"100%\" fill=\"#FFFFFF\"/>\n";
    sb << "\t<path d=\"";
    for (int y = 0; y < qr.getSize(); y++) {
        for (int x = 0; x < qr.getSize(); x++) {
            if (qr.getModule(x, y)) {
                if (x != 0 || y != 0)
                    sb << " ";
                sb << "M" << (x + border) << "," << (y + border) << "h1v1h-1z";
            }
        }
    }
    sb << "\" fill=\"#000000\"/>\n";
    sb << "</svg>\n";
    return sb.str();
}

RemoteServer::RemoteServer(uint16_t port) {

    printf("Fetching address and generating code...\n");

    char* data = EncodeQRString(port);

    qrcodegen::QrCode code = qrcodegen::QrCode::encodeText(data, qrcodegen::QrCode::Ecc::MEDIUM);

    std::string svg = toSvgString(code, 4);

    printQr(code);

    delete[] data;
}
