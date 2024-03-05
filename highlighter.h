 #ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>
#include <QRegularExpression>

class QTextDocument;

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:

    // init the highlighting rules
    Highlighter(QTextDocument *parent = 0);

protected:

    // whenever the text blocks change, highlight the text
    void highlightBlock(const QString &text);

private:

    // the highligting rule for each pattern
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

    // regular expressions for multiline comments
    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    // how to highlight the keywords
    QTextCharFormat keywordFormat;
    // how to highlight the class name
    QTextCharFormat classFormat;
    // how to highlight single line comments
    QTextCharFormat singleLineCommentFormat;
    // how to highlight multiline comments
    QTextCharFormat multiLineCommentFormat;
    // how to highlight strings
    QTextCharFormat quotationFormat;
    // how to highlight the function name
    QTextCharFormat functionFormat;
};

#endif
