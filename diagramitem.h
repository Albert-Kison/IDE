#ifndef DIAGRAMITEM_H
#define DIAGRAMITEM_H

// This class represents a flowchart shape in the DiagramScene

#include "diagramtextitem.h"

#include <QGraphicsPixmapItem>
#include <QList>
#include <QGraphicsTextItem>

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
    enum { Type = UserType + 15 };
    enum DiagramType { Step, Conditional, StartEnd, Io, Table };   // flowchart shapes

    DiagramItem(DiagramType diagramType, QMenu *contextMenu, QGraphicsItem *parent = nullptr);

    void removeArrow(Arrow *arrow);
    void removeArrows();
    DiagramType diagramType() const { return myDiagramType; }
    QPolygonF polygon() const { return myPolygon; }
    void addArrow(Arrow *arrow);
    int addItem(QString& name, QString& type);
    void updateText(const QString& newText);
    void updateItem(int index, const QString& newText, QString& newDataType);
    void removeColumn(int index);
    void drawColumns();
    void namePolygon(const QPolygonF& polygon);
    QPixmap image() const;
    int type() const override { return Type; }

    DiagramType myDiagramType;
    QGraphicsTextItem *textItem;

    // QStringList items;

    struct Column
    {
        QString name;
        QString dataType;
    };

    QVector<Column> columns;

public slots:
    void updateTextPosition();    

signals:
    void selectedChange(QGraphicsItem *item);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    // void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    // void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    // void keyPressEvent(QKeyEvent *event) override;

private:
    QPolygonF myPolygon;

    QPolygonF tableNamePolygon;
    QPolygonF itemListPolygon;

    QMenu *myContextMenu;
    QList<Arrow *> arrows;
};
//! [0]

#endif // DIAGRAMITEM_H
