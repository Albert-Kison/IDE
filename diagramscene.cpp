#include "diagramscene.h"
#include "QtCore/qjsonarray.h"
#include "QtCore/qjsonobject.h"
#include "arrow.h"

#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <iostream>



DiagramScene::DiagramScene(QObject *parent)
    : QGraphicsScene(parent)
{
    // myItemMenu = itemMenu;  // item actions
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
        item = drawTable();
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
        textItem = drawText();
        textItem->setFont(myFont);
        // textItem->setTextInteractionFlags(Qt::TextEditorInteraction);   // make it editable
        // textItem->setZValue(1000.0);
        // connect(textItem, &DiagramTextItem::lostFocus,
        //         this, &DiagramScene::editorLostFocus);
        // connect(textItem, &DiagramTextItem::selectedChange,
        //         this, &DiagramScene::itemSelected);
        // addItem(textItem);
        textItem->setDefaultTextColor(myTextColor);
        textItem->setPos(mouseEvent->scenePos());
        emit textInserted(textItem);

    default:    // MoveItem mode
        ;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}



DiagramTextItem* DiagramScene::drawText() {
    DiagramTextItem* textItem = new DiagramTextItem();
    textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
    textItem->setZValue(1000.0);
    connect(textItem, &DiagramTextItem::lostFocus,
            this, &DiagramScene::editorLostFocus);
    connect(textItem, &DiagramTextItem::selectedChange,
            this, &DiagramScene::itemSelected);
    addItem(textItem);

    return textItem;
}



