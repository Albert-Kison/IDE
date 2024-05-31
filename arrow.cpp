#include "arrow.h"
#include "diagramitem.h"

#include <QPainter>
#include <QPen>
#include <QtMath>



// create arrow from start item to end item
Arrow::Arrow(DiagramItem *startItem, DiagramItem *endItem, QGraphicsItem *parent)
    : QGraphicsLineItem(parent), myStartItem(startItem), myEndItem(endItem)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}



// the arrow is larger than the bounding rectangle of the QGraphicsLineItem
QRectF Arrow::boundingRect() const
{
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}



// used to check for collisions and selections with the mouse.
QPainterPath Arrow::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(arrowHead);
    return path;
}



// update the position of an arrow when its items are moved
void Arrow::updatePosition()
{
    QLineF line(mapFromItem(myStartItem, 0, 0), mapFromItem(myEndItem, 0, 0));
    setLine(line);
}



// called when the arrow is being painted
void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                  QWidget *)
{
    // if the items collide, exit the function
    if (myStartItem->collidesWithItem(myEndItem))
        return;

    // set the pen and brush for drawing
    QPen myPen = pen();
    myPen.setColor(myColor);
    qreal arrowSize = 20;
    painter->setPen(myPen);
    painter->setBrush(myColor);

    // find the start and end positions
    QPointF startPos = myStartItem->pos();
    QPointF endPos = myEndItem->pos();

    // Calculate intermediate points for a 90-degree turn path
    QPointF intermediate1, intermediate2;
    if (std::abs(startPos.x() - endPos.x()) > std::abs(startPos.y() - endPos.y())) {
        // More horizontal distance than vertical
        intermediate1 = QPointF(endPos.x(), startPos.y());
        intermediate2 = endPos;
    } else {
        // More vertical distance than horizontal
        intermediate1 = QPointF(startPos.x(), endPos.y());
        intermediate2 = endPos;
    }

    // Create the polyline path
    QPolygonF linePath;
    linePath << startPos << intermediate1 << intermediate2;

    // Draw the polyline
    for (int i = 0; i < linePath.size() - 1; ++i) {
        painter->drawLine(linePath[i], linePath[i + 1]);
    }

    // Calculate the angle for the arrow head
    QLineF lastSegment(linePath[linePath.size() - 2], linePath.last());
    double angle = std::atan2(-lastSegment.dy(), lastSegment.dx());

    // Polygon points of the arrow head
    QPointF arrowP1 = linePath.last() + QPointF(sin(angle + M_PI / 3) * arrowSize,
                                                cos(angle + M_PI / 3) * arrowSize);
    QPointF arrowP2 = linePath.last() + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                                                cos(angle + M_PI - M_PI / 3) * arrowSize);

    arrowHead.clear();
    arrowHead << linePath.last() << arrowP1 << arrowP2;

    // Draw the arrow head
    painter->drawPolygon(arrowHead);

    // If the arrow is selected, draw dashed lines around it
    if (isSelected()) {
        painter->setPen(QPen(myColor, 1, Qt::DashLine));
        for (int i = 0; i < linePath.size() - 1; ++i) {
            QLineF myLine(linePath[i], linePath[i + 1]);
            myLine.translate(0, 4.0);
            painter->drawLine(myLine);
            myLine.translate(0, -8.0);
            painter->drawLine(myLine);
        }
    }

}



// emit the selectedChanged signal when the item is selected
QVariant Arrow::itemChange(GraphicsItemChange change,
                                     const QVariant &value)
{
    if (change == QGraphicsLineItem::ItemSelectedHasChanged)
        emit selectedChange(this);
    return value;
}
