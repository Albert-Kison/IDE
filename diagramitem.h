#ifndef DIAGRAMITEM_H
#define DIAGRAMITEM_H

// This class represents a flowchart shape in the DiagramScene

#include <QGraphicsPixmapItem>
#include <QList>

QT_BEGIN_NAMESPACE
class QPixmap;
class QGraphicsSceneContextMenuEvent;
class QMenu;
class QPolygonF;
QT_END_NAMESPACE

class Arrow;


class DiagramItem : public QGraphicsPolygonItem
{
public:
    // unique identifier of the class that is used by qgraphicsitem_cast(), which does dynamic casts of graphics items
    enum { Type = UserType + 15 };
    enum DiagramType { Step, Conditional, StartEnd, Io };   // flowchart shapes

    DiagramItem(DiagramType diagramType, QMenu *contextMenu, QGraphicsItem *parent = nullptr);

    void removeArrow(Arrow *arrow);
    void removeArrows();
    DiagramType diagramType() const { return myDiagramType; }
    QPolygonF polygon() const { return myPolygon; }
    void addArrow(Arrow *arrow);
    QPixmap image() const;
    int type() const override { return Type; }

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    DiagramType myDiagramType;
    QPolygonF myPolygon;
    QMenu *myContextMenu;
    QList<Arrow *> arrows;
};
//! [0]

#endif // DIAGRAMITEM_H
