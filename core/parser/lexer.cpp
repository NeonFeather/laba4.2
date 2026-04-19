#include "lexer.hpp"
#include <QSet>

Lexer::Lexer(const QString &source): m_source(source) {}

QString Lexer::Token::toString() const
{
	QString typeStr;
	switch (type) 
	{
		case TokenIdentifier: typeStr = "Identifier"; break;
		case TokenKeyword: 		typeStr = "Keyword"; break;
		case TokenNumber: 		typeStr = "Number"; break;
		case TokenString: 		typeStr = "String"; break;
		case TokenOperator: 	typeStr = "Operator"; break;
		case TokenDelimiter: 	typeStr = "Delimiter"; break;
		case TokenEndOfFile: 	typeStr = "EOF"; break;
		default: 							typeStr = "None"; break;
	}
	return QString("%1('%2') at %3:%4").arg(typeStr, value).arg(line).arg(column);
}

QVector<Lexer::Token> Lexer::tokenize()
{
	QVector<Token> tokens;
	Token token;
	
	do {
		token = nextToken();
		tokens.append(token);
	} while (token.type != TokenEndOfFile && token.type != TokenNone);
	
	return tokens;
}

void Lexer::skipWhitespace()
{
	while (m_pos < m_source.length()) 
	{
		QChar ch = m_source[m_pos];
		if (ch == ' ' || ch == '\t' || ch == '\r') advance();
		else if (ch == '\n') {
			m_line++;
			m_column = 1;
			m_pos++;
		} else if (ch == '{') skipComment();
		else break;
	}
}

void Lexer::skipComment()
{
	// Пропуск {...}
	advance(); 
	while (m_pos < m_source.length() && m_source[m_pos] != '}') 
	{
		if (m_source[m_pos] == '\n') 
		{
			m_line++;
			m_column = 1;
		}
		m_pos++;
	}
	if (m_pos < m_source.length()) { advance(); }
}

Lexer::Token Lexer::readNumber()
{
	Token token;
	token.line = m_line;
	token.column = m_column;
	token.type = TokenNumber;
	
	while (m_pos < m_source.length()) 
	{
		QChar ch = peek();
		if (isDigit(ch)) token.value += advance();
		else if (ch == '.') token.value += advance();
		else break;
	}
	
	return token;
}

Lexer::Token Lexer::readString()
{
	Token token;
	token.line = m_line;
	token.column = m_column;
	token.type = TokenString;
	
	QChar quote = advance(); // ' или "
	
	while (m_pos < m_source.length() && peek() != quote) {
		token.value += advance();
	}
	
	if (m_pos < m_source.length()) advance(); // ' или ""
	
	return token;
}

Lexer::Token Lexer::readIdentifierOrKeyword()
{
	Token token;
	token.line = m_line;
	token.column = m_column;
	
	while (m_pos < m_source.length()) 
	{
		QChar ch = peek();
		if (isLetter(ch) || isDigit(ch) || ch == '_') token.value += advance();
		else break;
	}
	
	token.type = isKeyword(token.value) ? TokenKeyword : TokenIdentifier;
	return token;
}

Lexer::Token Lexer::readOperator()
{
	Token token;
	token.line = m_line;
	token.column = m_column;
	token.type = TokenOperator;
	
	QChar ch = advance();
	token.value = ch;
	
	if (ch == ':' && peek() == '=')				token.value += advance();
	else if (ch == '<' && peek() == '>')	token.value += advance();
	else if (ch == '<' && peek() == '=')	token.value += advance();
	else if (ch == '>' && peek() == '=')	token.value += advance();
		
	return token;
}

Lexer::Token Lexer::readDelimiter()
{
	Token token;
	token.line = m_line;
	token.column = m_column;
	token.type = TokenDelimiter;
	token.value = advance();
	return token;
}

bool Lexer::isKeyword(const QString &word)
{
	static QSet<QString> keywords = 
	{
		"program", "const", "var", "begin", "end",
		"if", "then", "else", "while", "do", "for", "to", "downto",
		"integer", "double", "boolean", "string",
		"and", "or", "not", "div", "mod",
		"read", "readln", "write", "writeln",
		"true", "false"
	};
	return keywords.contains(word.toLower());
}

Lexer::Token Lexer::nextToken()
{
	skipWhitespace();
	
	if (m_pos >= m_source.length()) 
	{
		Token token;
		token.type = TokenEndOfFile;
		token.line = m_line;
		token.column = m_column;
		return token;
	}
	
	QChar ch = peek();
	
	if (isLetter(ch) || ch == '_')
		return readIdentifierOrKeyword();
	else if (isDigit(ch))
		return readNumber();
	else if (ch == '$')
		return readNumber();
	else if (ch == '\'' || ch == '"')
		return readString();
	else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || 
			   ch == '=' || ch == '<' || ch == '>' || ch == ':') 
		return readOperator();
	else if (ch == ';' || ch == '.' || ch == ',' || ch == '(' || 
			   ch == ')' || ch == '[' || ch == ']') 
		return readDelimiter();
	
	// Неизвестный символ
	Token token;
	token.type = TokenNone;
	token.line = m_line;
	token.column = m_column;
	token.value = advance();
	m_hasError = true;
	m_error = QString("Unexpected character '%1' at %2:%3").arg(token.value).arg(m_line).arg(m_column);
	return token;
}

Lexer::Token Lexer::peekToken()
{
	int savedPos = m_pos;
	int savedLine = m_line;
	int savedColumn = m_column;
	
	Token token = nextToken();
	
	m_pos = savedPos;
	m_line = savedLine;
	m_column = savedColumn;
	
	return token;
}

bool Lexer::isLetter(QChar ch) const {
	return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool Lexer::isDigit(QChar ch) const{
	return ch >= '0' && ch <= '9';
}


QChar Lexer::peek() const
{
	if (m_pos < m_source.length())
		return m_source[m_pos];
	return '\0';
}

QChar Lexer::advance()
{
	if (m_pos < m_source.length()) {
		QChar ch = m_source[m_pos++];
		m_column++;
		return ch;
	}
	return '\0';
}

bool Lexer::match(QChar expected)
{
	if (peek() == expected) {
		advance();
		return true;
	}
	return false;
}