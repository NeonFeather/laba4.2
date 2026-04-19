#pragma once

#include <QStack>
#include <QVariant>
#include <functional>
#include <vector>
#include "ast/ast_node.hpp"
#include "symbols/symbol_table.hpp"

class PostfixEvaluator {
public:
	using OutputCallback = std::function<void(const QString&)>;
	using InputCallback = std::function<QString()>;

	PostfixEvaluator();
	
	void setOutputCallback(OutputCallback cb) { m_output = cb; }
	void setInputCallback(InputCallback cb) { m_input = cb; }
	const SymbolTable* getSymbolTable() const { return &m_symbols; }
	
	int evaluate(ASTProgram *program);

private:
	SymbolTable m_symbols;
	OutputCallback m_output;
	InputCallback m_input;
	
	void executeStatement(ASTNode *node);
	void executeCompoundStatement(ASTCompoundStatement *compound);
	void executeAssignment(ASTAssignment *assign);
	void executeIfStatement(ASTIfStatement *ifStmt);
	void executeWriteStatement(ASTWriteStatement *writeStmt);
	void executeReadStatement(ASTReadStatement *readStmt);
	
	QVariant evaluateExpression(ASTNode *expr);
	QVariant evaluateBinaryOp(const QString &op, const QVariant &left, const QVariant &right);
	QVariant evaluateUnaryOp(const QString &op, const QVariant &operand);
	
	void declareConstants(std::vector<std::unique_ptr<ASTNode>> &declarations);
	void declareVariables(std::vector<std::unique_ptr<ASTNode>> &declarations);
	
	QString variantToString(const QVariant &var);
};
