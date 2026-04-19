#include "parser.hpp"
#include <QDebug>

Parser::Parser(){}

bool Parser::parse(const QString &code)
{
	// Лексический анализ
	Lexer lexer(code);
	auto tokens = lexer.tokenize();
	m_tokens.assign(tokens.begin(), tokens.end());
	
	if (lexer.hasError()) 
	{
		m_error = lexer.errorString();
		return false;
	}
	
	m_pos = 0;
	
	// Синтаксический анализ
	try 
	{
		m_ast = parseProgram();
		return true;
	} catch (const QString &error) {
		m_error = error;
		return false;
	}
}

Lexer::Token Parser::peek() const
{
	if (m_pos < m_tokens.size())
		return m_tokens[m_pos];
	Lexer::Token token;
	token.type = Lexer::TokenEndOfFile;
	return token;
}

Lexer::Token Parser::advance()
{
	if (m_pos < m_tokens.size())
		return m_tokens[m_pos++];
	Lexer::Token token;
	token.type = Lexer::TokenEndOfFile;
	return token;
}

bool Parser::match(Lexer::TokenType type, const QString &value)
{
	Lexer::Token token = peek();
	if (token.type == type && (value.isEmpty() || token.value.compare(value, Qt::CaseInsensitive) == 0)) 
	{
		advance();
		return true;
	}
	return false;
}

bool Parser::expect(Lexer::TokenType type, const QString &value)
{
	if (match(type, value))
		return true;
	Lexer::Token token = peek();
	error(QString("Expected %1 '%2', got %3")
		  .arg(type == Lexer::TokenKeyword ? "keyword" : "token")
		  .arg(value.isEmpty() ? "something" : value)
		  .arg(token.toString()));
	return false;
}

void Parser::error(const QString &message){throw message;}

std::unique_ptr<ASTProgram> Parser::parseProgram()
{
	auto program = std::make_unique<ASTProgram>();
	
	// program
	expect(Lexer::TokenKeyword, "program");
	
	// identifier
	Lexer::Token idToken = peek();
	expect(Lexer::TokenIdentifier);
	
	// ;
	expect(Lexer::TokenDelimiter, ";");
	
	// Объявления
	while (peek().type == Lexer::TokenKeyword) {
		QString kw = peek().value.toLower();
		if (kw == "const") parseConstDeclarations(program->declarations);
		else if (kw == "var") parseVarDeclarations(program->declarations);
		else break;
	}
	
	// begin ... end.
	program->body = parseCompoundStatement();
	expect(Lexer::TokenDelimiter, ".");
	
	return program;
}

void Parser::parseConstDeclarations(std::vector<std::unique_ptr<ASTNode>> &declarations)
{
	expect(Lexer::TokenKeyword, "const");
	
	while (peek().type == Lexer::TokenIdentifier) 
	{
		auto decl = std::make_unique<ASTConstDecl>();
		decl->name = advance().value;
		
		expect(Lexer::TokenOperator, ":");
		
		if (peek().type == Lexer::TokenKeyword) 
		{
			QString typeStr = advance().value.toLower();
			if (typeStr == "integer") {
				decl->type = DataType::Integer;
			} else if (typeStr == "double") {
				decl->type = DataType::Double;
			} else if (typeStr == "boolean") {
				decl->type = DataType::Boolean;
			} else if (typeStr == "string") {
				decl->type = DataType::String;
			}
			expect(Lexer::TokenOperator, "=");
		} 
		else 
		{
			expect(Lexer::TokenOperator, "=");
			decl->type = DataType::Unknown;
		}
		
		Lexer::Token valueToken = peek();
		if (valueToken.type == Lexer::TokenNumber) 
		{
			advance();
			if (valueToken.value.contains('.')) 
			{
				decl->value = valueToken.value.toDouble();
				decl->type = DataType::Double;
			} 
			else 
			{
				decl->value = valueToken.value.toInt();
				if (decl->type == DataType::Unknown)
					decl->type = DataType::Integer;
			}
		} 
		else if (valueToken.type == Lexer::TokenString) 
		{
			advance();
			decl->value = valueToken.value;
			decl->type = DataType::String;
		} 
		else if (valueToken.type == Lexer::TokenKeyword) 
		{
			if (valueToken.value.toLower() == "true") 
			{
				advance();
				decl->value = true;
				decl->type = DataType::Boolean;
			} 
			else if (valueToken.value.toLower() == "false") 
			{
				advance();
				decl->value = false;
				decl->type = DataType::Boolean;
			}
		}
		
		expect(Lexer::TokenDelimiter, ";");
		declarations.push_back(std::move(decl));
	}
}

