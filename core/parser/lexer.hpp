#pragma once

#include <QString>
#include <QVector>

class Lexer {
public:
	enum TokenType {
		TokenNone,
		TokenIdentifier,
		TokenKeyword,
		TokenNumber,
		TokenString,
		TokenOperator,
		TokenDelimiter,
		TokenEndOfFile
	};
	
	struct Token {
		TokenType type;
		QString value;
		int line;
		int column;
		
		QString toString() const;
	};
	
	explicit Lexer(const QString &source);
	
	Token nextToken();
	Token peekToken();
	QVector<Token> tokenize();
	
	bool hasError() const { return m_hasError; }
	QString errorString() const { return m_error; }
	
private:
	QString m_source;
	int m_pos = 0;
	int m_line = 1;
	int m_column = 1;
	bool m_hasError = false;
	QString m_error;
	
	void skipWhitespace();
	void skipComment();
	Token readNumber();
	Token readString();
	Token readIdentifierOrKeyword();
	Token readOperator();
	Token readDelimiter();
	
	bool isKeyword(const QString &word);
	bool isLetter(QChar ch) const;
	bool isDigit(QChar ch) const;
	QChar peek() const;
	QChar advance();
	bool match(QChar expected);
};