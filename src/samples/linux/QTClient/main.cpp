 /****************************************************************************/
 /*                                                                          */
 /* This file is part of iviLINK 0.9 Technology Preview Samples Set.         */
 /*                                                                          */
 /* iviLINK 0.9 Technology Preview Samples Set is free software:             */
 /* you can redistribute it and/or modify                                    */
 /* it under the terms of the GNU General Public License as published by     */
 /* the Free Software Foundation, version 3 of the License.                  */
 /*                                                                          */
 /* iviLINK 0.9 Technology Preview Samples Set is distributed in the         */
 /* hope that it will be useful, but WITHOUT ANY WARRANTY; without           */
 /* even the implied warranty of MERCHANTABILITY or FITNESS FOR A            */
 /* PARTICULAR PURPOSE.  See the GNU General Public License for more details.*/
 /*                                                                          */
 /* You should have received a copy of the GNU General Public License        */
 /* along with iviLINK 0.9 Technology Preview.                               */
 /* If not, see <http://www.gnu.org/licenses/>.                              */
 /*                                                                          */
 /****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <sys/un.h>
#include <pthread.h>

#include <QtGui/QApplication>
#include <QVariant>
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#include <QObject>
#include <QGraphicsObject>
#include <QDebug>
#include "qmlapplicationviewer.h"
#include <qcoreapplication.h>

#include "CBtnProcessor.hpp"
#include "common.h"
#include "CStateUpdater.hpp"
#include "VideoContainer.h"
#include "utils/misc/include/Logger.hpp"
#include "utils/misc/include/logging.hpp"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("QTCLient.main"));

    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

    LOG4CPLUS_INFO(logger, "QTCLient Process started...");
    Log("QTCLient Process started...");

    //current state. to be moved to qml

    AirCondState currentState;
    // set default
    currentState.airRecirc = false;
    currentState.autoClimate = true;
    currentState.driverTemp = 20;
    currentState.dualClimate = false;
    currentState.fanLow = false;
    currentState.fanMiddle = false;
    currentState.fanTop = false;
    currentState.fanSpeed = 5;
    currentState.frontWindow = false;
    currentState.isAC = true;
    currentState.isOFF = false;
    currentState.passTemp = 20;
    currentState.rearWindow = false;

    CBtnProcessor * btnProcessor = new CBtnProcessor(); //(sockfd);

    QScopedPointer<QApplication> app(createApplication(argc, argv));

    app->setApplicationName("PhononTest");
    app->setQuitOnLastWindowClosed(true);

    qmlRegisterType<VideoContainer>("VideoContainer", 1, 0, "VideoContainer");

    QDeclarativeView view;
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view.rootContext()->setContextProperty("BtnProcessor", btnProcessor);
    view.setSource(QUrl::fromLocalFile("qml/test06_seat_app/main.qml"));
    view.setGeometry(100, 100, 800, 480);
    view.setCursor(Qt::PointingHandCursor);
    view.show();

    QObject *rootObject = dynamic_cast<QObject *>(view.rootObject());

    VideoContainer * mContainer=(rootObject->findChild<VideoContainer*>("videoContainer"));
    btnProcessor->setContainer(mContainer);

    btnProcessor->setRootObject(rootObject);

    CStateUpdater Updater(btnProcessor,rootObject );

    return app->exec();
}
