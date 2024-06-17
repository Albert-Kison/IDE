#include "diagramitem.h"
#include "QtGui/qtextdocument.h"
#include "arrow.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QObject>
// #include <iostream>



// std::ostream&  operator <<(std::ostream &stream,const QString &str)
// {
//     stream << str.toLatin1().constData(); //or: stream << str.toStdString(); //??
//     return stream;
// }



// create an item according to its type
DiagramItem::DiagramItem(QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
    , table("New item")
{

    // Create polygons for the table name and the item list
    tableNamePolygon << QPointF(-100, -75) << QPointF(100, -75)
                     << QPointF(100, -25) << QPointF(-100, -25)
                     << QPointF(-100, -75);

    itemListPolygon << QPointF(-100, -25) << QPointF(100, -25)
                    << QPointF(100, 75) << QPointF(-100, 75)
                    << QPointF(-100, -25);

    myPolygon = tableNamePolygon + itemListPolygon;

    setPolygon(myPolygon);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    // draw name on the diagram item
    drawName(tableNamePolygon);
}



void DiagramItem::drawName(const QPolygonF& polygon) {

    // Delete existing text items within the polygon
    QList<QGraphicsItem*> childItemsList = childItems();
    for (QGraphicsItem* item : childItemsList) {
        // Check if the item is a QGraphicsTextItem
        QGraphicsTextItem* textItem = dynamic_cast<QGraphicsTextItem*>(item);
        if (textItem) {
            // Check if the position of the text item falls within the specified polygon
            QPointF itemPos = textItem->pos();
            if (polygon.containsPoint(itemPos, Qt::OddEvenFill)) {
                // If it does, delete the text item
                delete textItem;
            }
        }
    }

    QGraphicsTextItem *tableName = new QGraphicsTextItem();
    tableName->setPlainText(table.name);
    // textItem->setFlags(ItemIsSelectable | ItemIsFocusable);

    // Set the width of the bounding rectangle
    tableName->setTextWidth(200);

    // Set the alignment of the text within the bounding rectangle
    QTextOption textOption;
    textOption.setAlignment(Qt::AlignCenter);
    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    tableName->document()->setDefaultTextOption(textOption);

    // Put the text in the center
    QPointF center = polygon.boundingRect().center();
    tableName->setPos(center.x() - tableName->boundingRect().width() / 2,
                     center.y() - tableName->boundingRect().height() / 2);
    tableName->setDefaultTextColor(Qt::black);

    // Set the DiagramItem as the parent item of the text item
    tableName->setParentItem(this);

    // Add the text item to the scene
    scene()->addItem(tableName);
}



// used to remove Arrow items when they or DiagramItems they are connected to are removed from the scene
void DiagramItem::removeArrow(Arrow *arrow)
{
    arrows.removeAll(arrow);
}



// called when the item is removed from the scene and removes all arrows that are connected to this item
void DiagramItem::removeArrows()
{
    // need a copy here since removeArrow() will
    // modify the arrows container
    const auto arrowsCopy = arrows;
    for (Arrow *arrow : arrowsCopy) {
        arrow->startItem()->removeArrow(arrow);
        arrow->endItem()->removeArrow(arrow);
        scene()->removeItem(arrow);
        delete arrow;
    }
}



// add new arrow
void DiagramItem::addArrow(Arrow *arrow)
{
    arrows.append(arrow);
}



QStringList DiagramItem::getColumns() const {
    QStringList columnNames;
    for (const auto& column : table.columns) {
        columnNames.append(column.name + " (" + column.type + ")");
    }
    return columnNames;
}



void DiagramItem::updateName(const QString& newText) {
    table.name = newText;
    drawName(tableNamePolygon);
}



// void DiagramItem::updateTextPosition() {
//     // Ensure that the text item exists
//     if (!tableName)
//         return;

//     // std::cout << "iodhfier" << std::endl;

//     // Calculate the new position based on the center point of the item's polygon
//     QPointF center = polygon().boundingRect().center();
//     tableName->setPos(center.x() - tableName->boundingRect().width() / 2,
//                      center.y() - tableName->boundingRect().height() / 2);
// }



// get the image of the item to create icons for the tool buttons in the tool box
QPixmap DiagramItem::image() const
{
    QPixmap pixmap(250, 250);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));
    painter.translate(125, 125);
    painter.drawPolyline(myPolygon);

    return pixmap;
}



