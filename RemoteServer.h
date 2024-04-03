//
// Created by zin on 4/2/24.
//

#ifndef MOBILEREMOTE_REMOTESERVER_H
#define MOBILEREMOTE_REMOTESERVER_H


#include "cpp/qrcodegen.hpp"

class RemoteServer {

    std::string toSvgString(const qrcodegen::QrCode &qr, int border);

    void printQr(const qrcodegen::QrCode &qr);

    char *EncodeQRString(uint16_t port);

    static int GetKey(void *data);

    static int fetchIP(char *ip);

public:
    RemoteServer(uint16_t port = 3621);
};


#endif //MOBILEREMOTE_REMOTESERVER_H
