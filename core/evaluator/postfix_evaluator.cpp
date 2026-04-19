#include <QDebug>
#include <cmath>

#include "postfix_evaluator.hpp"


PostfixEvaluator::PostfixEvaluator(){}

int PostfixEvaluator::evaluate(ASTProgram *program)
{
	if (!program) return 1;
	
	try 
	{
		// Объявляем константы и переменные
		declareConstants(program->declarations);
		declareVariables(program->declarations);
		
		// Выполняем тело программы
		if (program->body) executeStatement(program->body.get());
		
		return 0;
	} catch (const QString &error) {
		if (m_output) m_output("Runtime error: " + error + "\n");
		return 1;
	} catch (...) {
		if (m_output) m_output("Unknown runtime error\n");
		return 1;
	}
}

void PostfixEvaluator::declareConstants(std::vector<std::unique_ptr<ASTNode>> &declarations)
{
	for (auto &decl : declarations) 
	{
		if (decl->nodeType() == ASTNodeType::ConstDeclaration) 
		{
			ASTConstDecl *constDecl = static_cast<ASTConstDecl*>(decl.get());
			m_symbols.addConstant(constDecl->name, constDecl->type, constDecl->value);
		}
	}
}

void PostfixEvaluator::declareVariables(std::vector<std::unique_ptr<ASTNode>> &declarations)
{
	for (auto &decl : declarations) 
	{
		if (decl->nodeType() == ASTNodeType::VarDeclaration) 
		{
			ASTVarDecl *varDecl = static_cast<ASTVarDecl*>(decl.get());
			for (const QString &name : varDecl->names)
				m_symbols.addVariable(name, varDecl->type);
		}
	}
}

void PostfixEvaluator::executeStatement(ASTNode *node)
{
	if (!node) return;
	
	switch (node->nodeType()) 
	{
	  case ASTNodeType::CompoundStatement:
		  executeCompoundStatement(static_cast<ASTCompoundStatement*>(node));
		  break;
	  case ASTNodeType::Assignment:
		  executeAssignment(static_cast<ASTAssignment*>(node));
		  break;
	  case ASTNodeType::IfStatement:
		  executeIfStatement(static_cast<ASTIfStatement*>(node));
		  break;
	  case ASTNodeType::WriteStatement:
		  executeWriteStatement(static_cast<ASTWriteStatement*>(node));
		  break;
	  case ASTNodeType::ReadStatement:
		  executeReadStatement(static_cast<ASTReadStatement*>(node));
		  break;
	  default:
		  throw QString("Unknown statement type");
	}
}

void PostfixEvaluator::executeCompoundStatement(ASTCompoundStatement *compound)
{
	for (auto &stmt : compound->statements)
		executeStatement(stmt.get());
}

void PostfixEvaluator::executeAssignment(ASTAssignment *assign)
{
	QVariant value = evaluateExpression(assign->expression.get());
	m_symbols.setValue(assign->name, value);
}

void PostfixEvaluator::executeIfStatement(ASTIfStatement *ifStmt)
{
	QVariant condition = evaluateExpression(ifStmt->condition.get());
	
	if (condition.toBool()) executeStatement(ifStmt->thenBranch.get());
	else if (ifStmt->elseBranch)executeStatement(ifStmt->elseBranch.get());
}

void PostfixEvaluator::executeWriteStatement(ASTWriteStatement *writeStmt)
{
	QString output;
	
	for (auto &arg : writeStmt->arguments) 
	{
		QVariant value = evaluateExpression(arg.get());
		output += variantToString(value);
	}
	
	if (writeStmt->writeln) output += "\n";
	
	if (m_output) m_output(output);
}

