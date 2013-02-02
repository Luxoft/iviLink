/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 */ 

#include <QMessageBox>
#include <QCoreApplication>
#include <QCloseEvent>
#include <QRect>
#include <QDesktopWidget>
#include <QApplication>
#include <QTimer>

#include "stdio.h"
#include "ScreenSharingMessages.h"
#include "ScreenSharingViewer.hpp"
#include "CScreenSharingProfileProxy.hpp"

Logger ScreenSharingViewer::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Applications.ScreenSharingViewer"));

const std::string viewerKiller = "pkill -9 vncviewer";

ScreenSharingViewer::ScreenSharingViewer(QWidget *parent)
   : Application(iviLink::Service::Uid("ScreenSharingViewerService"))
   , mServerPort("5901")
   , startedByUser(true)
   , mpViewerProfileProxy(NULL)
   , isConnectedWithServer(false)
{
    adjustSize();
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    setWindowFlags(Qt::FramelessWindowHint);

    // progressbar without %
    setMinimum(0);
    setMaximum(0);

    // move to the screen center
    move(QApplication::desktop()->screen()->rect().center() - rect().center() );

    connect(this, SIGNAL(dataReceivedSig(const QString &) )
            , this, SLOT(onDataReceived(const QString &) ), Qt::QueuedConnection);
    mpViewerProfileProxy = new CScreenSharingProfileProxy(iviLink::Service::Uid("ScreenSharingViewerService") );

    QTimer::singleShot(20000, this, SLOT(onTimerCheckConnection() ) );
}

ScreenSharingViewer::~ScreenSharingViewer()
{
   delete mpViewerProfileProxy;
   mpViewerProfileProxy = NULL;
}

void ScreenSharingViewer::onInitDone(iviLink::ELaunchInfo launcher)
{
   if (iviLink::LAUNCHED_BY_USER == launcher)
   {
      startedByUser = true;
      LOG4CPLUS_INFO(msLogger, "started by user");

      registerProfileCallbacks(iviLink::Profile::ApiUid("ScreenSharingProfile_PAPI_UID"), this);
      //loading service with required UID
      iviLink::Error loadError = loadService(iviLink::Service::Uid("ScreenSharingViewerService"));
      if (loadError.isNoError())
      {
         LOG4CPLUS_INFO(msLogger, "ScreenSharingViewer::initDone() : sending data : " 
            + getMessageTypeStr(iviLink::SCR_SHARING_START) );
         if (mpViewerProfileProxy)
            mpViewerProfileProxy->sendData(getMessageTypeStr(iviLink::SCR_SHARING_START) );         
      }
      else
      {
         LOG4CPLUS_ERROR(msLogger, "service NOT started: " + loadError.toString());
         qApp->quit();
      }
   }
  
   //if application was launched by other side
   else
   {
      startedByUser = false;
      LOG4CPLUS_INFO(msLogger, "started by iviLink");
   }
}

void ScreenSharingViewer::handleError(BaseError const & error)
{
   LOG4CPLUS_INFO(msLogger,static_cast<std::string>(error));
}

void ScreenSharingViewer::onIncomingServiceBeforeLoading(const iviLink::Service::Uid &service)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(msLogger, "Registering profile callbacks");
   registerProfileCallbacks(iviLink::Profile::ApiUid("ScreenSharingProfile_PAPI_UID"), this);
}

void ScreenSharingViewer::onIncomingServiceAfterLoading(const iviLink::Service::Uid &service)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
}

void ScreenSharingViewer::onServiceDropped(const iviLink::Service::Uid &service)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_WARN(msLogger, "Other side has died!");
   qApp->quit();
}

std::string ScreenSharingViewer::getMessageTypeStr(int type)
{
   std::stringstream stream;
   stream << type;
   return stream.str();
}

