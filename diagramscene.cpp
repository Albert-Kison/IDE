#include "diagramscene.h"
#include "arrow.h"

#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <iostream>



DiagramScene::DiagramScene(QMenu *itemMenu, QObject *parent)
    : QGraphicsScene(parent)
{
    myItemMenu = itemMenu;  // item actions
    myMode = MoveItem;  // default behavior
    line = nullptr;
    textItem = nullptr;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = Qt::black;
}



// change line color
void DiagramScene::setLineColor(const QColor &color)
{
    myLineColor = color;

    // change the color of the selected arrow item
    if (isItemChange(Arrow::Type)) {
        Arrow *item = qgraphicsitem_cast<Arrow *>(selectedItems().first());
        item->setColor(myLineColor);
        update();
    }
}



// change text color
void DiagramScene::setTextColor(const QColor &color)
{
    myTextColor = color;

    // change the color of the selected text item
    if (isItemChange(DiagramTextItem::Type)) {
        DiagramTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        item->setDefaultTextColor(myTextColor);
    }
}



// change item color
void DiagramScene::setItemColor(const QColor &color)
{
    myItemColor = color;

    // change the color of the selected diagram item
    if (isItemChange(DiagramItem::Type)) {
        DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(selectedItems().first());
        item->setBrush(myItemColor);
    }
}



// change the font
void DiagramScene::setFont(const QFont &font)
{
    myFont = font;

    // change the font of the selected text item
    if (isItemChange(DiagramTextItem::Type)) {
        QGraphicsTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());

        //At this point the selection can change so the first selected item might not be a DiagramTextItem
        if (item)
            item->setFont(myFont);
    }
}



// set the scene mode
void DiagramScene::setMode(Mode mode)
{
    myMode = mode;
}



// emitted by DiagramTextItem when it loses focus
void DiagramScene::editorLostFocus(DiagramTextItem *item)
{
    QTextCursor cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);

    // remove the item if there is no text
    if (item->toPlainText().isEmpty()) {
        removeItem(item);
        item->deleteLater();
    }
}



// handles mouse press events
void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    std::cout << "Scene clicked" << std::endl;

    // if it is not a left button, exit the function
    if (mouseEvent->button() != Qt::LeftButton)
        return;


    DiagramItem *item;
    switch (myMode) {
    case InsertItem:
        std::cout << "In the insert item mode" << std::endl;
        // create a new DiagramItem and add it to the scene
        item = new DiagramItem(myItemMenu);
        item->setBrush(myItemColor);
        connect(item, &DiagramItem::selectedChange,
                this, &DiagramScene::itemSelected);
        addItem(item);
        item->setPos(mouseEvent->scenePos());
        std::cout << "Emit the item inserted signal" << std::endl;
        emit itemInserted(item);
        std::cout << "Signal is emitted" << std::endl;
        break;

    case InsertLine:
        // stretch the line while the mouse is held down
        line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
                                            mouseEvent->scenePos()));
        line->setPen(QPen(myLineColor, 2));
        addItem(line);
        break;

    case InsertText:
        // create a text item
        textItem = new DiagramTextItem();
        textItem->setFont(myFont);
        textItem->setTextInteractionFlags(Qt::TextEditorInteraction);   // make it editable
        textItem->setZValue(1000.0);
        connect(textItem, &DiagramTextItem::lostFocus,
                this, &DiagramScene::editorLostFocus);
        connect(textItem, &DiagramTextItem::selectedChange,
                this, &DiagramScene::itemSelected);
        addItem(textItem);
        textItem->setDefaultTextColor(myTextColor);
        textItem->setPos(mouseEvent->scenePos());
        emit textInserted(textItem);

    default:    // MoveItem mode
        ;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}



// handle mouse move
void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    // draw the line if the button is still pressed down
    if (myMode == InsertLine && line != nullptr) {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    } else if (myMode == MoveItem) {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}



// handle mouse release
// creates an arrow if two items should be connected
void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    // if there is a line being drawn and the mode is InsertLine
    if (line != nullptr && myMode == InsertLine) {

        // get the start items under the line
        QList<QGraphicsItem *> startItems = items(line->line().p1());
        if (startItems.count() && startItems.first() == line)
            // remove the line itself
            startItems.removeFirst();

        // get the end items under the line
        QList<QGraphicsItem *> endItems = items(line->line().p2());
        if (endItems.count() && endItems.first() == line)
            // remove the line itself
            endItems.removeFirst();

        removeItem(line);
        delete line;


        // check if there are two different items under the line
        if (startItems.count() > 0 && endItems.count() > 0 &&
            startItems.first()->type() == DiagramItem::Type &&
            endItems.first()->type() == DiagramItem::Type &&
            startItems.first() != endItems.first()) {

            DiagramItem *startItem = qgraphicsitem_cast<DiagramItem *>(startItems.first());
            DiagramItem *endItem = qgraphicsitem_cast<DiagramItem *>(endItems.first());

            // create the arrow
            Arrow *arrow = new Arrow(startItem, endItem);
            arrow->setColor(myLineColor);

            // add arrow to each item and to the scene
            startItem->addArrow(arrow);
            endItem->addArrow(arrow);
            arrow->setZValue(-1000.0);
            connect(arrow, &Arrow::selectedChange, this, &DiagramScene::itemSelected);
            this->clearSelection();
            arrow->setSelected(true);
            addItem(arrow);

            // update its position when the items are being moved
            arrow->updatePosition();
        }
    }

    line = nullptr;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}



// check whether a selected item exists and also is of the specified diagram type
bool DiagramScene::isItemChange(int type) const
{
    const QList<QGraphicsItem *> items = selectedItems();
    const auto cb = [type](const QGraphicsItem *item) { return item->type() == type; };
    return std::find_if(items.begin(), items.end(), cb) != items.end();
}
