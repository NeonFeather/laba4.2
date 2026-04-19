#pragma once

#include <QString>
#include <QVariant>
#include <vector>
#include <memory>



enum class ASTNodeType {
	Program,
	ConstDeclaration,
	VarDeclaration,
	Assignment,
	IfStatement,
	WriteStatement,
	ReadStatement,
	CompoundStatement,
	BinaryOp,
	UnaryOp,
	Identifier,
	NumberLiteral,
	StringLiteral,
	TypeDeclaration
};

// Типы данных
enum class DataType {
	Integer,
	Double,
	Boolean,
	String,
	Unknown
};

// Базовый класс для всех узлов AST
class ASTNode {
  public:
	virtual ~ASTNode() = default;
	virtual ASTNodeType nodeType() const = 0;
	int line = 0;
	int column = 0;
};

// Программа
class ASTProgram : public ASTNode {
  public:
	  ASTNodeType nodeType() const override { return ASTNodeType::Program; }
	  std::vector<std::unique_ptr<ASTNode>> declarations; // const + var
	  std::unique_ptr<ASTNode> body; // compound statement
};

// Объявление константы
class ASTConstDecl : public ASTNode {
	public:
		ASTNodeType nodeType() const override { return ASTNodeType::ConstDeclaration; }
		QString name;
		DataType type;
		QVariant value;
};

// Объявление переменной
class ASTVarDecl : public ASTNode {
	public:
		ASTNodeType nodeType() const override { return ASTNodeType::VarDeclaration; }
		std::vector<QString> names;
		DataType type;
};

// Begin, end
class ASTCompoundStatement : public ASTNode {
	public:
		ASTNodeType nodeType() const override { return ASTNodeType::CompoundStatement; }
		std::vector<std::unique_ptr<ASTNode>> statements;
};

// Присваивание
class ASTAssignment : public ASTNode {
	public:
		ASTNodeType nodeType() const override { return ASTNodeType::Assignment; }
		QString name;
		std::unique_ptr<ASTNode> expression;
};

// Условный оператор
class ASTIfStatement : public ASTNode {
	public:
		ASTNodeType nodeType() const override { return ASTNodeType::IfStatement; }
		std::unique_ptr<ASTNode> condition;
		std::unique_ptr<ASTNode> thenBranch;
		std::unique_ptr<ASTNode> elseBranch;
};

// Оператор Write
class ASTWriteStatement : public ASTNode {
	public:
		ASTNodeType nodeType() const override { return ASTNodeType::WriteStatement; }
		std::vector<std::unique_ptr<ASTNode>> arguments;
		bool writeln = false;
};

// Оператор Read
class ASTReadStatement : public ASTNode {
	public:
		ASTNodeType nodeType() const override { return ASTNodeType::ReadStatement; }
		std::vector<QString> variables;
};

// Бинарная операция
class ASTBinaryOp : public ASTNode {
	public:
		ASTNodeType nodeType() const override { return ASTNodeType::BinaryOp; }
		QString op; // +, -, *, /, div, mod, =, <>, <, <=, >, >=, and, or
		std::unique_ptr<ASTNode> left;
		std::unique_ptr<ASTNode> right;
};

// Унарная операция
class ASTUnaryOp : public ASTNode {
	public:
		ASTNodeType nodeType() const override { return ASTNodeType::UnaryOp; }
		QString op; // +, -, not
		std::unique_ptr<ASTNode> operand;
};

// Идентификатор
class ASTIdentifier : public ASTNode {
	public:
		ASTNodeType nodeType() const override { return ASTNodeType::Identifier; }
		QString name;
};

// Числовой литерал
class ASTNumberLiteral : public ASTNode {
	public:
		ASTNodeType nodeType() const override { return ASTNodeType::NumberLiteral; }
		QVariant value;
		DataType type;
};

// Строковый литерал
class ASTStringLiteral : public ASTNode {
	public:
		ASTNodeType nodeType() const override { return ASTNodeType::StringLiteral; }
		QString value;
};
