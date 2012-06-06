/* 
 *   This file is part of iviLINK 0.9 Technology Preview Samples Set.          
 *                                                                             
 *   iviLINK 0.9 Technology Preview Samples Set is free software:              
 *   you can redistribute it and/or modify                                     
 *   it under the terms of the GNU General Public License as published by      
 *   the Free Software Foundation, version 3 of the License.                   
 *                                                                             
 *   iviLINK 0.9 Technology Preview Samples Set is distributed in the          
 *   hope that it will be useful, but WITHOUT ANY WARRANTY; without            
 *   even the implied warranty of MERCHANTABILITY or FITNESS FOR A             
 *   PARTICULAR PURPOSE.  See the GNU General Public License for more details. 
 *                                                                              
 *   You should have received a copy of the GNU General Public License         
 *   along with iviLINK 0.9 Technology Preview.                                
 *   If not, see <http://www.gnu.org/licenses/>.                               
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
