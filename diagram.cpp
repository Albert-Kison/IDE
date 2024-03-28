#include "arrow.h"
#include "diagramitem.h"
#include "diagramscene.h"
#include "diagramtextitem.h"
#include "diagram.h"

#include <QtWidgets>

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
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(toolBox);

    view = new QGraphicsView(scene);
    view->viewport()->installEventFilter(this);
    layout->addWidget(view);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    setCentralWidget(widget);
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



// changes the state of the text buttons
// only text items emit this signal
void Diagram::itemSelected(QGraphicsItem *item)
{
    DiagramTextItem *textItem =
        qgraphicsitem_cast<DiagramTextItem *>(item);

    QFont font = textItem->font();
    fontCombo->setCurrentFont(font);
    fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
    boldAction->setChecked(font.weight() == QFont::Bold);
    italicAction->setChecked(font.italic());
    underlineAction->setChecked(font.underline());
}
