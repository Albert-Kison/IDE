#include "arrow.h"
#include "diagramscene.h"
#include "diagramtextitem.h"
#include "diagram.h"

#include <QtWidgets>
#include <iostream>



// std::ostream&  operator <<(std::ostream &stream,const QString &str)
// {
//     stream << str.toLatin1().constData();
//     return stream;
// }

const int InsertTextButton = 10;


Diagram::Diagram()
{
    // create the widgets, layouts, and the scene
    createActions();
    createMenus();

    scene = new DiagramScene(itemMenu, this);
    scene->setSceneRect(QRectF(0, 0, 5000, 5000));

    // in order to manipulate the buttons in the tool box
    connect(scene, &DiagramScene::itemInserted,
            this, &Diagram::itemInserted);
    connect(scene, &DiagramScene::textInserted,
            this, &Diagram::textInserted);
    connect(scene, &DiagramScene::itemSelected,
            this, &Diagram::itemSelected);

    // connect the tool bars to its signals
    createToolbars();

    // create horizontal layout and add widgets to it
    layout = new QHBoxLayout;
    // layout->addWidget(toolBox);

    view = new QGraphicsView(scene);
    view->viewport()->installEventFilter(this);
    layout->addWidget(view);

    // set the white background
    scene->setBackgroundBrush(QPixmap(":/images/images/background4.png"));
    scene->update();
    view->update();

    // used to display table info or relationship info
    sideWidget = new QWidget;

    mainWidget = new QWidget;
    mainWidget->setLayout(layout);

    setCentralWidget(mainWidget);
    setUnifiedTitleAndToolBarOnMac(true);
}



void Diagram::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exitAction);

    itemMenu = menuBar()->addMenu(tr("&Item"));
    itemMenu->addAction(deleteAction);
    itemMenu->addSeparator();
    itemMenu->addAction(toFrontAction);
    itemMenu->addAction(sendBackAction);

    aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(aboutAction);
}



