/* 
 *  iviLINK SDK, version 0.9 (for preview only)                                      
 *    http://www.ivilink.net                                                         
 *  Cross Platform Application Communication Stack for In-Vehicle Applications       
 *                                                                                   
 *  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
 *                                                                                   
 *  This library is free software; you can redistribute it and/or                    
 *  modify it under the terms of the GNU Lesser General Public                       
 *  License as published by the Free Software Foundation; version 2.1.               
 *                                                                                   
 *  This library is distributed in the hope that it will be useful,                  
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
 *  Lesser General Public License for more details.                                  
 *                                                                                   
 *  You should have received a copy of the GNU Lesser General Public                 
 *  License along with this library; if not, write to the Free Software              
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
 * 
 * 
 * 
 * 
 */


#ifndef COLORFILTERPRWIDGET_H
#define COLORFILTERPRWIDGET_H

#include <QtGui>
#include <phonon/phonon>
#include <QGraphicsProxyWidget>
#include <QPainter>
#include <QImage>

class ColorFilterProxyWidget : public QGraphicsProxyWidget
{
public:
    ColorFilterProxyWidget(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0):
    QGraphicsProxyWidget(parent,wFlags)
    {
    }

protected:
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    QPainter *painter_inverted = new QPainter();
    QImage invertedColor(boundingRect().width(),boundingRect().height(),QImage::Format_RGB32);
    painter_inverted->begin(&invertedColor);
    QGraphicsProxyWidget::paint(painter_inverted,option,widget);
    painter_inverted->end();
    painter->drawImage(0,0,invertedColor.rgbSwapped());
  }
};


#endif // COLORFILTERPRWIDGET_H
