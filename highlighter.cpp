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
    keywordPatterns << "\\bSELECT\\b" << "\\bFROM\\b" << "\\bWHERE\\b"
                    << "\\bAND\\b" << "\\bOR\\b" << "\\bNOT\\b"
                    << "\\bINSERT\\b" << "\\bINTO\\b" << "\\bVALUES\\b"
                    << "\\bUPDATE\\b" << "\\bSET\\b" << "\\bDELETE\\b"
                    << "\\bCREATE\\b" << "\\bTABLE\\b" << "\\bALTER\\b"
                    << "\\bDROP\\b" << "\\bINDEX\\b" << "\\bVIEW\\b"
                    << "\\bJOIN\\b" << "\\bINNER\\b" << "\\bLEFT\\b"
                    << "\\bRIGHT\\b" << "\\bOUTER\\b" << "\\bFULL\\b"
                    << "\\bON\\b" << "\\bGROUP\\b" << "\\bBY\\b"
                    << "\\bORDER\\b" << "\\bASC\\b" << "\\bDESC\\b"
                    << "\\bHAVING\\b" << "\\bDISTINCT\\b" << "\\bAS\\b"
                    << "\\bCOUNT\\b" << "\\bSUM\\b" << "\\bAVG\\b"
                    << "\\bMIN\\b" << "\\bMAX\\b" << "\\bIN\\b"
                    << "\\bBETWEEN\\b" << "\\bLIKE\\b" << "\\bCASE\\b"
                    << "\\bWHEN\\b" << "\\bTHEN\\b" << "\\bELSE\\b"
                    << "\\bEND\\b" << "\\bIS\\b" << "\\bNULL\\b"
                    << "\\bTRUE\\b" << "\\bFALSE\\b" << "\\bTOP\\b"
                    << "\\bPRIMARY\\b" << "\\bKEY\\b";


    QStringList sqlDataTypes;
    sqlDataTypes << "\\bINT\\b" << "\\bVARCHAR\\b" << "\\bCHAR\\b"
                 << "\\bTEXT\\b" << "\\bDATE\\b" << "\\bDATETIME\\b"
                 << "\\bTIMESTAMP\\b" << "\\bDECIMAL\\b" << "\\bNUMERIC\\b"
                 << "\\bFLOAT\\b" << "\\bREAL\\b" << "\\bDOUBLE\\b"
                 << "\\bBOOLEAN\\b" << "\\bBIT\\b" << "\\bTINYINT\\b"
                 << "\\bSMALLINT\\b" << "\\bMEDIUMINT\\b" << "\\bBIGINT\\b"
                 << "\\bBINARY\\b" << "\\bVARBINARY\\b" << "\\bBLOB\\b"
                 << "\\bTINYBLOB\\b" << "\\bMEDIUMBLOB\\b" << "\\bLONGBLOB\\b"
                 << "\\bENUM\\b" << "\\bSET\\b";


    // highlight rules for each keyword
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    foreach (const QString &pattern, sqlDataTypes) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = functionFormat;
        highlightingRules.append(rule);
    }

    // // highlight rule for class names
    // classFormat.setFontWeight(QFont::Bold);
    // classFormat.setForeground(Qt::darkMagenta);
    // rule.pattern = QRegularExpression("\\bQ[A-Za-z]+\\b");
    // rule.format = classFormat;
    // highlightingRules.append(rule);

    // // highlight rule for single line comments
    // singleLineCommentFormat.setForeground(Qt::red);
    // rule.pattern = QRegularExpression("//[^\n]*");
    // rule.format = singleLineCommentFormat;
    // highlightingRules.append(rule);

    // // highlight rule for multiline comments
    // multiLineCommentFormat.setForeground(Qt::red);

    // // highlight rule for strings
    // quotationFormat.setForeground(Qt::darkGreen);
    // rule.pattern = QRegularExpression("\".*\"");
    // rule.format = quotationFormat;
    // highlightingRules.append(rule);

    // // highlight rule for function names
    // functionFormat.setFontItalic(true);
    // functionFormat.setForeground(Qt::blue);
    // rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    // rule.format = functionFormat;
    // highlightingRules.append(rule);

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