void PostfixEvaluator::executeReadStatement(ASTReadStatement *readStmt)
{
	for (const QString &varName : readStmt->variables) 
	{
		SymbolInfo *info = m_symbols.find(varName);
		if (!info) throw QString("Variable '%1' not declared").arg(varName);
		
		if (m_input) 
		{
			QString input = m_input();
			if (m_output) m_output(input + "\n");

			bool ok = false;
			
			switch (info->type) 
			{
				case DataType::Integer:
					info->value = input.toInt(&ok);
					break;
				case DataType::Double:
					info->value = input.toDouble(&ok);
					break;
				default:
					info->value = input;
					ok = true;
			}
			
			if (!ok) throw QString("Invalid input for variable '%1'").arg(varName);
		}
	}
}

QVariant PostfixEvaluator::evaluateExpression(ASTNode *expr)
{
	if (!expr) return QVariant();
	
	switch (expr->nodeType()) 
	{
		case ASTNodeType::NumberLiteral: 
		{
			ASTNumberLiteral *num = static_cast<ASTNumberLiteral*>(expr);
			return num->value;
		}
		case ASTNodeType::StringLiteral: 
		{
			ASTStringLiteral *str = static_cast<ASTStringLiteral*>(expr);
			return str->value;
		}
		case ASTNodeType::Identifier: 
		{
			ASTIdentifier *id = static_cast<ASTIdentifier*>(expr);
			SymbolInfo *info = m_symbols.find(id->name);
			if (!info) throw QString("Variable '%1' not declared").arg(id->name);
			return info->value;
		}
		case ASTNodeType::BinaryOp: 
		{
			ASTBinaryOp *binOp = static_cast<ASTBinaryOp*>(expr);
			QVariant left = evaluateExpression(binOp->left.get());
			QVariant right = evaluateExpression(binOp->right.get());
			return evaluateBinaryOp(binOp->op, left, right);
		}
		case ASTNodeType::UnaryOp: 
		{
			ASTUnaryOp *unOp = static_cast<ASTUnaryOp*>(expr);
			QVariant operand = evaluateExpression(unOp->operand.get());
			return evaluateUnaryOp(unOp->op, operand);
		}
		default:
			throw QString("Unknown expression type");
	}
}

QVariant PostfixEvaluator::evaluateBinaryOp(const QString &op, const QVariant &left, const QVariant &right)
{
	if (op == "+") {
		if (left.type() == QVariant::String || right.type() == QVariant::String) 
			return left.toString() + right.toString();
		return left.toDouble() + right.toDouble();
	} else if (op == "/") {
		double r = right.toDouble();
		if (r == 0) 			throw QString("Division by zero");
		return left.toDouble() / r;
	} else if (op == "div") {
		int r = right.toInt();
		if (r == 0) 			throw QString("Division by zero");
		return left.toInt() / r;
	} else if (op == "mod") {
		int r = right.toInt();
		if (r == 0)				throw QString("Modulo by zero");
		return left.toInt() % r;
	} else if (op == "=")		return left == right;
		else if (op == "-") 	return left.toDouble() - right.toDouble();
		else if (op == "*") 	return left.toDouble() * right.toDouble();
		else if (op == "<>")	return left != right;
	 	else if (op == "<")		return left.toDouble() < right.toDouble();
	 	else if (op == "<=")	return left.toDouble() <= right.toDouble();
	 	else if (op == ">")		return left.toDouble() > right.toDouble();
	 	else if (op == ">=")	return left.toDouble() >= right.toDouble();
	 	else if (op == "and")	return left.toBool() && right.toBool();
		else if (op == "or") 	return left.toBool() || right.toBool();
	
	throw QString("Unknown operator: %1").arg(op);
}

QVariant PostfixEvaluator::evaluateUnaryOp(const QString &op, const QVariant &operand)
{
	if (op == "+") 				return operand;
	else if (op == "-") 	return -operand.toDouble();
	else if (op == "not") return !operand.toBool();
	
	throw QString("Unknown unary operator: %1").arg(op);
}

QString PostfixEvaluator::variantToString(const QVariant &var)
{
	if (var.type() == QVariant::Bool) 					return var.toBool() ? "true" : "false";
	else if (var.type() == QVariant::Double)		return QString::number(var.toDouble(), 'g', 6);
	else if (var.type() == QVariant::Int) 			return QString::number(var.toInt());
																							return var.toString();
}