void Diagram::createActions()
{
    // create table
    createTableAction = new QAction(QIcon(":/images/images/table.png"), tr("&Create table"), this);
    createTableAction->setStatusTip(tr("Create an SQL table"));
    connect(createTableAction, &QAction::triggered, this, &Diagram::createTableButtonClicked);

    // create text
    createTextAction = new QAction(QIcon(":/images/images/textpointer.png"), tr("&Create text"), this);
    createTextAction->setStatusTip(tr("Create text"));
    connect(createTextAction, &QAction::triggered, this, &Diagram::createTextButtonClicked);

    // generate SQL code
    generateSqlAction = new QAction(QIcon(":/images/images/sql-server.png"), tr("&Generate SQL"), this);
    generateSqlAction->setStatusTip(tr("Generate SQL code"));
    connect(generateSqlAction, &QAction::triggered, this, &Diagram::generateSql);

    // bring item to front
    toFrontAction = new QAction(QIcon(":/images/images/bringtofront.png"),
                                tr("Bring to &Front"), this);
    toFrontAction->setShortcut(tr("Ctrl+F"));
    toFrontAction->setStatusTip(tr("Bring item to front"));
    connect(toFrontAction, &QAction::triggered, this, &Diagram::bringToFront);

    // send item to back
    sendBackAction = new QAction(QIcon(":/images/images/sendtoback.png"), tr("Send to &Back"), this);
    sendBackAction->setShortcut(tr("Ctrl+T"));
    sendBackAction->setStatusTip(tr("Send item to back"));
    connect(sendBackAction, &QAction::triggered, this, &Diagram::sendToBack);

    // delete item from diagram
    deleteAction = new QAction(QIcon(":/images/images/delete.png"), tr("&Delete"), this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(tr("Delete item from diagram"));
    connect(deleteAction, &QAction::triggered, this, &Diagram::deleteItem);    

    // quit the app
    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Quit App"));
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // set text to bold
    boldAction = new QAction(tr("Bold"), this);
    boldAction->setCheckable(true);
    QPixmap pixmap(":/images/images/bold.png"); // the action image
    boldAction->setIcon(QIcon(pixmap));
    boldAction->setShortcut(tr("Ctrl+B"));
    connect(boldAction, &QAction::triggered, this, &Diagram::handleFontChange);

    // set text to italic
    italicAction = new QAction(QIcon(":/images/images/italic.png"), tr("Italic"), this);
    italicAction->setCheckable(true);
    italicAction->setShortcut(tr("Ctrl+I"));
    connect(italicAction, &QAction::triggered, this, &Diagram::handleFontChange);

    // set text to underline
    underlineAction = new QAction(QIcon(":/images/images/underline.png"), tr("Underline"), this);
    underlineAction->setCheckable(true);
    underlineAction->setShortcut(tr("Ctrl+U"));
    connect(underlineAction, &QAction::triggered, this, &Diagram::handleFontChange);

    // aboutAction = new QAction(tr("A&bout"), this);
    // aboutAction->setShortcut(tr("F1"));
    // connect(aboutAction, &QAction::triggered, this, &Diagram::about);
}



void Diagram::createToolbars()
{
    // create table, text, and generate SQL
    createGenerateToolBar = addToolBar(tr("Create"));
    createGenerateToolBar->addAction(createTableAction);
    createGenerateToolBar->addAction(createTextAction);
    createGenerateToolBar->addAction(generateSqlAction);

    // bring item to front, back, and delete it
    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(deleteAction);
    editToolBar->addAction(toFrontAction);
    editToolBar->addAction(sendBackAction);


    // combo box with the fonts
    fontCombo = new QFontComboBox();
    // when font is changed
    connect(fontCombo, &QFontComboBox::currentFontChanged,
            this, &Diagram::currentFontChanged);


    // combo box with font sizes
    fontSizeCombo = new QComboBox;
    fontSizeCombo->setEditable(true);

    for (int i = 8; i < 30; i = i + 2)
        fontSizeCombo->addItem(QString().setNum(i));

    // the range of the font sizes from 2 to 64
    QIntValidator *validator = new QIntValidator(2, 64, this);
    fontSizeCombo->setValidator(validator);
    // when size is changed
    connect(fontSizeCombo, &QComboBox::currentTextChanged,
            this, &Diagram::fontSizeChanged);


    // changes the font color
    fontColorToolButton = new QToolButton;

    // set the color menu and show it when the button is clicked
    fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fontColorToolButton->setMenu(createColorMenu(&Diagram::textColorChanged, Qt::black));

    // assign the default color to text
    textAction = fontColorToolButton->menu()->defaultAction();

    fontColorToolButton->setIcon(createColorToolButtonIcon(":/images/images/textpointer.png", Qt::black));
    fontColorToolButton->setAutoFillBackground(true);
    connect(fontColorToolButton, &QAbstractButton::clicked,
            this, &Diagram::textButtonTriggered);


    // changes the color of a shape
    fillColorToolButton = new QToolButton;

    // set the color menu and show it when the button is clicked
    fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fillColorToolButton->setMenu(createColorMenu(&Diagram::itemColorChanged, Qt::white));

    // assign the default color to the shapes
    fillAction = fillColorToolButton->menu()->defaultAction();

    fillColorToolButton->setIcon(createColorToolButtonIcon(
        ":/images/images/floodfill.png", Qt::white));
    connect(fillColorToolButton, &QAbstractButton::clicked,
            this, &Diagram::fillButtonTriggered);


    // changes the color of the line that connects two items
    lineColorToolButton = new QToolButton;

    // set the color menu and show it when the button is clicked
    lineColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    lineColorToolButton->setMenu(createColorMenu(&Diagram::lineColorChanged, Qt::black));

    // assign the default color to line
    lineAction = lineColorToolButton->menu()->defaultAction();

    lineColorToolButton->setIcon(createColorToolButtonIcon(
        ":/images/images/linecolor.png", Qt::black));
    connect(lineColorToolButton, &QAbstractButton::clicked,
            this, &Diagram::lineButtonTriggered);


    // text actions
    textToolBar = addToolBar(tr("Font"));
    textToolBar->addWidget(fontCombo);
    textToolBar->addWidget(fontSizeCombo);
    textToolBar->addAction(boldAction);
    textToolBar->addAction(italicAction);
    textToolBar->addAction(underlineAction);

    // color actions
    colorToolBar = addToolBar(tr("Color"));
    colorToolBar->addWidget(fontColorToolButton);
    colorToolBar->addWidget(fillColorToolButton);
    colorToolBar->addWidget(lineColorToolButton);

    // to move an item
    QToolButton *pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon(":/images/images/pointer.png"));

    // to connect items with a line
    QToolButton *linePointerButton = new QToolButton;
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon(":/images/images/linepointer.png"));

    pointerTypeGroup = new QButtonGroup(this);
    pointerTypeGroup->addButton(pointerButton, int(DiagramScene::MoveItem));
    pointerTypeGroup->addButton(linePointerButton, int(DiagramScene::InsertLine));
    connect(pointerTypeGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            this, &Diagram::pointerGroupClicked);


    // combo box with available scales
    sceneScaleCombo = new QComboBox;

    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");

    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(2);
    connect(sceneScaleCombo, &QComboBox::currentTextChanged,
            this, &Diagram::sceneScaleChanged);

    pointerToolbar = addToolBar(tr("Pointer type"));
    pointerToolbar->addWidget(pointerButton);
    pointerToolbar->addWidget(linePointerButton);
    pointerToolbar->addWidget(sceneScaleCombo);
}