// show the context menu when the right mouse button is clicked
// void DiagramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
// {
//     scene()->clearSelection();
//     setSelected(true);  // item must be selected to change its elevation with the bringToFront and sendToBack actions
//     myContextMenu->popup(event->screenPos());
// }



// update the position of the arrows when the item is moved
QVariant DiagramItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        for (Arrow *arrow : std::as_const(arrows))
            arrow->updatePosition();
    }

    if (change == QGraphicsItem::ItemSelectedHasChanged)
        emit selectedChange(this);

    return value;
}



void DiagramItem::drawColumns() {

    // Clear existing text items
    QList<QGraphicsItem*> childItemsList = childItems();
    for (QGraphicsItem* item : childItemsList) {
        // Check if the item is a QGraphicsTextItem
        QGraphicsTextItem* textItem = dynamic_cast<QGraphicsTextItem*>(item);
        if (textItem) {
            // Check if the position of the text item falls within the specified polygon
            QPointF itemPos = textItem->pos();
            if (itemListPolygon.containsPoint(itemPos, Qt::OddEvenFill)) {
                // If it does, delete the text item
                delete textItem;
            }
        }
    }

    // Clear existing item list polygon
    itemListPolygon.clear();

    // Update the size of the item list polygon
    itemListPolygon << QPointF(-100, -25) << QPointF(100, -25)
                    << QPointF(100, -15 + 20 * table.columns.size()) << QPointF(-100, -15 + 20 * table.columns.size())
                    << QPointF(-100, -25);

    // Update tablePolygon by combining tableNamePolygon and itemListPolygon
    myPolygon = tableNamePolygon + itemListPolygon;

    // Update the path of the table item
    setPolygon(myPolygon);

    qreal y = itemListPolygon.boundingRect().y(); // Initial y-coordinate for the first item name
    for (const Column &column : table.columns) {
        QGraphicsTextItem *textItem = new QGraphicsTextItem(column.name + " (" + column.type + ")" + (column.isPrimary ? " Primary" : ""));
        textItem->setDefaultTextColor(Qt::red);

        // Set the position of the text item relative to the itemListPolygon
        textItem->setPos(itemListPolygon.boundingRect().x() + 10, y);
        textItem->setParentItem(this);

        scene()->addItem(textItem);

        // Adjust the y-coordinate to ensure the text remains inside the itemListPolygon
        y += 20; // Adjust as needed
    }
}




void DiagramItem::addItem(const QString& name, const QString &type, bool isPrimary) {
    // std::cout << "Add item: " << name << "(" << type << ")" << std::endl;
    if (isPrimary) {
        for (int i = 0; i < table.columns.size(); i++) {
            table.columns[i].isPrimary = false;
        }
        Column column(name, type, true);
        table.columns << column;
    } else {
        Column column(name, type, table.columns.size() == 0 ? true : false);
        table.columns << column;
    }

    drawColumns();
}



void DiagramItem::updateColumn(int index, Column& newColumn) {
    if (table.columns[index].isPrimary != newColumn.isPrimary) {
        // reset primary
        for (int i = 0; i < table.columns.size(); i++) {
            table.columns[i].isPrimary = false;
        }
    }

    table.columns[index] = newColumn;

    drawColumns();
}



void DiagramItem::removeColumn(int index) {
    if (index >= 0 && index < table.columns.size()) {
        table.columns.remove(index);
        if (table.columns.size() > 0) {
            table.columns[0].isPrimary = true;
        }
    }

    drawColumns();
}
