#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include "QtWidgets/qplaintextedit.h"
#include "highlighter.h"

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = nullptr);

    // paint the line number area
    void lineNumberAreaPaintEvent(QPaintEvent *event);

    // calculate the width of the line number area widget
    int lineNumberAreaWidth();

protected:
    // whenever the size of the editor changes, resize the line number area
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    // whenever the text is scrolled, update the line number area widget
    void updateLineNumberArea(const QRect &rect, int dy);

public slots:
    void onFileSelected(const QString &filePath);
    void onCodeGenerated(const QString &code);

private:
    QWidget *lineNumberArea;
    Highlighter *highlighter;
    QString currentFilePath;
};

#endif // CODEEDITOR_H
