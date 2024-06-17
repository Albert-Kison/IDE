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
    keywordPatterns << "\\bABORT\\b" << "\\bACTION\\b" << "\\bADD\\b"
                    << "\\bAFTER\\b" << "\\bALL\\b" << "\\bALTER\\b"
                    << "\\bALWAYS\\b" << "\\bANALYZE\\b" << "\\bAND\\b"
                    << "\\bAS\\b" << "\\bASC\\b" << "\\bATTACH\\b"
                    << "\\bAUTOINCREMENT\\b" << "\\bBEFORE\\b" << "\\bBEGIN\\b"
                    << "\\bBETWEEN\\b" << "\\bBY\\b" << "\\bCASCADE\\b"
                    << "\\bCASE\\b" << "\\bCAST\\b" << "\\bCHECK\\b"
                    << "\\bCOLLATE\\b" << "\\bCOLUMN\\b" << "\\bCOMMIT\\b"
                    << "\\bCONFLICT\\b" << "\\bCONSTRAINT\\b" << "\\bCREATE\\b"
                    << "\\bCROSS\\b" << "\\bCURRENT\\b" << "\\bCURRENT_DATE\\b"
                    << "\\bCURRENT_TIME\\b" << "\\bCURRENT_TIMESTAMP\\b" << "\\bDATABASE\\b"
                    << "\\bDEFAULT\\b" << "\\bDEFERRABLE\\b" << "\\bDEFERRED\\b"
                    << "\\bDELETE\\b" << "\\bDESC\\b" << "\\bDETACH\\b"
                    << "\\bDISTINCT\\b" << "\\bDO\\b" << "\\bDROP\\b"
                    << "\\bEACH\\b" << "\\bELSE\\b" << "\\bEND\\b"
                    << "\\bESCAPE\\b" << "\\bEXCEPT\\b" << "\\bEXCLUDE\\b"
                    << "\\bEXCLUSIVE\\b" << "\\bEXISTS\\b" << "\\bEXPLAIN\\b"
                    << "\\bFAIL\\b" << "\\bFILTER\\b" << "\\bFIRST\\b"
                    << "\\bFOLLOWING\\b" << "\\bFOR\\b" << "\\bFOREIGN\\b"
                    << "\\bFROM\\b" << "\\bFULL\\b" << "\\bGENERATED\\b"
                    << "\\bGLOB\\b" << "\\bGROUP\\b" << "\\bGROUPS\\b"
                    << "\\bHAVING\\b" << "\\bIF\\b" << "\\bIGNORE\\b"
                    << "\\bIMMEDIATE\\b" << "\\bIN\\b" << "\\bINDEX\\b"
                    << "\\bINDEXED\\b" << "\\bINITIALLY\\b" << "\\bINNER\\b"
                    << "\\bINSERT\\b" << "\\bINSTEAD\\b" << "\\bINTERSECT\\b"
                    << "\\bINTO\\b" << "\\bIS\\b" << "\\bISNULL\\b"
                    << "\\bJOIN\\b" << "\\bKEY\\b" << "\\bLAST\\b"
                    << "\\bLEFT\\b" << "\\bLIKE\\b" << "\\bLIMIT\\b"
                    << "\\bMATCH\\b" << "\\bNATURAL\\b" << "\\bNO\\b"
                    << "\\bNOT\\b" << "\\bNOTHING\\b" << "\\bNOTNULL\\b"
                    << "\\bNULL\\b" << "\\bNULLS\\b" << "\\bOF\\b"
                    << "\\bOFFSET\\b" << "\\bON\\b" << "\\bOR\\b"
                    << "\\bORDER\\b" << "\\bOTHERS\\b" << "\\bOUTER\\b"
                    << "\\bOVER\\b" << "\\bPARTITION\\b" << "\\bPLAN\\b"
                    << "\\bPRAGMA\\b" << "\\bPRECEDING\\b" << "\\bPRIMARY\\b"
                    << "\\bQUERY\\b" << "\\bRAISE\\b" << "\\bRANGE\\b"
                    << "\\bRECURSIVE\\b" << "\\bREFERENCES\\b" << "\\bREGEXP\\b"
                    << "\\bREINDEX\\b" << "\\bRELEASE\\b" << "\\bRENAME\\b"
                    << "\\bREPLACE\\b" << "\\bRESTRICT\\b" << "\\bRETURNING\\b"
                    << "\\bRIGHT\\b" << "\\bROLLBACK\\b" << "\\bROW\\b"
                    << "\\bROWS\\b" << "\\bSAVEPOINT\\b" << "\\bSELECT\\b"
                    << "\\bSET\\b" << "\\bTABLE\\b" << "\\bTEMP\\b"
                    << "\\bTEMPORARY\\b" << "\\bTHEN\\b" << "\\bTIES\\b"
                    << "\\bTO\\b" << "\\bTRANSACTION\\b" << "\\bTRIGGER\\b"
                    << "\\bUNBOUNDED\\b" << "\\bUNION\\b" << "\\bUNIQUE\\b"
                    << "\\bUPDATE\\b" << "\\bUSING\\b" << "\\bVACUUM\\b"
                    << "\\bVALUES\\b" << "\\bVIEW\\b" << "\\bVIRTUAL\\b"
                    << "\\bWHEN\\b" << "\\bWHERE\\b" << "\\bWINDOW\\b"
                    << "\\bWITH\\b" << "\\bWITHOUT\\b";

    QStringList sqlDataTypes;
    sqlDataTypes << "\\bNULL\\b" << "\\bINTEGER\\b" << "\\bREAL\\b"
                 << "\\bTEXT\\b" << "\\bBLOB\\b"
                 << "\\bBOOLEAN\\b" << "\\bDATE\\b" << "\\bDATETIME\\b"
                 << "\\bINT\\b" << "\\bVARCHAR\\b" << "\\bCHAR\\b"
                 << "\\bFLOAT\\b" << "\\bNUMERIC\\b" << "\\bDECIMAL\\b"
                 << "\\bDOUBLE\\b" << "\\bBOOLEAN\\b" << "\\bBIT\\b"
                 << "\\bTINYINT\\b" << "\\bSMALLINT\\b" << "\\bMEDIUMINT\\b"
                 << "\\bBIGINT\\b" << "\\bUNSIGNED\\b" << "\\bINT2\\b"
                 << "\\bINT8\\b";


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

    // highlight rule for single line comments
    singleLineCommentFormat.setForeground(Qt::gray);
    rule.pattern = QRegularExpression("--[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    // // highlight rule for multiline comments
    // multiLineCommentFormat.setForeground(Qt::red);

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
