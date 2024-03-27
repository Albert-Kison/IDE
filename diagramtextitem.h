#ifndef DIAGRAMTEXTITEM_H
#define DIAGRAMTEXTITEM_H

// This class adds the possibility to move and interact with editable text items

#include <QGraphicsTextItem>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE


class DiagramTextItem : public QGraphicsTextItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 3 };

    DiagramTextItem(QGraphicsItem *parent = nullptr);

    int type() const override { return Type; }

signals:
    void lostFocus(DiagramTextItem *item);
    void selectedChange(QGraphicsItem *item);

protected:
    // used to notify the DiagramScene when the text item loses focus and gets selected
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void focusOutEvent(QFocusEvent *event) override;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // DIAGRAMTEXTITEM_H
