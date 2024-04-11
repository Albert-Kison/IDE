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
DiagramItem::DiagramItem(DiagramType diagramType, QMenu *contextMenu,
                         QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent), myDiagramType(diagramType)
    , myContextMenu(contextMenu)
{
    textItem = nullptr;

    QPainterPath path;
    switch (myDiagramType) {
    case StartEnd:
        path.moveTo(200, 50);
        path.arcTo(150, 0, 50, 50, 0, 90);
        path.arcTo(50, 0, 50, 50, 90, 90);
        path.arcTo(50, 50, 50, 50, 180, 90);
        path.arcTo(150, 50, 50, 50, 270, 90);
        path.lineTo(200, 25);
        myPolygon = path.toFillPolygon();
        break;
    case Conditional:
        myPolygon << QPointF(-100, 0) << QPointF(0, 100)
                  << QPointF(100, 0) << QPointF(0, -100)
                  << QPointF(-100, 0);
        break;
    case Step:
        myPolygon << QPointF(-100, -100) << QPointF(100, -100)
                  << QPointF(100, 100) << QPointF(-100, 100)
                  << QPointF(-100, -100);
        break;
    case Table:
        // Create polygons for the table name and the item list
        tableNamePolygon << QPointF(-100, -75) << QPointF(100, -75)
                         << QPointF(100, -25) << QPointF(-100, -25)
                         << QPointF(-100, -75);

        itemListPolygon << QPointF(-100, -25) << QPointF(100, -25)
                        << QPointF(100, 75) << QPointF(-100, 75)
                        << QPointF(-100, -25);

        myPolygon = tableNamePolygon + itemListPolygon;
        break;
    default:
        myPolygon << QPointF(-120, -80) << QPointF(-70, 80)
                  << QPointF(120, 80) << QPointF(70, -80)
                  << QPointF(-120, -80);
        break;
    }
    setPolygon(myPolygon);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    // name the table
    if (myDiagramType == DiagramItem::Table) {
        namePolygon(tableNamePolygon);
    }
}



void DiagramItem::namePolygon(const QPolygonF& polygon) {
    textItem = new QGraphicsTextItem();
    textItem->setPlainText("New name");
    // textItem->setFlags(ItemIsSelectable | ItemIsFocusable);

    // Set the width of the bounding rectangle
    textItem->setTextWidth(200);

    // Set the alignment of the text within the bounding rectangle
    QTextOption textOption;
    textOption.setAlignment(Qt::AlignCenter);
    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    textItem->document()->setDefaultTextOption(textOption);

    // Put the text in the center
    QPointF center = polygon.boundingRect().center();
    textItem->setPos(center.x() - textItem->boundingRect().width() / 2,
                     center.y() - textItem->boundingRect().height() / 2);
    textItem->setDefaultTextColor(Qt::black);

    // Set the DiagramItem as the parent item of the text item
    textItem->setParentItem(this);

    // Add the text item to the scene
    scene()->addItem(textItem);
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



void DiagramItem::updateText(const QString& newText) {
    textItem->setPlainText(newText);
}



void DiagramItem::updateTextPosition() {
    // Ensure that the text item exists
    if (!textItem)
        return;

    // std::cout << "iodhfier" << std::endl;

    // Calculate the new position based on the center point of the item's polygon
    QPointF center = polygon().boundingRect().center();
    textItem->setPos(center.x() - textItem->boundingRect().width() / 2,
                     center.y() - textItem->boundingRect().height() / 2);
}



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
void DiagramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene()->clearSelection();
    setSelected(true);  // item must be selected to change its elevation with the bringToFront and sendToBack actions
    myContextMenu->popup(event->screenPos());
}



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
    QList<QGraphicsItem*> childItems = this->childItems();
    for (int i = 1; i < childItems.size(); i++) {
        QGraphicsTextItem *textItem = qgraphicsitem_cast<QGraphicsTextItem*>(childItems[i]);
        // Check if the item is a QGraphicsTextItem and its index matches the updated item
        if (textItem) {
            delete textItem;
        }
    }

    // Clear existing item list polygon
    itemListPolygon.clear();

    // Update the size of the item list polygon
    itemListPolygon << QPointF(-100, -25) << QPointF(100, -25)
                    << QPointF(100, -15 + 20 * columns.size()) << QPointF(-100, -15 + 20 * columns.size())
                    << QPointF(-100, -25);

    // Update tablePolygon by combining tableNamePolygon and itemListPolygon
    myPolygon = tableNamePolygon + itemListPolygon;

    // Update the path of the table item
    setPolygon(myPolygon);

    qreal y = itemListPolygon.boundingRect().y(); // Initial y-coordinate for the first item name
    for (const Column &column : columns) {
        QGraphicsTextItem *textItem = new QGraphicsTextItem(column.name);
        textItem->setDefaultTextColor(Qt::red);

        // Set the position of the text item relative to the itemListPolygon
        textItem->setPos(itemListPolygon.boundingRect().x() + 10, y);
        textItem->setParentItem(this);

        scene()->addItem(textItem);

        // Adjust the y-coordinate to ensure the text remains inside the itemListPolygon
        y += 20; // Adjust as needed
    }
}




