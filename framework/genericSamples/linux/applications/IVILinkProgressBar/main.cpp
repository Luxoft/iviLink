#include <QtGui/QApplication>
#include <QApplication>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>

#include "ivilinkprogressbar.hpp"
#include "Logger.hpp"
#include "Types.hpp"
#include "TcpSocket.hpp"

void *threadFunc(void *arg);
UInt8 buf[1]={0};


int main(int argc, char *argv[])
{
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
    Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("IVILinkProgressBar.main.socket"));
    QApplication a(argc, argv);
    IVILinkProgressBar w;
    w.setCursor(QCursor(Qt::BlankCursor));
    w.setGeometry(0, 0, 800, 600);
    w.setAttribute(Qt::WA_TranslucentBackground);
    w.setAttribute(Qt::WA_NoSystemBackground);
    w.setWindowFlags(Qt::FramelessWindowHint);
    w.activateWindow();
    w.show();

    pthread_t pth;
    pthread_create(&pth, NULL, threadFunc, (void*)"socket listening");

    return a.exec();
}


void *threadFunc(void *arg)
{
    TcpSocket tcpSocket(3425);
    tcpSocket.initServer();
    UInt8 message;
    do
    {
        message = -1;
        tcpSocket.acceptSocket();
        tcpSocket.receiveMessage(message);
    } while (message != 1);
    tcpSocket.closeSocket();
    exit(0);
}
