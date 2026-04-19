#pragma once

#include <QPlainTextEdit>
#include <QTextCharFormat>

class ConsoleWidget : public QPlainTextEdit
{
	Q_OBJECT

public:
	enum class MessageType {
		Normal,
		Error,
		Warning,
		Success,
		Info,
		Debug
	};

	explicit ConsoleWidget(QWidget *parent = nullptr);

public slots:
	void appendMessage(const QString &text, MessageType type = MessageType::Normal);
	void appendOutput(const QString &text);
	void appendError(const QString &text);
	void appendWarning(const QString &text);
	void appendSuccess(const QString &text);
	void appendInfo(const QString &text);
	void appendDebug(const QString &text);
	void clearConsole();

protected:
	void keyPressEvent(QKeyEvent *event) override;
	void contextMenuEvent(QContextMenuEvent *event) override;

private:
	void setupFormats();
	QTextCharFormat getFormatForType(MessageType type);

	QTextCharFormat m_normalFormat;
	QTextCharFormat m_errorFormat;
	QTextCharFormat m_warningFormat;
	QTextCharFormat m_successFormat;
	QTextCharFormat m_infoFormat;
	QTextCharFormat m_debugFormat;
};