// this function creates a color menu that is used as the drop-down menu for the tool buttons in the colorToolBar
// a slot can be passed to this function
template<typename PointerToMemberFunction>
QMenu *Diagram::createColorMenu(const PointerToMemberFunction &slot, QColor defaultColor)
{
    // available colors
    QList<QColor> colors;
    colors << Qt::black << Qt::white << Qt::red << Qt::blue << Qt::yellow;
    QStringList names;
    names << tr("black") << tr("white") << tr("red") << tr("blue")
          << tr("yellow");

    QMenu *colorMenu = new QMenu(this);
    for (int i = 0; i < colors.count(); ++i) {
        QAction *action = new QAction(names.at(i), this);
        // set data that contains the corresponding color
        action->setData(colors.at(i));
        // rectangle filled with the corresponding color
        action->setIcon(createColorIcon(colors.at(i)));
        // connect to the provided slot
        connect(action, &QAction::triggered, this, slot);
        colorMenu->addAction(action);
        if (colors.at(i) == defaultColor)
            colorMenu->setDefaultAction(action);
    }
    return colorMenu;
}



// draws icon with image and rectangle underneath filled with corresponding color
QIcon Diagram::createColorToolButtonIcon(const QString &imageFile, QColor color)
{
    QPixmap pixmap(50, 80);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPixmap image(imageFile);
    // Draw icon centred horizontally on button.
    QRect target(4, 0, 42, 43);
    QRect source(0, 0, 42, 43);
    painter.fillRect(QRect(0, 60, 50, 80), color);
    painter.drawPixmap(target, image, source);

    return QIcon(pixmap);
}



// draws rectangle filled with corresponding color
// used for creating icons for the color menus
// fillColorToolButton, fontColorToolButton, and lineColorToolButton.
QIcon Diagram::createColorIcon(QColor color)
{
    QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);

    return QIcon(pixmap);
}



// changes the background of the scene
void Diagram::backgroundButtonGroupClicked(QAbstractButton *button)
{
    // uncheck all other buttons
    const QList<QAbstractButton *> buttons = backgroundButtonGroup->buttons();
    for (QAbstractButton *myButton : buttons) {
        if (myButton != button)
            button->setChecked(false);
    }

    QString text = button->text();
    if (text == tr("Blue Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/images/background1.png"));
    else if (text == tr("White Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/images/background2.png"));
    else if (text == tr("Gray Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/images/background3.png"));
    else
        scene->setBackgroundBrush(QPixmap(":/images/images/background4.png"));

    scene->update();
    view->update();
}



// sets the scene mode (InsertText or InsertItem)
void Diagram::buttonGroupClicked(QAbstractButton *button)
{
    // uncheck all other buttons
    const QList<QAbstractButton *> buttons = buttonGroup->buttons();
    for (QAbstractButton *myButton : buttons) {
        if (myButton != button)
            button->setChecked(false);
    }

    const int id = buttonGroup->id(button);
    if (id == InsertTextButton) {
        scene->setMode(DiagramScene::InsertText);
    } else {
        scene->setMode(DiagramScene::InsertItem);
    }
}



// sets the scene mode (ItemMove or InsertLine)
void Diagram::pointerGroupClicked()
{
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}



