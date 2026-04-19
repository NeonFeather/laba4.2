#pragma once

#include <QObject>
#include <QProcess>
#include <QString>

class SymbolTable; 

class Executor : public QObject
{
	Q_OBJECT

public:
	explicit Executor(QObject *parent = nullptr);
	~Executor();

	void execute(const QString &code);
	void stop();
	bool isRunning() const { return m_isRunning; }

public slots:
  void provideInput(const QString &input);
signals:
	void started();
	void finished(int exitCode);
	void errorOccurred(const QString &error);
	void outputReceived(const QString &output);
	void errorReceived(const QString &error);
	void inputRequested();
	void variablesUpdated(const SymbolTable *symbols);



private:
	bool m_isRunning = false;
	bool m_waitingForInput = false;
	QString m_pendingInput;
};
