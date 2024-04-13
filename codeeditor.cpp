#include "codeeditor.h"
#include "LineNumberArea.cpp"
#include "QtCore/qfile.h"
#include "QtGui/qpainter.h"
#include <QTextBlock>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);
    highlighter = new Highlighter(document());

    //  whenever block count changes, update the line number area width
    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);

    // whenever the text is scrolled, update the line number area widget
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);

    // whenever the cursor position changed, highlight the current line
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    // set the width for line number area widget
    updateLineNumberAreaWidth(0);

    // highlight first line
    highlightCurrentLine();
}


// calculate the width of the line number area widget
int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}


void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}


// whenever the text is scrolled, update the line number area widget
void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}


// whenever the size of the editor changes, resize the line number area
void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}


void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        // color of the selection
        QColor lineColor = QColor(Qt::gray).darker(150);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}


// paint the line number area
void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    // widget's background
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    // top and bottom y-coordinate of the first text block
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    // loop through all visible lines and paint the line numbers in the extra area for each line
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            // color of line number
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::onFileSelected(const QString &filePath) {
    QFile file(filePath);

    // Open the file in Read-Only mode
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Read the contents of the file
        QByteArray fileData = file.readAll();

        // Convert the data to QString
        QString fileText(fileData);

        // Set the contents of the CodeEditor
        setPlainText(fileText);

        // Close the file
        file.close();

        currentFilePath = filePath;
    } else {
        setPlainText("Failed to open the file: " + file.errorString());
        return;
    }
}


void CodeEditor::onCodeGenerated(const QString &code) {
    setPlainText(code);
}
