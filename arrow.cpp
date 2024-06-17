#include "arrow.h"
#include "diagramitem.h"

#include <QPainter>
#include <QPen>
#include <QtMath>
#include <iostream>



// create arrow from start item to end item
Arrow::Arrow(DiagramItem *startItem, DiagramItem *endItem, QGraphicsItem *parent)
    : QGraphicsLineItem(parent), myRelationshipType(OneToMany), myStartItem(startItem), myEndItem(endItem)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}



// the arrow is larger than the bounding rectangle of the QGraphicsLineItem
QRectF Arrow::boundingRect() const
{
    qreal extra = 20; // Extra space around the line for easier selection
    return QGraphicsLineItem::boundingRect().adjusted(-extra, -extra, extra, extra);
}



// used to check for collisions and selections with the mouse.
QPainterPath Arrow::shape() const
{
    QPainterPath path;

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

    // Create the painter path from the polyline
    path.moveTo(linePath[0]);
    for (int i = 1; i < linePath.size(); ++i) {
        path.lineTo(linePath[i]);
    }

    // Create a wider path for selection purposes
    QPainterPathStroker stroker;
    stroker.setWidth(10); // Width for easier selection
    QPainterPath strokedPath = stroker.createStroke(path);

    // Include the arrowhead in the shape path
    QPolygonF arrowHeadCopy = arrowHead;
    for (const QPointF &point : arrowHeadCopy)
    {
        strokedPath.addEllipse(point, 5, 5);
    }

    return strokedPath;
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
    // If the items collide, exit the function
    if (myStartItem->collidesWithItem(myEndItem))
        return;

    // Set the pen and brush for drawing
    QPen myPen = pen();
    myPen.setColor(myColor);
    qreal arrowSize = 20;
    painter->setPen(myPen);
    painter->setBrush(myColor);

    // Find the start and end positions
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
    // std::cout << "Last segment {(" << lastSegment.x1() << ", " << lastSegment.y1() << "), (" << lastSegment.x2() << ", " << lastSegment.y2() << ")}" << std::endl;
    double angle = std::atan2(-lastSegment.dy(), lastSegment.dx());

    // Polygon points of the arrow head
    QPointF arrowP1 = linePath.last() + QPointF(sin(angle + M_PI / 3) * arrowSize,
                                                cos(angle + M_PI / 3) * arrowSize);
    QPointF arrowP2 = linePath.last() + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                                                cos(angle + M_PI - M_PI / 3) * arrowSize);

    // Draw the arrow head
    QPolygonF arrowHead;
    arrowHead << linePath.last() << arrowP1 << arrowP2;
    // painter->drawPolygon(arrowHead);


    if (myRelationshipType == OneToMany) {
        drawBranches(myEndItem, linePath, painter);
    } else if (myRelationshipType == ManyToMany) {
        drawBranches(myStartItem, linePath, painter);
        drawBranches(myEndItem, linePath, painter);
    }

    // If the arrow is selected, draw dashed lines around it
    if (isSelected()) {
        QPen dashedPen(myColor, 1, Qt::DashLine);
        painter->setPen(dashedPen);

        // Draw dashed lines all around the arrow
        qreal distance = 5; // Distance between each dashed line
        QPointF offset(distance, distance);
        for (int i = 0; i < 8; ++i) {
            QPointF offsetLine1 = linePath[0] + offset;
            QPointF offsetLine2 = linePath[1] + offset;
            QPointF offsetLine3 = linePath[2] + offset;
            painter->drawLine(offsetLine1, offsetLine2);
            painter->drawLine(offsetLine2, offsetLine3);
            offset.rx() *= -1; // Reverse direction
            offset.ry() *= -1; // Reverse direction
        }
    }
}