// deletes selected item from the scene
void Diagram::deleteItem()
{
    QList<QGraphicsItem *> selectedItems = scene->selectedItems();

    // delete the arrows first
    for (QGraphicsItem *item : std::as_const(selectedItems)) {
        if (item->type() == Arrow::Type) {
            scene->removeItem(item);
            Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);
            arrow->startItem()->removeArrow(arrow);
            arrow->endItem()->removeArrow(arrow);
            delete item;
        }
    }

    selectedItems = scene->selectedItems();
    for (QGraphicsItem *item : std::as_const(selectedItems)) {
        if (item->type() == DiagramItem::Type)
            // delete the arrows connected to the item
            qgraphicsitem_cast<DiagramItem *>(item)->removeArrows();
        scene->removeItem(item);
        delete item;
    }

    scene->clearSelection(); // Deselect all items in the scene
    sideWidget->hide(); // Hide the tablePropertiesWidget
}



// generate the corresponding SQL code
// the function gets the tables and the relationships drawn on the diagram
// and emits the signal passing the data
void Diagram::generateSql() {

    QList<Table> tables;
    QList<Relationship> relationships;

    foreach (QGraphicsItem* item, scene->items()) {

        // get the tables
        if (DiagramItem *currentItem = qgraphicsitem_cast<DiagramItem *>(item)) {
            if (!currentItem->table.name.isEmpty() && currentItem->table.columns.size() > 0) {
                tables << currentItem->table;
            }
        }

        // get the ralationships
        Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);
        if (arrow) {
            for (int i = 0; i < arrow->columnRelationships.size(); ++i) {
                Relationship rel(arrow->startItem()->table.name, arrow->columnRelationships[i].startColumn->name, arrow->endItem()->table.name, arrow->columnRelationships[i].endColumn->name);
                relationships << rel;
            }
        }
    }


    emit generateSqlClicked(tables, relationships);
}



// set the insert table mode
void Diagram::createTableButtonClicked() {
    scene->setMode(DiagramScene::InsertItem);
}



// set the insert text mode
void Diagram::createTextButtonClicked() {
    scene->setMode(DiagramScene::InsertText);
}



void Diagram::drawDiagram(QList<Table> &tables, QList<Relationship> &relationships) {
    view->centerOn(scene->drawDiagram(tables, relationships).center());
}



// brings selected item to the front of the scene
void Diagram::bringToFront()
{
    // check if there are any selected items
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    const QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    // set zValue of the selected item
    qreal zValue = 0;
    for (const QGraphicsItem *item : overlapItems) {
        if (item->zValue() >= zValue && item->type() == DiagramItem::Type)
            zValue = item->zValue() + 0.1;
    }
    selectedItem->setZValue(zValue);
}



// sends selected item to the back of the scene
void Diagram::sendToBack()
{
    // check if there are any selected items
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    const QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    // set zValue of the selected item
    qreal zValue = 0;
    for (const QGraphicsItem *item : overlapItems) {
        if (item->zValue() <= zValue && item->type() == DiagramItem::Type)
            zValue = item->zValue() - 0.1;
    }
    selectedItem->setZValue(zValue);
}



// sets mode of the scene to MoveItem
void Diagram::itemInserted(DiagramItem *item)
{
    pointerTypeGroup->button(int(DiagramScene::MoveItem))->setChecked(true);
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
    // buttonGroup->button(int(item->diagramType()))->setChecked(false);
}



// sets the mode of scene to the one that was before
void Diagram::textInserted(QGraphicsTextItem *)
{
    // buttonGroup->button(InsertTextButton)->setChecked(false);
    std::cout << "Text item inserted" << std::endl;
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}



// changes the font according to the states of the widgets
void Diagram::currentFontChanged(const QFont &)
{
    handleFontChange();
}



// changes the font according to the states of the widgets
void Diagram::fontSizeChanged(const QString &)
{
    handleFontChange();
}



