#ifndef ARROW_H
#define ARROW_H

// graphics item that connects two DiagramItems

#include <QGraphicsLineItem>

class DiagramItem;

class Arrow : public QGraphicsLineItem
{
public:
    enum { Type = UserType + 4 };

    Arrow(DiagramItem *startItem, DiagramItem *endItem,
          QGraphicsItem *parent = nullptr);

    int type() const override { return Type; }

    // used to check for collisions and selections
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    void setColor(const QColor &color) { myColor = color; }
    DiagramItem *startItem() const { return myStartItem; }
    DiagramItem *endItem() const { return myEndItem; }

    void updatePosition();

protected:
    // in order to draw the head of an arrow
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

private:
    // diagram items that the arrow connects
    DiagramItem *myStartItem;
    DiagramItem *myEndItem;

    QPolygonF arrowHead;
    QColor myColor = Qt::black;
};


#endif // ARROW_H