void Arrow::drawBranches(DiagramItem *withItem, QPolygonF linePath, QPainter *painter) {
    // Determine the direction of the last segment
    // Determine the intersection point
    QRectF withItemRect = withItem->boundingRect().translated(withItem->pos());
    QLineF intersectSegment;
    QPointF intersectionPoint;

    // List of edges of the end item
    QList<QLineF> withItemEdges;
    withItemEdges << QLineF(withItemRect.topLeft(), withItemRect.topRight())
                 << QLineF(withItemRect.bottomLeft(), withItemRect.bottomRight())
                 << QLineF(withItemRect.topLeft(), withItemRect.bottomLeft())
                 << QLineF(withItemRect.topRight(), withItemRect.bottomRight());

    // Loop through all segments of the line
    for (int i = 0; i < linePath.size() - 1; ++i) {
        QLineF currentSegment(linePath[i], linePath[i + 1]);

        // Check for intersection with each edge of the end item
        for (const QLineF &edge : withItemEdges) {
            if (currentSegment.intersects(edge, &intersectionPoint) == QLineF::BoundedIntersection) {
                intersectSegment = currentSegment;
                std::cout << "intersected" << std::endl;

                // Output the information about the last segment
                if (intersectSegment.x1() < intersectSegment.x2() && intersectSegment.y1() == intersectSegment.y2()) {

                    std::cout << "The line goes right from " << myStartItem->table.name.toStdString() << " to " << myEndItem->table.name.toStdString() << std::endl;
                    if (withItem == myEndItem) {
                        painter->drawLine(intersectionPoint.x() - 10, intersectionPoint.y(), intersectionPoint.x(), intersectionPoint.y() - 10);
                        painter->drawLine(intersectionPoint.x() - 10, intersectionPoint.y(), intersectionPoint.x(), intersectionPoint.y() + 10);
                    } else {
                        painter->drawLine(intersectionPoint.x() + 10, intersectionPoint.y(), intersectionPoint.x(), intersectionPoint.y() - 10);
                        painter->drawLine(intersectionPoint.x() + 10, intersectionPoint.y(), intersectionPoint.x(), intersectionPoint.y() + 10);
                    }

                } else if (intersectSegment.x1() > intersectSegment.x2() && intersectSegment.y1() == intersectSegment.y2()) {

                    std::cout << "The line goes left from " << myStartItem->table.name.toStdString() << " to " << myEndItem->table.name.toStdString() << std::endl;
                    if (withItem == myEndItem) {
                        painter->drawLine(intersectionPoint.x() + 10, intersectionPoint.y(), intersectionPoint.x(), intersectionPoint.y() - 10);
                        painter->drawLine(intersectionPoint.x() + 10, intersectionPoint.y(), intersectionPoint.x(), intersectionPoint.y() + 10);
                    } else {
                        painter->drawLine(intersectionPoint.x() - 10, intersectionPoint.y(), intersectionPoint.x(), intersectionPoint.y() - 10);
                        painter->drawLine(intersectionPoint.x() - 10, intersectionPoint.y(), intersectionPoint.x(), intersectionPoint.y() + 10);
                    }

                } else if (intersectSegment.y1() < intersectSegment.y2() && intersectSegment.x1() == intersectSegment.x2()) {

                    std::cout << "The line goes down from " << myStartItem->table.name.toStdString() << " to " << myEndItem->table.name.toStdString() << std::endl;
                    if (withItem == myEndItem) {
                        painter->drawLine(intersectionPoint.x(), intersectionPoint.y() - 10, intersectionPoint.x() + 10, intersectionPoint.y());
                        painter->drawLine(intersectionPoint.x(), intersectionPoint.y() - 10, intersectionPoint.x() - 10, intersectionPoint.y());
                    } else {
                        painter->drawLine(intersectionPoint.x(), intersectionPoint.y() + 10, intersectionPoint.x() + 10, intersectionPoint.y());
                        painter->drawLine(intersectionPoint.x(), intersectionPoint.y() + 10, intersectionPoint.x() - 10, intersectionPoint.y());
                    }

                } else if (intersectSegment.y1() > intersectSegment.y2() && intersectSegment.x1() == intersectSegment.x2()) {

                    std::cout << "The line goes up from " << myStartItem->table.name.toStdString() << " to " << myEndItem->table.name.toStdString() << std::endl;
                    if (withItem == myEndItem) {
                        painter->drawLine(intersectionPoint.x(), intersectionPoint.y() + 10, intersectionPoint.x() + 10, intersectionPoint.y());
                        painter->drawLine(intersectionPoint.x(), intersectionPoint.y() + 10, intersectionPoint.x() - 10, intersectionPoint.y());
                    } else {
                        painter->drawLine(intersectionPoint.x(), intersectionPoint.y() - 10, intersectionPoint.x() + 10, intersectionPoint.y());
                        painter->drawLine(intersectionPoint.x(), intersectionPoint.y() - 10, intersectionPoint.x() - 10, intersectionPoint.y());
                    }

                } else {
                    std::cout << "The line is diagonal or complex." << std::endl;
                }

                break;
            }
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
