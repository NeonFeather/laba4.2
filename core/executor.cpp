#include "executor.hpp"
#include "parser/parser.hpp"
#include "evaluator/postfix_evaluator.hpp"
#include <QCoreApplication>
#include <QEventLoop>

Executor::Executor(QObject *parent): QObject(parent), m_isRunning(false), m_waitingForInput(false){}

Executor::~Executor(){stop();}

void Executor::execute(const QString &code)
{
	if (m_isRunning)
		stop();
	
	Parser parser;
	
	if (!parser.parse(code)) 
	{
		emit errorReceived("Syntax error: " + parser.errorString() + "\n");
		emit finished(1);
		return;
	}
	
	ASTProgram *program = parser.getAST();
	if (!program) 
	{
		emit errorReceived("Failed to build AST\n");
		emit finished(1);
		return;
	}
	
	m_isRunning = true;
	emit started();
	
	PostfixEvaluator evaluator;
	
	// Вывод сразу в консоль
	evaluator.setOutputCallback([this](const QString &text) 
	{
		emit outputReceived(text);
	});
	
	// Ввод через сигнал
	evaluator.setInputCallback([this]() -> QString 
	{
		m_waitingForInput = true;
		emit inputRequested();
		
		// Ждём пока не придёт ввод
		while (m_waitingForInput)
			QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
		
		return m_pendingInput;
	});
	
	int result = evaluator.evaluate(program);
	emit variablesUpdated(evaluator.getSymbolTable());
	
	m_isRunning = false;
	emit finished(result);
}

void Executor::stop()
{
	m_isRunning = false;
	m_waitingForInput = false;
}

void Executor::provideInput(const QString &input)
{
	m_pendingInput = input;
	m_waitingForInput = false;
}