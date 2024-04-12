#include "arrow.h"
#include "diagramitem.h"
#include "diagramscene.h"
#include "diagramtextitem.h"
#include "diagram.h"

#include <QtWidgets>
#include <iostream>



// std::ostream&  operator <<(std::ostream &stream,const QString &str)
// {
//     stream << str.toLatin1().constData(); //or: stream << str.toStdString(); //??
//     return stream;
// }

const int InsertTextButton = 10;


Diagram::Diagram()
{
    // create the widgets, layouts, and the scene
    createActions();
    createToolBox();
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
    layout->addWidget(toolBox);

    view = new QGraphicsView(scene);
    view->viewport()->installEventFilter(this);
    layout->addWidget(view);

    tablePropertiesWidget = new QWidget;

    mainWidget = new QWidget;
    mainWidget->setLayout(layout);

    setCentralWidget(mainWidget);
    setUnifiedTitleAndToolBarOnMac(true);
}



void Diagram::createToolBox()
{
    // This button group contains the flowchart shapes
    buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(false);   // allow all buttons to be unchecked

    // when one of the items is clicked
    connect(buttonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            this, &Diagram::buttonGroupClicked);

    // create the layout with the flowchart shapes
    QGridLayout *layout = new QGridLayout;

    // createCellWidget() creates a widget with a diagram item image
    // and assigns the diagram type to it
    layout->addWidget(createCellWidget(tr("Conditional"), DiagramItem::Conditional), 0, 0);
    layout->addWidget(createCellWidget(tr("Process"), DiagramItem::Step),0, 1);
    layout->addWidget(createCellWidget(tr("Input/Output"), DiagramItem::Io), 1, 0);
    layout->addWidget(createCellWidget(tr("Table"), DiagramItem::Table), 1, 1);


    // add text button
    // almost the same code in createCellWidget()
    QToolButton *textButton = new QToolButton;
    textButton->setCheckable(true);
    buttonGroup->addButton(textButton, InsertTextButton);
    textButton->setIcon(QIcon(QPixmap(":/images/images/textpointer.png")));
    textButton->setIconSize(QSize(50, 50));

    QGridLayout *textLayout = new QGridLayout;
    textLayout->addWidget(textButton, 0, 0, Qt::AlignHCenter);
    textLayout->addWidget(new QLabel(tr("Text")), 1, 0, Qt::AlignCenter);

    QWidget *textWidget = new QWidget;
    textWidget->setLayout(textLayout);
    layout->addWidget(textWidget, 2, 0);

    layout->setRowStretch(3, 10);
    layout->setColumnStretch(2, 10);

    // the widget with the flowchart shapes buttons and the text button
    // will be added to the tool box later
    QWidget *itemWidget = new QWidget;
    itemWidget->setLayout(layout);


    // This button group contains the backgrounds for the graphics view
    backgroundButtonGroup = new QButtonGroup(this);
    // when one of the backgrounds is clicked
    connect(backgroundButtonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            this, &Diagram::backgroundButtonGroupClicked);

    // create the layout with the backgrounds
    QGridLayout *backgroundLayout = new QGridLayout;

    // createBackgroundCellWidget() creates a widget with a background image
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("Blue Grid"),
                                                           ":/images/images/background1.png"), 0, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("White Grid"),
                                                           ":/images/images/background2.png"), 0, 1);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("Gray Grid"),
                                                           ":/images/images/background3.png"), 1, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("No Grid"),
                                                           ":/images/images/background4.png"), 1, 1);

    backgroundLayout->setRowStretch(2, 10);
    backgroundLayout->setColumnStretch(2, 10);

    // the widget with the backgrounds buttons
    // will be added to the tool box later
    QWidget *backgroundWidget = new QWidget;
    backgroundWidget->setLayout(backgroundLayout);


    // create the tool box with the flowchart shapes and the backgrounds buttons
    toolBox = new QToolBox;
    toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    toolBox->setMinimumWidth(itemWidget->sizeHint().width());
    toolBox->addItem(itemWidget, tr("Basic Flowchart Shapes"));
    toolBox->addItem(backgroundWidget, tr("Backgrounds"));
}



// creates a widget with a diagram item image and assigns the diagram type to it
QWidget *Diagram::createCellWidget(const QString &text, DiagramItem::DiagramType type)
{

    // get the icon of the item
    DiagramItem item(type, itemMenu);
    QIcon icon(item.image());

    // set the image and add the button to the button group with the flowchart shapes
    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    buttonGroup->addButton(button, int(type));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}



// creates a widget with a background image
QWidget *Diagram::createBackgroundCellWidget(const QString &text, const QString &image)
{
    // set the image and add the button to the backgrounds button group
    QToolButton *button = new QToolButton;
    button->setText(text);
    button->setIcon(QIcon(image));
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    backgroundButtonGroup->addButton(button);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
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
        scene->setItemType(DiagramItem::DiagramType(id));
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
    buttonGroup->button(int(item->diagramType()))->setChecked(false);
}



