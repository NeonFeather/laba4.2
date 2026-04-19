#include "syntax_highlighter.hpp"
#include "theme/theme.hpp"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;
    
    // Ключевые слова Pascal
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Theme::Colors::Keyword);
    keywordFormat.setFontWeight(QFont::Bold);
    
    QStringList keywordPatterns = {
        "\\bprogram\\b", "\\bconst\\b", "\\bvar\\b", "\\bbegin\\b", "\\bend\\b",
        "\\bif\\b", "\\bthen\\b", "\\belse\\b", "\\bwhile\\b", "\\bdo\\b",
        "\\bfor\\b", "\\bto\\b", "\\bdownto\\b", "\\bdiv\\b", "\\bmod\\b",
        "\\band\\b", "\\bor\\b", "\\bnot\\b", "\\bwrite\\b", "\\bwriteln\\b",
        "\\bread\\b", "\\breadln\\b"
    };
    
    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
    
    // Типы данных
    QTextCharFormat typeFormat;
    typeFormat.setForeground(Theme::Colors::Type);
    
    QStringList typePatterns = {
        "\\binteger\\b", "\\bdouble\\b", "\\breal\\b", "\\bboolean\\b",
        "\\bstring\\b", "\\bchar\\b"
    };
    
    for (const QString &pattern : typePatterns) {
        rule.pattern = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);
        rule.format = typeFormat;
        highlightingRules.append(rule);
    }
    
    // Числа
    QTextCharFormat numberFormat;
    numberFormat.setForeground(Theme::Colors::Number);
    
    rule.pattern = QRegularExpression("\\b[0-9]+(\\.[0-9]+)?\\b");
    rule.format = numberFormat;
    highlightingRules.append(rule);
    
    // Строки
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Theme::Colors::String);
    
    rule.pattern = QRegularExpression("\"[^\"]*\"|'[^']*'");
    rule.format = stringFormat;
    highlightingRules.append(rule);
    
    // Комментарии
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Theme::Colors::Comment);
    commentFormat.setFontItalic(true);
    
    rule.pattern = QRegularExpression("\\{[^}]*\\}|\\(\\*.*?\\*\\)|//[^\n]*");
    rule.format = commentFormat;
    highlightingRules.append(rule);
    
    // Операторы
    QTextCharFormat operatorFormat;
    operatorFormat.setForeground(Theme::Colors::Operator);
    
    rule.pattern = QRegularExpression(":=|\\+|\\-|\\*|/|=|<>|<=|>=|<|>");
    rule.format = operatorFormat;
    highlightingRules.append(rule);
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}