#pragma once

#include <QString>
#include <memory>
#include <vector>
#include "lexer.hpp"
#include "ast/ast_node.hpp"

class Parser {
public:
	Parser();
	
	bool parse(const QString &code);
	ASTProgram* getAST() const { return m_ast.get(); }
	QString errorString() const { return m_error; }
	
private:
	std::unique_ptr<ASTProgram> m_ast;
	std::vector<Lexer::Token> m_tokens;
	size_t m_pos = 0;
	QString m_error;
	
	// Вспомогательные методы
	Lexer::Token peek() const;
	Lexer::Token advance();
	bool match(Lexer::TokenType type, const QString &value = QString());
	bool expect(Lexer::TokenType type, const QString &value = QString());
	void error(const QString &message);
	
	// Правила грамматики
	std::unique_ptr<ASTProgram> parseProgram();
	void parseConstDeclarations(std::vector<std::unique_ptr<ASTNode>> &declarations);
	void parseVarDeclarations(std::vector<std::unique_ptr<ASTNode>> &declarations);
	std::unique_ptr<ASTCompoundStatement> parseCompoundStatement();
	std::unique_ptr<ASTNode> parseStatement();
	std::unique_ptr<ASTNode> parseIfStatement();
	std::unique_ptr<ASTNode> parseWriteStatement();
	std::unique_ptr<ASTNode> parseReadStatement();
	std::unique_ptr<ASTNode> parseAssignment();
	std::unique_ptr<ASTNode> parseExpression();
	std::unique_ptr<ASTNode> parseSimpleExpression();
	std::unique_ptr<ASTNode> parseTerm();
	std::unique_ptr<ASTNode> parseFactor();
	
	// Приоритеты операторов для постфиксной формы
	int getPrecedence(const QString &op);
	bool isComparisonOp(const QString &op);
	bool isAdditiveOp(const QString &op);
	bool isMultiplicativeOp(const QString &op);
};