// sets the mode of scene to the one that was before
void Diagram::textInserted(QGraphicsTextItem *)
{
    buttonGroup->button(InsertTextButton)->setChecked(false);
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



// handles view events:
// scale the view when mouse is scrolled and ControlModifier is pressed
bool Diagram::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == view->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
        if (wheelEvent->modifiers() & Qt::ControlModifier) { // Check if Control key is pressed
            QPointF scenePos = view->mapToScene(wheelEvent->position().toPoint());
            if (view->sceneRect().contains(scenePos)) {
                // Scale the view
                qreal scaleFactor = 1.5;
                QPoint delta = wheelEvent->angleDelta() / 8; // Get scroll distance
                if (!delta.isNull()) {
                    qreal factor = qPow(scaleFactor, delta.y() / 120.0);
                    view->scale(factor, factor);
                    return true;
                }
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
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
    DiagramTextItem *textItem =
        qgraphicsitem_cast<DiagramTextItem *>(item);

    DiagramItem *selectedItem =
        qgraphicsitem_cast<DiagramItem *>(item);

    // Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);

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
    if (selectedItem && selectedItem->myDiagramType == DiagramItem::Table) {

        // delete the existing layout
        if (tablePropertiesWidget->layout()) {
            QLayoutItem *child;
            while ((child = tablePropertiesWidget->layout()->takeAt(0)) != nullptr) {
                delete child;
            }
            delete tablePropertiesWidget->layout();
            tablePropertiesWidget->hide();
        }

        QVBoxLayout *verticalMainLayout = new QVBoxLayout;
        verticalMainLayout->setAlignment(Qt::AlignTop);
        // selectedItem->updateText("New text");
        // std::cout << selectedItem->textItem->toPlainText() << std::endl;

        QVBoxLayout *verticalTableInfoLayout = new QVBoxLayout;
        verticalTableInfoLayout->setAlignment(Qt::AlignTop);

        QLabel *tableNameLabel = new QLabel("Table name:");
        // gridLayout->addWidget(tableNameLabel, 0, 0);

        QLineEdit *lineEdit = new QLineEdit;

        if (selectedItem->textItem) {
            lineEdit->setText(selectedItem->textItem->toPlainText());
        }
        lineEdit->setFixedWidth(100);

        // disconnect(lineEdit, &QLineEdit::textChanged, nullptr, nullptr);
        // disconnect(selectedItem->textItem->document(), &QTextDocument::contentsChanged, nullptr, nullptr);

        connect(lineEdit, &QLineEdit::textChanged, selectedItem, &DiagramItem::updateText);

        QHBoxLayout *tableNameLayout = new QHBoxLayout;
        tableNameLayout->addWidget(tableNameLabel);
        tableNameLayout->addWidget(lineEdit);
        verticalTableInfoLayout->addLayout(tableNameLayout);

        // Create the close button
        QPushButton *closeButton = new QPushButton("Close");
        // gridLayout->addWidget(closeButton, 1, 0, 1, 2); // Add the button to the layout

        // Connect the close button's clicked signal to a slot that hides the tablePropertiesWidget
        connect(closeButton, &QPushButton::clicked, [this, selectedItem]() {
            scene->clearSelection(); // Deselect all items in the scene
            tablePropertiesWidget->hide(); // Hide the tablePropertiesWidget
        });

        QHBoxLayout *closeButtonLayout = new QHBoxLayout;
        closeButtonLayout->addWidget(closeButton);
        verticalTableInfoLayout->addLayout(closeButtonLayout);

        // Create the "Add Item" button
        QPushButton *addItemButton = new QPushButton("Add Item");
        // gridLayout->addWidget(addItemButton, 2, 0, 1, 2);

        QHBoxLayout *addItemButtonLayout = new QHBoxLayout;
        addItemButtonLayout->addWidget(addItemButton);
        verticalTableInfoLayout->addLayout(addItemButtonLayout);
        verticalMainLayout->addLayout(verticalTableInfoLayout);

        QVBoxLayout *verticalColumnsLayout = new QVBoxLayout;
        verticalColumnsLayout->setAlignment(Qt::AlignTop);
        createColumnsWidgets(verticalColumnsLayout, selectedItem);
        std::cout << "Out of the method" << std::endl;
        // verticalLayout->addLayout(verticalColumnsLayout);
        // Create a scroll area and set the vertical layout as its widget
        // Create a scroll area and set the vertical layout as its widget
        QScrollArea *scrollArea = new QScrollArea;
        scrollArea->setWidgetResizable(true);
        QWidget *scrollAreaContent = new QWidget;
        scrollAreaContent->setLayout(verticalColumnsLayout);

        // Set the size policy of the contained widget to fixed horizontally
        scrollAreaContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

        scrollArea->setWidget(scrollAreaContent);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn); // Show vertical scrollbar
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Hide horizontal scrollbar

        verticalMainLayout->addWidget(scrollArea);
        std::cout << "Added to the vertical layout" << std::endl;

        // Connect the add item button's clicked signal to a slot that adds the item
        connect(addItemButton, &QPushButton::clicked, [this, verticalColumnsLayout, selectedItem]() {
            // QLabel *itemNameLabel = new QLabel("Item name:");
            QLineEdit *nameLineEdit = new QLineEdit;
            nameLineEdit->setPlaceholderText("Item name");
            nameLineEdit->setFixedWidth(100);

            // QLabel *dataTypeLabel = new QLabel("Data type:");
            QComboBox *dataTypeComboBox = new QComboBox;
            // Add SQL data types to the combo box
            dataTypeComboBox->addItem("INT");
            dataTypeComboBox->addItem("VARCHAR");
            dataTypeComboBox->addItem("DATE");

            // Add more data types as needed

            QHBoxLayout *itemLayout = new QHBoxLayout;
            itemLayout->addWidget(nameLineEdit);
            itemLayout->addWidget(dataTypeComboBox);
            std::cout << "This should not be null" << std::endl;
            verticalColumnsLayout->addLayout(itemLayout);

            // Create the "Add" button
            QPushButton *addButton = new QPushButton("Add");
            QHBoxLayout *addButtonLayout = new QHBoxLayout;
            addButtonLayout->addWidget(addButton);
            verticalColumnsLayout->addLayout(addButtonLayout);

            // Connect the add button's clicked signal to a slot that adds the item
            connect(addButton, &QPushButton::clicked, [this, nameLineEdit, dataTypeComboBox, selectedItem, addButton, verticalColumnsLayout]() {
                // Retrieve the entered item name and data type
                QString itemName = nameLineEdit->text();
                QString dataType = dataTypeComboBox->currentText();
                // Do something with the item name and data type, such as adding the item to the table
                // You can emit a signal here to handle the item addition in the scene
                // Example: emit addItemSignal(itemName, dataType);
                if (!itemName.isEmpty()) {
                    int index = selectedItem->addItem(itemName, dataType);
                    deleteLayout(verticalColumnsLayout);
                    createColumnsWidgets(verticalColumnsLayout, selectedItem);
                }
            });
        });

        verticalMainLayout->addLayout(verticalColumnsLayout);

        tablePropertiesWidget = new QWidget;
        // tablePropertiesWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        tablePropertiesWidget->setLayout(verticalMainLayout);
        tablePropertiesWidget->setVisible(true);
        layout->addWidget(tablePropertiesWidget);
    } else {
        tablePropertiesWidget->hide();
    }



    // if (arrow) {
    //     std::cout << "Arrow selected" << std::endl;
    // }
}



void Diagram::createColumnsWidgets(QVBoxLayout *verticalColumnsLayout, DiagramItem *item) {
    // Re-draw the widgets
    for (int i = 0; i < item->columns.size(); ++i) {
        QHBoxLayout *itemLayout = new QHBoxLayout;

        QLineEdit *nameLineEdit = new QLineEdit;
        nameLineEdit->setPlaceholderText("Item name");
        nameLineEdit->setFixedWidth(100);
        nameLineEdit->setText(item->columns[i].name);
        connect(nameLineEdit, &QLineEdit::textChanged, this, [this, item, i](const QString &text) {
            item->updateItem(i, text, item->columns[i].dataType);
        });

        QComboBox *dataTypeComboBox = new QComboBox;
        dataTypeComboBox->addItem("INT");
        dataTypeComboBox->addItem("VARCHAR");
        dataTypeComboBox->addItem("DATE");
        // Add more data types as needed
        dataTypeComboBox->setCurrentText(item->columns[i].dataType);
        connect(dataTypeComboBox, &QComboBox::currentTextChanged, [=](const QString &text) {
            QString newDataType = text;
            item->updateItem(i, item->columns[i].name, newDataType);
        });

        QPushButton *deleteButton = new QPushButton("Delete");
        QHBoxLayout *deleteButtonLayout = new QHBoxLayout;
        deleteButtonLayout->addWidget(deleteButton);
        std::cout << "Added the delete button" << std::endl;
        connect(deleteButton, &QPushButton::clicked, [deleteButton, this, item, i, verticalColumnsLayout]() {
            item->removeColumn(i);
            deleteLayout(verticalColumnsLayout);
            createColumnsWidgets(verticalColumnsLayout, item);
        });

        itemLayout->addWidget(nameLineEdit);
        itemLayout->addWidget(dataTypeComboBox);
        itemLayout->addLayout(deleteButtonLayout);
        verticalColumnsLayout->addLayout(itemLayout);
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