DiagramItem* DiagramScene::drawTable() {
    DiagramItem* item = new DiagramItem();
    item->setBrush(myItemColor);
    connect(item, &DiagramItem::selectedChange,
            this, &DiagramScene::itemSelected);
    addItem(item);

    return item;
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



// parse JSON and draw the scene
QRectF DiagramScene::openScene(QJsonObject& sceneDataJSON) {
    clear();

    QRectF focusRect;
    std::cout << "Called open scene in scene" << std::endl;

    // array of text items
    QJsonArray textArray = sceneDataJSON["text_items"].toArray();
    // array of table items
    QJsonArray tablesArray = sceneDataJSON["table_items"].toArray();
    // array of arrow items
    QJsonArray arrowsArray = sceneDataJSON["arrow_items"].toArray();


    // draw the text items
    for (const QJsonValue& textValue : textArray) {
        QJsonObject textJSON = textValue.toObject();
        // create a text item
        DiagramTextItem* textItem = drawText();
        textItem->setPlainText(textJSON["text"].toString());
        textItem->setPos(QPointF(textJSON["x"].toDouble(), textJSON["y"].toDouble()));
        textItem->setDefaultTextColor(QColor(textJSON["color"].toString()));

        // set the font
        QFont font;
        font.fromString(textJSON["font_text"].toString());
        font.setPointSize(textJSON["font_size"].toInt());
        font.setWeight(textJSON["is_bold"].toBool() ? QFont::Bold : QFont::Normal);
        font.setItalic(textJSON["is_italic"].toBool());
        font.setUnderline(textJSON["is_underline"].toBool());
        textItem->setFont(font);

        emit textInserted(textItem);

        focusRect = focusRect.united(textItem->sceneBoundingRect());
    }


    // draw the tables
    std::cout << "Tables array size: " << tablesArray.size() << std::endl;
    for (const QJsonValue& tableValue : tablesArray) {
        QJsonObject tableJSON = tableValue.toObject();
        // create a table
        DiagramItem *tableItem = drawTable();
        tableItem->setPos(QPointF(tableJSON["x"].toDouble(), tableJSON["y"].toDouble()));
        tableItem->updateName(tableJSON["name"].toString());

        // add the columns
        QJsonArray columns = tableJSON["columns"].toArray();
        for (const QJsonValue& columnValue : columns) {
            QJsonObject columnJSON = columnValue.toObject();

            tableItem->addItem(columnJSON["name"].toString(), columnJSON["type"].toString(), columnJSON["is_primary"].toBool());
        }

        emit itemInserted(tableItem);
        std::cout << "Table opened" << std::endl;

        focusRect = focusRect.united(tableItem->sceneBoundingRect());
    }


    // draw the arrows
    for (const QJsonValue& arrowValue : arrowsArray) {
        QJsonObject arrowJSON = arrowValue.toObject();

        DiagramItem *startTable;
        DiagramItem *endTable;
        foreach (QGraphicsItem* item, items()) {
            if (DiagramItem *currentItem = qgraphicsitem_cast<DiagramItem *>(item)) {
                // get the parent table
                if (currentItem->table.name == arrowJSON["start_item_name"].toString()) {
                    startTable = currentItem;
                }

                if (currentItem->table.name == arrowJSON["end_item_name"].toString()) {
                    endTable = currentItem;
                }
            }
        }

        Arrow *arrow = new Arrow(startTable, endTable);

        // add the column relationships
        Column *startColumn = nullptr;
        Column *endColumn = nullptr;
        QJsonArray columnRelationships = arrowJSON["column_relationships"].toArray();
        for (const QJsonValue& columnRelationshipValue : columnRelationships) {
            QJsonObject columnRelationshipJSON = columnRelationshipValue.toObject();

            // get the start and end columns
            for (Column& col : startTable->table.columns) {
                std::cout << "Start column: " << col.name.toStdString() << std::endl;
                std::cout << (col.name == columnRelationshipJSON["start_column_name"].toString()) << std::endl;
                if (col.name == columnRelationshipJSON["start_column_name"].toString()) {
                    startColumn = &col;
                }
            }
            for (Column& col : endTable->table.columns) {
                if (col.name == columnRelationshipJSON["end_column_name"].toString()) {
                    endColumn = &col;
                }
            }


            if (startColumn != nullptr && endColumn != nullptr) {
                std::cout << "Not null" << std::endl;
                arrow->columnRelationships.append(ColumnRelationship(startColumn, endColumn));
            }
        }

        arrow->setColor(Qt::black);

        // add arrow to each item and to the scene
        startTable->addArrow(arrow);
        endTable->addArrow(arrow);
        arrow->setZValue(-1000.0);
        connect(arrow, &Arrow::selectedChange, this, &DiagramScene::itemSelected);
        clearSelection();
        arrow->setSelected(true);
        addItem(arrow);

        // update its position when the items are being moved
        arrow->updatePosition();
    }

    return focusRect;
}



QRectF DiagramScene::drawDiagram(QList<Table> &tables, QList<Relationship> &relationships) {
    // Clear the scene before drawing
    clear();

    // middle of the view
    qreal x = 5000 / 2;
    qreal y = 5000 / 2;
    QRectF focusRect;

    // draw tables
    for (int i = 0; i < tables.size(); ++i) {
        DiagramItem *item = drawTable();

        // change the coordinates so that the items do not overlap
        if (i % 2 == 0) {
            x += 250;
        } else {
            y += 250;
        }

        item->setPos(QPointF(x, y));
        item->updateName(tables[i].name);   // assign the table's name

        // view.centerOn(item);

        // add columns to the table
        for (int j = 0; j < tables[i].columns.size(); ++j) {
            item->addItem(tables[i].columns[j].name, tables[i].columns[j].type, tables[i].columns[j].isPrimary);
        }

        emit itemInserted(item);

        focusRect = focusRect.united(item->sceneBoundingRect());

        x = 5000 / 2;   // reset the x coordinate
    }


    // draw relationships
    QList<Arrow *> arrows;
    std::cout << "Size: " << relationships.size() << std::endl;
    for (int i = 0; i < relationships.size(); ++i) {
        std::cout << "Parent table: " << relationships[i].parentTableName.toStdString() << std::endl;
        std::cout << "Parent column: " << relationships[i].parentColumnName.toStdString() << std::endl;
        std::cout << "Child table: " << relationships[i].childTableName.toStdString() << std::endl;
        std::cout << "Child column: " << relationships[i].childColumnName.toStdString() << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        DiagramItem *startTable;
        DiagramItem *endTable;
        Column *startColumn;
        Column *endColumn;

        // get the start and end tables and columns
        foreach (QGraphicsItem* item, items()) {
            if (DiagramItem *currentItem = qgraphicsitem_cast<DiagramItem *>(item)) {
                // get the parent table
                if (currentItem->table.name == relationships[i].parentTableName) {
                    startTable = currentItem;

                    // get the parent column
                    for (Column& col : startTable->table.columns) {
                        if (col.name == relationships[i].parentColumnName) {
                            startColumn = &col;
                        }
                    }
                }

                // get the child table
                if (currentItem->table.name == relationships[i].childTableName) {
                    endTable = currentItem;

                    // get the child column
                    for (Column& col : endTable->table.columns) {
                        if (col.name == relationships[i].childColumnName) {
                            endColumn = &col;
                        }
                    }
                }
            }
        }

        // assign a relationship to the same arrow if it is one-to-many
        bool hasDuplicate = false;
        for (int j = 0; j < arrows.size(); ++j) {
            // if parent table and child table names are the same
            if (startTable->table.name == arrows[j]->startItem()->table.name && endTable->table.name == arrows[j]->endItem()->table.name) {
                hasDuplicate = true;
                arrows[j]->columnRelationships << ColumnRelationship(startColumn, endColumn);
                clearSelection();
                arrows[j]->setSelected(true);
                // break;
            }
        }

        // if no one-to-many, then create a new arrow
        if (!hasDuplicate) {
            // draw the relationships
            Arrow *arrow = new Arrow(startTable, endTable);
            arrow->columnRelationships << ColumnRelationship(startColumn, endColumn);
            arrow->setColor(Qt::black);

            // add arrow to each item and to the scene
            startTable->addArrow(arrow);
            endTable->addArrow(arrow);
            arrow->setZValue(-1000.0);
            connect(arrow, &Arrow::selectedChange, this, &DiagramScene::itemSelected);
            clearSelection();
            arrow->setSelected(true);
            addItem(arrow);

            // update its position when the items are being moved
            arrow->updatePosition();

            arrows << arrow;
        }
    }

    return focusRect;
}



// check whether a selected item exists and also is of the specified diagram type
bool DiagramScene::isItemChange(int type) const
{
    const QList<QGraphicsItem *> items = selectedItems();
    const auto cb = [type](const QGraphicsItem *item) { return item->type() == type; };
    return std::find_if(items.begin(), items.end(), cb) != items.end();
}
