#include "FrontCamComm.h"

#include <QDebug>

#include <thread>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

#define PACKET_SIZE 1024
#define serverPort 8282

FrontCamComm::FrontCamComm()
{
    m_bServerState = false;

}

FrontCamComm::~FrontCamComm()
{

}

void FrontCamComm::initialization()
{
    qDebug() << "[FrontCamComm::initialization] - ";
}

bool FrontCamComm::ServiceStart()
{
    qDebug() << "[FrontCamComm::ServiceStart] - ";

    bool bRet = true;

    bRet = CreateServer();

    m_bServerState = bRet;

    return bRet;
}

bool FrontCamComm::ServiceStop()
{
    qDebug() << "[FrontCamComm::ServiceStop] - ";

    m_bServerState = false;

    return true;
}

bool FrontCamComm::CreateServer()
{
    qDebug() << "[FrontCamComm::CreateServer] - ";

    bool bRet = true;

    int nOpttion = 1;
    struct sockaddr_in serverAddr;
    int server_socket;
    int nBindRet;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(server_socket < 0 ){
        qDebug() << "[CreateServer] - sock create fail...";
        return false;
    }
    qDebug() << "[CreateServer] - server_socket : " << server_socket;

    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &nOpttion, sizeof(nOpttion));

    memset(&serverAddr, 0x00, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(serverPort);

    nBindRet = bind(server_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    qDebug() << "[CreateServer] - nBindRet : " << nBindRet;

    if(nBindRet < 0){
        qDebug() << "[] - sock bind fail...";
        close(server_socket);
        return false;
    }

    listen(server_socket, 2);

    pthread_t listen_thread;
    if(pthread_create(&listen_thread, nullptr, ClientAccept, nullptr) != 0){
        return false;
    }

    m_bServerState = true;

//    while(m_bServerState){
//        sleep(1);
//        qDebug() << "[CreateServer] - ";
//    }

    pthread_join(listen_thread, nullptr);

    pthread_detach(listen_thread);

    close(server_socket);

    return bRet;
}

void *FrontCamComm::ClientAccept(void *data)
{

}