// changes the font according to the states of the widgets
void Diagram::handleFontChange()
{
    QFont font = fontCombo->currentFont();
    font.setPointSize(fontSizeCombo->currentText().toInt());
    font.setWeight(boldAction->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(italicAction->isChecked());
    font.setUnderline(underlineAction->isChecked());

    scene->setFont(font);
}



// changes the scale of the view
void Diagram::sceneScaleChanged(const QString &scale)
{
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
    QTransform oldMatrix = view->transform();
    view->resetTransform();
    view->translate(oldMatrix.dx(), oldMatrix.dy());
    view->scale(newScale, newScale);
}



// changes text color and the icon of the button to the corresponding color
void Diagram::textColorChanged()
{
    textAction = qobject_cast<QAction *>(sender());
    fontColorToolButton->setIcon(createColorToolButtonIcon(
        ":/images/images/textpointer.png",
        qvariant_cast<QColor>(textAction->data())));
    textButtonTriggered();
}



// sets the text color
void Diagram::textButtonTriggered()
{
    scene->setTextColor(qvariant_cast<QColor>(textAction->data()));
}



// changes item color and the icon of the button to the corresponding color
void Diagram::itemColorChanged()
{
    fillAction = qobject_cast<QAction *>(sender());
    fillColorToolButton->setIcon(createColorToolButtonIcon(
        ":/images/images/floodfill.png",
        qvariant_cast<QColor>(fillAction->data())));
    fillButtonTriggered();
}



// sets the item color
void Diagram::fillButtonTriggered()
{
    scene->setItemColor(qvariant_cast<QColor>(fillAction->data()));
}



// changes line color and the icon of the button to the corresponding color
void Diagram::lineColorChanged()
{
    lineAction = qobject_cast<QAction *>(sender());
    lineColorToolButton->setIcon(createColorToolButtonIcon(
        ":/images/images/linecolor.png",
        qvariant_cast<QColor>(lineAction->data())));
    lineButtonTriggered();
}



// sets the line color
void Diagram::lineButtonTriggered()
{
    scene->setLineColor(qvariant_cast<QColor>(lineAction->data()));
}



// changes the UI if another item is selected
void Diagram::itemSelected(QGraphicsItem *item)
{
    // get the selected text item
    DiagramTextItem *textItem =
        qgraphicsitem_cast<DiagramTextItem *>(item);

    // get the selected item (table)
    DiagramItem *selectedItem =
        qgraphicsitem_cast<DiagramItem *>(item);

    // get the selected arrow (ralationship)
    Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);

    // change the text buttons if a text item is selected
    if (textItem) {
        QFont font = textItem->font();
        fontCombo->setCurrentFont(font);
        fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
        boldAction->setChecked(font.weight() == QFont::Bold);
        italicAction->setChecked(font.italic());
        underlineAction->setChecked(font.underline());
    }

    // show the table properties widget if a table item is selected
    if (selectedItem) {

        // delete the existing layout
        if (sideWidget->layout()) {
            deleteLayout(sideWidget->layout());
            sideWidget->hide();
        }

        // the main layout of the side widget
        QVBoxLayout *verticalMainLayout = new QVBoxLayout;
        verticalMainLayout->setAlignment(Qt::AlignTop);

        // Table name, add column button, and close the side widget button
        QVBoxLayout *verticalTableInfoLayout = new QVBoxLayout;
        verticalTableInfoLayout->setAlignment(Qt::AlignTop);

        QLabel *tableNameLabel = new QLabel("Table name:");

        QLineEdit *lineEdit = new QLineEdit;
        lineEdit->setText(selectedItem->table.name);
        lineEdit->setFixedWidth(100);

        connect(lineEdit, &QLineEdit::textChanged, selectedItem, &DiagramItem::updateName);

        // Table name: (Label)   New name (LineEdit)
        QHBoxLayout *tableNameLayout = new QHBoxLayout;
        tableNameLayout->addWidget(tableNameLabel);
        tableNameLayout->addWidget(lineEdit);
        verticalTableInfoLayout->addLayout(tableNameLayout);

        // Create the close button
        QPushButton *closeButton = new QPushButton("Close");

        // When the close button is clicked, hide the side widget
        connect(closeButton, &QPushButton::clicked, [this, selectedItem]() {
            scene->clearSelection(); // Deselect all items in the scene
            sideWidget->hide(); // Hide the tablePropertiesWidget
        });

        // Close (Button)
        QHBoxLayout *closeButtonLayout = new QHBoxLayout;
        closeButtonLayout->addWidget(closeButton);
        verticalTableInfoLayout->addLayout(closeButtonLayout);

        // Create the "Add Column" button
        QPushButton *addItemButton = new QPushButton("Add Column");

        // Add column (button)
        QHBoxLayout *addItemButtonLayout = new QHBoxLayout;
        addItemButtonLayout->addWidget(addItemButton);
        verticalTableInfoLayout->addLayout(addItemButtonLayout);
        verticalMainLayout->addLayout(verticalTableInfoLayout);

        // Column name 1, column data type 1, primary button 1
        // Column name 2, column data type 2, primary button 2
        // ...
        QVBoxLayout *verticalColumnsLayout = new QVBoxLayout;
        verticalColumnsLayout->setAlignment(Qt::AlignTop);
        createColumnsWidgets(verticalColumnsLayout, selectedItem);

        // Scroll the columns
        QScrollArea *scrollArea = new QScrollArea;
        scrollArea->setWidgetResizable(true);
        QWidget *scrollAreaContent = new QWidget;
        scrollAreaContent->setLayout(verticalColumnsLayout);
        scrollAreaContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        scrollArea->setWidget(scrollAreaContent);
        verticalMainLayout->addWidget(scrollArea);
        std::cout << "Added to the vertical layout" << std::endl;

        // When the add column button is clicked, draw the interface to add a column
        connect(addItemButton, &QPushButton::clicked, [this, verticalColumnsLayout, selectedItem]() {

            // Type the column name
            QLineEdit *nameLineEdit = new QLineEdit;
            nameLineEdit->setPlaceholderText("Item name");
            nameLineEdit->setFixedWidth(100);

            // Choose the data types
            QComboBox *dataTypeComboBox = new QComboBox;
            // Add SQL data types to the combo box
            dataTypeComboBox->addItem("INT");
            dataTypeComboBox->addItem("VARCHAR");
            dataTypeComboBox->addItem("DATE");
            // Add more data types as needed

            // Item name (line edit)   data type (combo box)
            QHBoxLayout *itemLayout = new QHBoxLayout;
            itemLayout->addWidget(nameLineEdit);
            itemLayout->addWidget(dataTypeComboBox);
            verticalColumnsLayout->addLayout(itemLayout);

            // Create the "Add" button
            QPushButton *addButton = new QPushButton("Add");
            QHBoxLayout *addButtonLayout = new QHBoxLayout;
            addButtonLayout->addWidget(addButton);
            verticalColumnsLayout->addLayout(addButtonLayout);

            // When the add button is clicked, add the column to the table
            connect(addButton, &QPushButton::clicked, [this, nameLineEdit, dataTypeComboBox, selectedItem, addButton, verticalColumnsLayout]() {

                // Retrieve the entered item name and data type
                QString itemName = nameLineEdit->text();
                QString dataType = dataTypeComboBox->currentText();

                // check if there is a column name
                if (!itemName.isEmpty()) {
                    // add the column to the table
                    selectedItem->addItem(itemName, dataType);

                    // update the widget
                    deleteLayout(verticalColumnsLayout);
                    createColumnsWidgets(verticalColumnsLayout, selectedItem);
                }
            });
        });

        verticalMainLayout->addLayout(verticalColumnsLayout);

        // add layout to the side widget
        sideWidget = new QWidget;
        // tablePropertiesWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sideWidget->setLayout(verticalMainLayout);
        sideWidget->setFixedWidth(300);
        sideWidget->setVisible(true);
        layout->addWidget(sideWidget);
    } else {
        // if another item is selected, hide the widget or show a new one
        sideWidget->hide();
    }



    // if the selected item is an arrow (relationship)
    if (arrow) {
        std::cout << "Arrow selected" << std::endl;

        // delete the existing layout
        if (sideWidget->layout()) {
            deleteLayout(sideWidget->layout());
            sideWidget->hide();
        }

        // main widget layout
        // From: (label)   Combo box with data types (ComboBox)
        // To: (label)     Combo box with data types (ComboBox)

        QVBoxLayout *verticalMainLayout = new QVBoxLayout;
        verticalMainLayout->setAlignment(Qt::AlignTop);

        // Create the close button
        QPushButton *closeButton = new QPushButton("Close");

        // When the close button is clicked, hide the side widget
        connect(closeButton, &QPushButton::clicked, [this, selectedItem]() {
            scene->clearSelection(); // Deselect all items in the scene
            sideWidget->hide(); // Hide the tablePropertiesWidget
        });

        QPushButton *addRelationshipButton = new QPushButton("Add Relationship");

        verticalMainLayout->addWidget(closeButton);
        verticalMainLayout->addWidget(addRelationshipButton);


        // if there are columns in start and end items and no relationships yet, then create a new relationship
        if (!arrow->startItem()->table.columns.isEmpty() && !arrow->endItem()->table.columns.isEmpty() && arrow->columnRelationships.size() == 0) {
            // arrow->setEndColumn(arrow->endItem()->table->columns[0]);
            arrow->columnRelationships << ColumnRelationship(&arrow->startItem()->table.columns[0], &arrow->endItem()->table.columns[0]);
        }

        QVBoxLayout *verticalRelationshipsLayout = new QVBoxLayout;
        verticalRelationshipsLayout->setAlignment(Qt::AlignTop);
        createRelationshipsWidgets(verticalRelationshipsLayout, arrow);

        // verticalMainLayout->addLayout(verticalRelationshipsLayout);

        // Scroll the relationships
        QScrollArea *scrollArea = new QScrollArea;
        scrollArea->setWidgetResizable(true);
        QWidget *scrollAreaContent = new QWidget;
        scrollAreaContent->setLayout(verticalRelationshipsLayout);
        scrollAreaContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        scrollArea->setWidget(scrollAreaContent);
        verticalMainLayout->addWidget(scrollArea);

        connect(addRelationshipButton, &QPushButton::clicked, [this, verticalRelationshipsLayout, arrow]() {
            if (!arrow->startItem()->table.columns.isEmpty() && !arrow->endItem()->table.columns.isEmpty()) {
                // arrow->setEndColumn(arrow->endItem()->table->columns[0]);
                arrow->columnRelationships << ColumnRelationship(&arrow->startItem()->table.columns[0], &arrow->endItem()->table.columns[0]);
                std::cout << arrow->columnRelationships.size() << std::endl;
            }

            // update the widget
            deleteLayout(verticalRelationshipsLayout);
            createRelationshipsWidgets(verticalRelationshipsLayout, arrow);
        });


        std::cout << "Arrow selected 2" << std::endl;


        // add layout to the side widget
        sideWidget = new QWidget;
        sideWidget->setLayout(verticalMainLayout);
        sideWidget->setFixedWidth(250);
        sideWidget->setVisible(true);
        layout->addWidget(sideWidget);

        std::cout << "Arrow selected 3" << std::endl;
    }
}



