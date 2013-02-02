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


#ifndef CSCREENSHARINGVIEWER_HPP_
#define CSCREENSHARINGVIEWER_HPP_

#include <QProgressBar>

#include "Application.hpp"
#include "Logger.hpp"
#include "IScreenSharingProfileAPI.hpp"
#include "CSignalSemaphore.hpp"

class QCloseEvent;

class CScreenSharingProfileProxy;

class ScreenSharingViewer : public QProgressBar,
                            public iviLink::Application,
                            public IScreenSharingProfile_API::Callbacks
{   
   Q_OBJECT

public:
   ScreenSharingViewer(QWidget *parent = 0/*, CSignalSemaphore & semaphore*/);
   virtual ~ScreenSharingViewer();

   /**
    * Callback that should be invoked by application library thread 
    * after initialization application in application manager 
    * @param launcher shows the initiator of application start
    * @return none
    */
   virtual void onInitDone(iviLink::ELaunchInfo launcher);

   /**
    * Callback is called before loading incoming service
    * @param service is UID of service
    */
   virtual void onIncomingServiceBeforeLoading(const iviLink::Service::Uid &service);

   /**
    * Callback is called after loading of incoming service
    */
   virtual void onIncomingServiceAfterLoading(const iviLink::Service::Uid &service);

   /**
    * Callback is called in case of and service session
    * @param service UID is uid of service
    */
   virtual void onServiceDropped(const iviLink::Service::Uid &service);

   virtual void handleError(BaseError const & error);

   /**
    * Callback is called in case of incoming data
    * @param data is data
    */
   virtual void dataReceived(const std::string &data);

   /**
    * Method is called in case of closing the widget
    */
   virtual void closeEvent(QCloseEvent *event);

private:
   std::string getMessageTypeStr(int type);
   void startViewer();
   void sendData(const std::string &data);

   IScreenSharingProfile_API* mpScreenSharingProfileAPI;
   CScreenSharingProfileProxy *mpViewerProfileProxy;

   static Logger msLogger;
   std::string mServerAddress;
   const std::string mServerPort;
   bool startedByUser;
   bool isConnectedWithServer;

private slots:
   void onDataReceived(const QString &data);
   void onTimerCheckConnection();

signals:
   void dataReceivedSig(const QString &data);
};

#endif /* CSCREENSHARINGVIEWER_HPP_ */