void Parser::parseVarDeclarations(std::vector<std::unique_ptr<ASTNode>> &declarations)
{
	expect(Lexer::TokenKeyword, "var");
	
	while (peek().type == Lexer::TokenIdentifier) {
		auto decl = std::make_unique<ASTVarDecl>();
		
		do 
		{
			if (peek().type == Lexer::TokenDelimiter && peek().value == ",") {
				advance();
			}
			decl->names.push_back(advance().value);
		} while (peek().type == Lexer::TokenDelimiter && peek().value == ",");
		
		expect(Lexer::TokenOperator, ":");
		
		expect(Lexer::TokenKeyword);
		QString typeStr = m_tokens[m_pos - 1].value.toLower();
		if (typeStr == "integer")
			decl->type = DataType::Integer;
		else if (typeStr == "double") 
			decl->type = DataType::Double;
		else if (typeStr == "boolean") 
			decl->type = DataType::Boolean;
		else if (typeStr == "string") 
			decl->type = DataType::String;
		
		
		expect(Lexer::TokenDelimiter, ";");
		declarations.push_back(std::move(decl));
	}
}

std::unique_ptr<ASTCompoundStatement> Parser::parseCompoundStatement()
{
	auto compound = std::make_unique<ASTCompoundStatement>();
	
	expect(Lexer::TokenKeyword, "begin");
	
	while (!match(Lexer::TokenKeyword, "end")) 
	{
		compound->statements.push_back(parseStatement());
		if (!match(Lexer::TokenDelimiter, ";")) {
			// Точка с запятой может отсутствовать перед end
			if (peek().type == Lexer::TokenKeyword && peek().value.toLower() == "end") break;
			expect(Lexer::TokenDelimiter, ";");
		}
	}
	
	return compound;
}

std::unique_ptr<ASTNode> Parser::parseStatement()
{
	Lexer::Token token = peek();
	
	if (token.type == Lexer::TokenKeyword) 
	{
		QString kw = token.value.toLower();
		if (kw == "if") 																return parseIfStatement();
		else if (kw == "write" || kw == "writeln") 			return parseWriteStatement();
		else if (kw == "read" || kw == "readln") 				return parseReadStatement();
		else if (kw == "begin") 												return parseCompoundStatement();
	}	
	else if (token.type == Lexer::TokenIdentifier) 		return parseAssignment();
	
	
	error("Unexpected token in statement: " + token.toString());
	return nullptr;
}

std::unique_ptr<ASTNode> Parser::parseIfStatement()
{
	auto ifStmt = std::make_unique<ASTIfStatement>();
	
	expect(Lexer::TokenKeyword, "if");
	ifStmt->condition = parseExpression();
	expect(Lexer::TokenKeyword, "then");
	ifStmt->thenBranch = parseStatement();
	
	if (match(Lexer::TokenKeyword, "else")) {
		ifStmt->elseBranch = parseStatement();
	}
	
	return ifStmt;
}

std::unique_ptr<ASTNode> Parser::parseWriteStatement()
{
	auto writeStmt = std::make_unique<ASTWriteStatement>();
	
	Lexer::Token kw = advance();
	writeStmt->writeln = (kw.value.toLower() == "writeln");
	
	expect(Lexer::TokenDelimiter, "(");
	
	if (!match(Lexer::TokenDelimiter, ")")) {
		do 
		{
			if (peek().type == Lexer::TokenDelimiter && peek().value == ",") advance();
			writeStmt->arguments.push_back(parseExpression());
		} while (peek().type == Lexer::TokenDelimiter && peek().value == ",");
		expect(Lexer::TokenDelimiter, ")");
	}
	
	return writeStmt;
}

std::unique_ptr<ASTNode> Parser::parseReadStatement()
{
	auto readStmt = std::make_unique<ASTReadStatement>();
	
	advance();
	
	expect(Lexer::TokenDelimiter, "(");
	
	do {
		if (peek().type == Lexer::TokenDelimiter && peek().value == ",") advance();
		Lexer::Token id = peek();
		expect(Lexer::TokenIdentifier);
		readStmt->variables.push_back(id.value);
	} while (peek().type == Lexer::TokenDelimiter && peek().value == ",");
	
	expect(Lexer::TokenDelimiter, ")");
	
	return readStmt;
}