// create the layout with column names and data types
void Diagram::createColumnsWidgets(QVBoxLayout *verticalColumnsLayout, DiagramItem *item) {

    // loop through the columns in the table
    for (int i = 0; i < item->table.columns.size(); ++i) {
        QVBoxLayout *itemLayout = new QVBoxLayout;

        // line edit with the column name
        QLineEdit *nameLineEdit = new QLineEdit;
        nameLineEdit->setPlaceholderText("Item name");
        nameLineEdit->setFixedWidth(100);
        // set the current column name
        nameLineEdit->setText(item->table.columns[i].name);
        // when the text is editted, re-draw the text and update it in the table
        connect(nameLineEdit, &QLineEdit::textChanged, this, [this, item, i](const QString &text) {
            Column newColumn(text, item->table.columns[i].type, item->table.columns[i].isPrimary);
            item->updateColumn(i, newColumn);
        });

        // combo box with data types
        QComboBox *dataTypeComboBox = new QComboBox;
        dataTypeComboBox->addItem("INT");
        dataTypeComboBox->addItem("VARCHAR");
        dataTypeComboBox->addItem("DATE");
        // Add more data types as needed

        // set the current column data type
        dataTypeComboBox->setCurrentText(item->table.columns[i].type);
        // when the data type is changed, re-draw the columns on the diagram and update it in the table
        connect(dataTypeComboBox, &QComboBox::currentTextChanged, [=](const QString &text) {
            QString newDataType = text;
            Column newColumn(item->table.columns[i].name, newDataType, item->table.columns[i].isPrimary);
            item->updateColumn(i, newColumn);
        });

        // sets a primary column
        QRadioButton *primaryKeyRadioButton = new QRadioButton("Primary Key");

        // When clicked, set the column to primary
        connect(primaryKeyRadioButton, &QRadioButton::clicked, [=]() {
            Column newColumn(item->table.columns[i].name, item->table.columns[i].type, true);
            item->updateColumn(i, newColumn);
        });

        // Set the current primary key radio button based on the column's primary key status
        primaryKeyRadioButton->setChecked(item->table.columns[i].isPrimary);

        QHBoxLayout *itemInfoLayout = new QHBoxLayout;
        itemInfoLayout->addWidget(nameLineEdit);
        itemInfoLayout->addWidget(dataTypeComboBox);
        itemInfoLayout->addWidget(primaryKeyRadioButton);
        itemLayout->addLayout(itemInfoLayout);

        QPushButton *deleteButton = new QPushButton("Delete");
        QHBoxLayout *deleteButtonLayout = new QHBoxLayout;
        deleteButtonLayout->addWidget(deleteButton);
        std::cout << "Added the delete button" << std::endl;
        connect(deleteButton, &QPushButton::clicked, [deleteButton, this, item, i, verticalColumnsLayout]() {
            item->removeColumn(i);
            deleteLayout(verticalColumnsLayout);
            createColumnsWidgets(verticalColumnsLayout, item);
        });

        itemLayout->addLayout(deleteButtonLayout);
        verticalColumnsLayout->addLayout(itemLayout);
    }
}



