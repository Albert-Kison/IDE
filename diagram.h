#ifndef DIAGRAM_H
#define DIAGRAM_H

// This class provides the diagram scene with the tool bars and user interface
// to allow the user to draw shapes, change their color, font, style,
// and create relationships

// manages interaction between the widgets
// and the graphics scene, view, and items

#include "diagramitem.h"
#include "Table.h"
#include "Relationship.h"

#include <QMainWindow>

class DiagramScene;

QT_BEGIN_NAMESPACE
class QAction;
class QToolBox;
class QSpinBox;
class QComboBox;
class QFontComboBox;
class QButtonGroup;
class QLineEdit;
class QGraphicsTextItem;
class QFont;
class QToolButton;
class QAbstractButton;
class QGraphicsView;
class QHBoxLayout;
class QVBoxLayout;
class QLineEdit;
QT_END_NAMESPACE


class Diagram : public QMainWindow
{
    Q_OBJECT
public:
    Diagram();

private slots:
    void backgroundButtonGroupClicked(QAbstractButton *button);
    void buttonGroupClicked(QAbstractButton *button);
    void pointerGroupClicked();

    void deleteItem();
    void bringToFront();
    void sendToBack();
    void generateSql();

    void itemInserted(DiagramItem *item);
    // void itemClicked(DiagramItem *item);
    void textInserted(QGraphicsTextItem *item);
    void currentFontChanged(const QFont &font);
    void fontSizeChanged(const QString &size);
    void sceneScaleChanged(const QString &scale);
    void textColorChanged();
    void itemColorChanged();
    void lineColorChanged();
    void textButtonTriggered();
    void fillButtonTriggered();
    void lineButtonTriggered();
    void handleFontChange();
    void itemSelected(QGraphicsItem *item);
    // void about();

signals:
    void generateSqlClicked(QList<Table> &tables);

private:
    void createToolBox();
    void createActions();
    void createMenus();
    void createToolbars();
    void createColumnsWidgets(QVBoxLayout *verticalLayout, DiagramItem *item);
    void deleteLayout(QLayout *layout);

    bool eventFilter(QObject *obj, QEvent *event) override;

    QWidget *createBackgroundCellWidget(const QString &text,
                                        const QString &image);
    QWidget *createCellWidget(const QString &text,
                              DiagramItem::DiagramType type);

    template<typename PointerToMemberFunction>
    QMenu *createColorMenu(const PointerToMemberFunction &slot, QColor defaultColor);
    QIcon createColorToolButtonIcon(const QString &image, QColor color);
    QIcon createColorIcon(QColor color);

    DiagramScene *scene;
    QGraphicsView *view;
    QHBoxLayout *layout;
    // QWidget *tablePropertiesWidget;
    QWidget *sideWidget;
    QWidget *mainWidget;

    QAction *exitAction;
    QAction *addAction;
    QAction *deleteAction;
    QAction *generateSqlAction;

    QAction *toFrontAction;
    QAction *sendBackAction;
    QAction *aboutAction;

    QMenu *fileMenu;
    QMenu *itemMenu;
    QMenu *aboutMenu;

    QToolBar *textToolBar;
    QToolBar *editToolBar;
    QToolBar *colorToolBar;
    QToolBar *pointerToolbar;

    QComboBox *sceneScaleCombo;
    QComboBox *itemColorCombo;
    QComboBox *textColorCombo;
    QComboBox *fontSizeCombo;
    QFontComboBox *fontCombo;

    QToolBox *toolBox;
    QButtonGroup *buttonGroup;
    QButtonGroup *pointerTypeGroup;
    QButtonGroup *backgroundButtonGroup;
    QToolButton *fontColorToolButton;
    QToolButton *fillColorToolButton;
    QToolButton *lineColorToolButton;
    QAction *boldAction;
    QAction *underlineAction;
    QAction *italicAction;
    QAction *textAction;
    QAction *fillAction;
    QAction *lineAction;
};

#endif // DIAGRAM_H
