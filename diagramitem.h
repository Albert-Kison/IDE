#ifndef DIAGRAMITEM_H
#define DIAGRAMITEM_H

// This class represents a flowchart shape in the DiagramScene

// #include "diagramtextitem.h"

#include <QGraphicsPixmapItem>
#include <QList>
#include <QGraphicsTextItem>
#include "Table.h"

QT_BEGIN_NAMESPACE
class QPixmap;
class QGraphicsSceneContextMenuEvent;
class QMenu;
class QPolygonF;
QT_END_NAMESPACE

class Arrow;


class DiagramItem : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT

public:
    // unique identifier of the class that is used by qgraphicsitem_cast(), which does dynamic casts of graphics items
    // enum { Type = UserType + 15 };

    DiagramItem(QGraphicsItem *parent = nullptr);

    void removeArrow(Arrow *arrow);
    void removeArrows();
    QPolygonF polygon() const { return myPolygon; }
    void addArrow(Arrow *arrow);
    void addItem(const QString& name, const QString& type, bool isPrimary = false);
    void updateName(const QString& newText);
    void updateColumn(int index, Column& column);
    void removeColumn(int index);
    void drawColumns();
    void drawName(const QPolygonF& polygon);
    QStringList getColumns() const;
    QPixmap image() const;
    int type() const override { return Type; }

    Table table;

// public slots:
//     void updateTextPosition();

signals:
    void selectedChange(QGraphicsItem *item);

protected:
    // void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    // void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    // void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    // void keyPressEvent(QKeyEvent *event) override;

private:
    QPolygonF myPolygon;

    QPolygonF tableNamePolygon;
    QPolygonF itemListPolygon;

    // QMenu *myContextMenu;
    QList<Arrow *> arrows;
};
//! [0]

#endif // DIAGRAMITEM_H