int DiagramItem::addItem(QString& name, QString &type) {
    // std::cout << "Add item: " << name << "(" << type << ")" << std::endl;
    Column column;
    column.name = name;
    column.dataType = type;
    columns.append(column);

    drawColumns();

    return columns.size() - 1;
}



void DiagramItem::updateItemText(int index, const QString &newText) {
    columns[index].name = newText;

    // Iterate over the child items of the DiagramItem
    foreach (QGraphicsItem *item, childItems()) {
        QGraphicsTextItem *textItem = qgraphicsitem_cast<QGraphicsTextItem*>(item);
        // Check if the item is a QGraphicsTextItem and its index matches the updated item
        if (textItem && childItems().indexOf(textItem) == index + 1) {
            // Update the text of the QGraphicsTextItem
            textItem->setPlainText(newText);
            break; // No need to continue iterating once the item is found and updated
        }
    }
}



void DiagramItem::removeColumn(int index) {
    if (index >= 0 && index < columns.size()) {
        columns.remove(index);
    }

    drawColumns();
}



// void DiagramItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
// {
//     if (event->button() == Qt::LeftButton) {
//         emit itemClicked(this);
//     }

//     QGraphicsItem::mousePressEvent(event);
// }



// void DiagramItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
// {
//     if (event->button() == Qt::LeftButton) {
//         // Get the position of the click relative to the DiagramItem
//         QPointF pos = mapFromScene(event->scenePos());

//         // get the polygon that was clicked
//         QPolygonF clickedPolygon = this->polygon();
//         if (myDiagramType == DiagramType::Table) {
//             // only allow to change the text in the tableNamePolygon
//             if (itemListPolygon.containsPoint(pos, Qt::OddEvenFill)) {
//                 QStringList items;
//                 items << "orfoirehf" << "uierhfieh" << "iwhhrfiuheri" << "iuhihfeirhu";

//                 // Update itemListPolygon based on the number of items
//                 itemListPolygon.clear();
//                 itemListPolygon << QPointF(-100, -25) << QPointF(100, -25)
//                                 << QPointF(100, 75 + 20 * items.size()) << QPointF(-100, 75 + 20 * items.size())
//                                 << QPointF(-100, -25);

//                 // Update tablePolygon by combining tableNamePolygon and itemListPolygon
//                 myPolygon = tableNamePolygon + itemListPolygon;

//                 // Update the path of the table item
//                 setPolygon(myPolygon);

//                 // QPointF center = clickedPolygon.boundingRect().center();

//                 // Loop through each point of the polygon and map it to scene coordinates
//                 QPolygonF sceneItemListPolygon;
//                 for (const QPointF &point : itemListPolygon) {
//                     QPointF scenePoint = mapToScene(point);
//                     sceneItemListPolygon << scenePoint;
//                 }

//                 // Add item names
//                 // std::cout << "Start y coordinate: " << sceneItemListPolygon.boundingRect().y() << std::endl;
//                 qreal y = sceneItemListPolygon.boundingRect().y(); // Initial y-coordinate for the first item name
//                 for (const QString &itemName : items) {
//                     QGraphicsTextItem *textItem = new QGraphicsTextItem(itemName);
//                     textItem->setDefaultTextColor(Qt::black);

//                     // Set the position of the text item relative to the itemListPolygon
//                     textItem->setPos(sceneItemListPolygon.boundingRect().x() + 10, y);

//                     scene()->addItem(textItem);

//                     // Adjust the y-coordinate to ensure the text remains inside the itemListPolygon
//                     y += sceneItemListPolygon.boundingRect().height() / items.size(); // Adjust as needed
//                 }
//             }
//         }
//     }
// }