void Diagram::createRelationshipsWidgets(QVBoxLayout *verticalLayout, Arrow *arrow) {
    std::cout << arrow->columnRelationships.size() << std::endl;

    for (int i = 0; i < arrow->columnRelationships.size(); ++i) {
        QVBoxLayout *relationshipLayout = new QVBoxLayout;


        QHBoxLayout *fromLayout = new QHBoxLayout;

        QLabel *fromLabel = new QLabel("From: ");
        QComboBox* fromComboBox = new QComboBox();
        for (int j = 0; j < arrow->startItem()->table.columns.size(); j++) {
            fromComboBox->addItem(arrow->startItem()->table.columns[j].name + " (" + arrow->startItem()->table.columns[j].type + ")");
        }
        fromComboBox->setCurrentText(arrow->columnRelationships[i].startColumn->name + " (" + arrow->columnRelationships[i].startColumn->type + ")");

        connect(fromComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, arrow, i](int index) {
            // arrow->setStartColumn(arrow->startItem()->table->columns[index]);
            arrow->columnRelationships[i].startColumn = &arrow->startItem()->table.columns[index];
        });
        arrow->columnRelationships[i].startColumn = &arrow->startItem()->table.columns[fromComboBox->currentIndex()];
        std::cout << "From: " << arrow->columnRelationships[i].startColumn->name.toStdString() << std::endl;

        fromLayout->addWidget(fromLabel);
        fromLayout->addWidget(fromComboBox);
        relationshipLayout->addLayout(fromLayout);


        QHBoxLayout *toLayout = new QHBoxLayout;

        QLabel *toLabel = new QLabel("To: ");
        QComboBox* toComboBox = new QComboBox();
        for (int j = 0; j < arrow->endItem()->table.columns.size(); j++) {
            toComboBox->addItem(arrow->endItem()->table.columns[j].name + " (" + arrow->endItem()->table.columns[j].type + ")");
        }
        toComboBox->setCurrentText(arrow->columnRelationships[i].endColumn->name + " (" + arrow->columnRelationships[i].endColumn->type + ")");

        connect(toComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, arrow, i](int index) {
            // arrow->setEndColumn(arrow->endItem()->table->columns[index]);
            arrow->columnRelationships[i].endColumn = &arrow->endItem()->table.columns[index];
        });
        arrow->columnRelationships[i].endColumn = &arrow->endItem()->table.columns[toComboBox->currentIndex()];
        std::cout << "To: " << arrow->columnRelationships[i].endColumn->name.toStdString() << std::endl;

        toLayout->addWidget(toLabel);
        toLayout->addWidget(toComboBox);
        relationshipLayout->addLayout(toLayout);


        QPushButton *deleteButton = new QPushButton("Delete");
        connect(deleteButton, &QPushButton::clicked, [deleteButton, this, arrow, i, verticalLayout]() {
            arrow->columnRelationships.removeAt(i);
            deleteLayout(verticalLayout);
            createRelationshipsWidgets(verticalLayout, arrow);
        });

        relationshipLayout->addWidget(deleteButton);
        verticalLayout->addLayout(relationshipLayout);
    }
}



void Diagram::deleteLayout(QLayout *layout) {
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (QLayout *childLayout = child->layout()) {
            // Recursively delete child layouts
            deleteLayout(childLayout);
        } else if (QWidget *widget = child->widget()) {
            // Remove and delete widgets
            layout->removeWidget(widget);
            delete widget;
        }
        // Delete the layout item
        delete child;
    }
}


