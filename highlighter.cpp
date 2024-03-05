 #include <QtGui>

#include "highlighter.h"


Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // how to highlight the keywords
    keywordFormat.setForeground(Qt::darkMagenta);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\babstract\\b" << "\\bassert\\b" << "\\bboolean\\b"
                    << "\\bbreak\\b" << "\\bbyte\\b" << "\\bcase\\b"
                    << "\\bcatch\\b" << "\\bchar\\b" << "\\bclass\\b"
                    << "\\bcontinue\\b" << "\\bdefault\\b" << "\\bdo\\b"
                    << "\\bdouble\\b" << "\\belse\\b" << "\\benum\\b"
                    << "\\bextends\\b" << "\\bfinal\\b" << "\\bfinally\\b"
                    << "\\bfloat\\b" << "\\bfor\\b" << "\\bgoto\\b"
                    << "\\bif\\b" << "\\binstanceof\\b" << "\\bint\\b"
                    << "\\binterface\\b" << "\\blong\\b" << "\\bnative\\b"
                    << "\\bpackage\\b" << "\\bprivate\\b" << "\\bprotected\\b"
                    << "\\bpublic\\b" << "\\breturn\\b" << "\\bshort\\b"
                    << "\\bstatic\\b" << "\\bstrictfp\\b" << "\\bsuper\\b"
                    << "\\bswitch\\b" << "\\bsynchronized\\b" << "\\bthis\\b"
                    << "\\bthrow\\b" << "\\bthrows\\b" << "\\btransient\\b"
                    << "\\btry\\b" << "\\bvoid\\b" << "\\bvolatile\\b"
                    << "\\bwhile\\b";

    // highlight rules for each keyword
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // highlight rule for class names
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    // highlight rule for single line comments
    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    // highlight rule for multiline comments
    multiLineCommentFormat.setForeground(Qt::red);

    // highlight rule for strings
    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    // highlight rule for function names
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    // regular expressions for multiline comments
    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");
}


// whenever the text blocks change, highlight the text
void Highlighter::highlightBlock(const QString &text)
{
    // search for the patterns and highlight the text
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpression expression(rule.pattern);
        QRegularExpressionMatchIterator i = expression.globalMatch(text);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // highlight multiline comments
    setCurrentBlockState(0);

    QRegularExpressionMatchIterator i;
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.globalMatch(text).next().capturedStart();

    while (startIndex >= 0) {
        QRegularExpressionMatch match = i.next();
        int endIndex = commentEndExpression.globalMatch(text, startIndex).next().capturedStart();
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + commentEndExpression.globalMatch(text).next().capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.globalMatch(text, startIndex + commentLength).next().capturedStart();
    }
}