std::unique_ptr<ASTNode> Parser::parseAssignment()
{
	auto assign = std::make_unique<ASTAssignment>();
	
	assign->name = advance().value;
	expect(Lexer::TokenOperator, ":=");
	assign->expression = parseExpression();
	
	return assign;
}

std::unique_ptr<ASTNode> Parser::parseExpression(){return parseSimpleExpression();}

std::unique_ptr<ASTNode> Parser::parseSimpleExpression()
{
	auto left = parseTerm();
	
	while (peek().type == Lexer::TokenOperator || 
		   (peek().type == Lexer::TokenKeyword && 
			(isAdditiveOp(peek().value.toLower()) || 
			 isComparisonOp(peek().value.toLower())))) 
	{
		
		QString op = peek().value.toLower();
		if (isAdditiveOp(op) || isComparisonOp(op)) {
			advance();
			auto right = parseTerm();
			auto binaryOp = std::make_unique<ASTBinaryOp>();
			binaryOp->op = op;
			binaryOp->left = std::move(left);
			binaryOp->right = std::move(right);
			left = std::move(binaryOp);
		}
		else 
			break;
	}
	
	return left;
}

std::unique_ptr<ASTNode> Parser::parseTerm()
{
	auto left = parseFactor();
	
	while (peek().type == Lexer::TokenOperator || 
		   (peek().type == Lexer::TokenKeyword && 
			isMultiplicativeOp(peek().value.toLower()))) 
	{
		
		QString op = peek().value.toLower();
		if (isMultiplicativeOp(op)) 
		{
			advance();
			auto right = parseFactor();
			auto binaryOp = std::make_unique<ASTBinaryOp>();
			binaryOp->op = op;
			binaryOp->left = std::move(left);
			binaryOp->right = std::move(right);
			left = std::move(binaryOp);
		} 
		else 
			break;
	}
	
	return left;
}

std::unique_ptr<ASTNode> Parser::parseFactor()
{
	Lexer::Token token = peek();
	
	// Унарные операторы
	if (token.type == Lexer::TokenOperator) 
	{
		QString op = token.value;
		if (op == "+" || op == "-" || op == "not") 
		{
			advance();
			auto unaryOp = std::make_unique<ASTUnaryOp>();
			unaryOp->op = op;
			unaryOp->operand = parseFactor();
			return unaryOp;
		}
	}
	
	// Скобки
	if (token.type == Lexer::TokenDelimiter && token.value == "(") 
	{
		advance();
		auto expr = parseExpression();
		expect(Lexer::TokenDelimiter, ")");
		return expr;
	}
	
	// Идентификатор
	if (token.type == Lexer::TokenIdentifier) 
	{
		auto id = std::make_unique<ASTIdentifier>();
		id->name = advance().value;
		return id;
	}
	
	// Числовой литерал
	if (token.type == Lexer::TokenNumber) 
	{
		auto num = std::make_unique<ASTNumberLiteral>();
		QString value = advance().value;
		if (value.contains('.')) 
		{
			num->value = value.toDouble();
			num->type = DataType::Double;
		} 
		else 
		{
			num->value = value.toInt();
			num->type = DataType::Integer;
		}
		return num;
	}
	
	// Строковый литерал
	if (token.type == Lexer::TokenString) 
	{
		auto str = std::make_unique<ASTStringLiteral>();
		str->value = advance().value;
		return str;
	}
	
	// Булевы литералы
	if (token.type == Lexer::TokenKeyword) 
	{
		QString kw = token.value.toLower();
		if (kw == "true" || kw == "false") 
		{
			advance();
			auto num = std::make_unique<ASTNumberLiteral>();
			num->value = (kw == "true");
			num->type = DataType::Boolean;
			return num;
		}
	}
	
	error("Unexpected token in factor: " + token.toString());
	return nullptr;
}

int Parser::getPrecedence(const QString &op)
{
	if (op == "or") return 1;
	if (op == "and") return 2;
	if (op == "not") return 3;
	if (isComparisonOp(op)) return 4;
	if (isAdditiveOp(op)) return 5;
	if (isMultiplicativeOp(op)) return 6;
	return 0;
}

bool Parser::isComparisonOp(const QString &op)
{
	return op == "=" || op == "<>" || op == "<" || op == "<=" || 
		   op == ">" || op == ">=";
}

bool Parser::isAdditiveOp(const QString &op)
{
	return op == "+" || op == "-" || op == "or";
}

bool Parser::isMultiplicativeOp(const QString &op)
{
	return op == "*" || op == "/" || op == "div" || op == "mod" || op == "and";
}