void ScreenSharingViewer::startViewer()
{
   LOG4CPLUS_INFO(msLogger, "ENTER: ScreenSharingViewer::startViewer()");
   std::string vncViewerStr = "vncviewer";
   char command[vncViewerStr.size() + sizeof(char) 
                + mServerAddress.size() + sizeof(char) + mServerPort.size()];
   //sprintf(command, "%s %s:%s  -fullscreen", vncViewerStr.c_str(), mServerAddress.c_str(), mServerPort.c_str() );
   sprintf(command, "%s %s:%s", vncViewerStr.c_str(), mServerAddress.c_str(), mServerPort.c_str() );
   LOG4CPLUS_INFO(msLogger, "system command : " + std::string(command) );
   system(command);
   qApp->quit();
}

void ScreenSharingViewer::sendData(const std::string &data)
{
    if (mpViewerProfileProxy)
        mpViewerProfileProxy->sendData(data);
    else
        LOG4CPLUS_WARN(msLogger,
                       "ScreenSharingViewer::sendData called with invalid pointer: mpViewerProfileProxy");
}

void ScreenSharingViewer::dataReceived(const std::string &data)
{
    emit dataReceivedSig(QString::fromStdString(data) );
}

void ScreenSharingViewer::onDataReceived(const QString &inData)
{
    std::string data = inData.toStdString();
    LOG4CPLUS_INFO(msLogger, "ENTER: ScreenSharingViewer::dataReceived(), data : " + data);
    if (!data.compare(0, 1, getMessageTypeStr(iviLink::SCR_SHARING_START) ) )
    {
       LOG4CPLUS_INFO(msLogger, "case : data starts with iviLink::SCR_SHARING_START");
    }
    else if (!data.compare(0, 1, getMessageTypeStr(iviLink::SCR_SHARING_SERVER_ADDRESS) ) )
    {
       LOG4CPLUS_INFO(msLogger, "case : data starts with iviLink::SCR_SHARING_SERVER_ADDRESS");
       mServerAddress = data.substr(1);
       LOG4CPLUS_INFO(msLogger, "mServerAddress : " + mServerAddress);
       if (!startedByUser)
       {
          if (QMessageBox::Ok == QMessageBox::question(this, "Incoming Screen Sharing Request"
                                    ,"Do you want to start Screen Sharing Session?",
                                    QMessageBox::Cancel, QMessageBox::Ok) )
          {
             sendData(getMessageTypeStr(iviLink::SCR_SHARING_START_ACK) );
             LOG4CPLUS_INFO(msLogger, "dialog : OK : start viewer");
             sleep(4);
             hide();
             startViewer();
          }
          else
             sendData(getMessageTypeStr(iviLink::SCR_SHARING_EXIT) );
             LOG4CPLUS_INFO(msLogger, "dialog : Cancel : quit");             
             qApp->quit();
       }
    }
    else if (!data.compare(0, 1, getMessageTypeStr(iviLink::SCR_SHARING_START_ACK) ) )
    {
       LOG4CPLUS_INFO(msLogger, "case : iviLink::SCR_SHARING_START_ACK : start viewer");
       isConnectedWithServer = true;
       hide();
       startViewer();
    }
    else if (!data.compare(0, 1, getMessageTypeStr(iviLink::SCR_SHARING_EXIT) ) )
    {
       LOG4CPLUS_INFO(msLogger, "case : iviLink::SCR_SHARING_EXIT : quit");
       system(viewerKiller.c_str() );
       qApp->quit();
    }
}

void ScreenSharingViewer::closeEvent(QCloseEvent *event)
{
   LOG4CPLUS_INFO(msLogger, "CLOSE EVENT");
   system(viewerKiller.c_str() );
   event->accept();
   qApp->quit();
}

void ScreenSharingViewer::onTimerCheckConnection()
{
   if (!isConnectedWithServer && startedByUser)
   {    
      LOG4CPLUS_ERROR(msLogger, "onTimerCheckConnection : NO CONNECTION WITH THE OTHER SIDE");
      qApp->quit();
   }
}
