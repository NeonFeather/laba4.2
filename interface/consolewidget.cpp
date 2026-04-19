#include "consolewidget.hpp"
#include "theme/theme.hpp"
#include <QKeyEvent>
#include <QMenu>
#include <QAction>
#include <QTextCursor>
#include <QScrollBar>

ConsoleWidget::ConsoleWidget(QWidget *parent): QPlainTextEdit(parent)
{
    setReadOnly(true);
    setLineWrapMode(QPlainTextEdit::NoWrap);
    setFont(Theme::Fonts::getConsoleFont());
    
    setupFormats();
    
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, Theme::Colors::ConsoleBackground);
    palette.setColor(QPalette::Text, Theme::Colors::TextPrimary);
    setPalette(palette);
    
    setStyleSheet(Theme::StyleSheets::getConsoleStyle());
}

void ConsoleWidget::setupFormats()
{
    m_normalFormat.setForeground(Theme::Colors::ConsoleOutput);
    m_errorFormat.setForeground(Theme::Colors::ConsoleError);
    m_warningFormat.setForeground(Theme::Colors::ConsoleWarning);
    m_successFormat.setForeground(Theme::Colors::ConsoleSuccess);
    m_infoFormat.setForeground(Theme::Colors::ConsoleInfo);
    m_debugFormat.setForeground(Theme::Colors::ConsoleDebug);
}

QTextCharFormat ConsoleWidget::getFormatForType(MessageType type)
{
    switch (type) {
      case MessageType::Error:
          return m_errorFormat;
      case MessageType::Warning:
          return m_warningFormat;
      case MessageType::Success:
          return m_successFormat;
      case MessageType::Info:
          return m_infoFormat;
      case MessageType::Debug:
          return m_debugFormat;
      default:
          return m_normalFormat;
    }
}

void ConsoleWidget::appendMessage(const QString &text, MessageType type)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(text, getFormatForType(type));
    
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
    ensureCursorVisible();
}

void ConsoleWidget::appendOutput(const QString &text) {appendMessage(text, MessageType::Normal);}
void ConsoleWidget::appendError(const QString &text) {appendMessage(text, MessageType::Error);}
void ConsoleWidget::appendWarning(const QString &text) {appendMessage(text, MessageType::Warning);}
void ConsoleWidget::appendSuccess(const QString &text) {appendMessage(text, MessageType::Success);}
void ConsoleWidget::appendInfo(const QString &text) {appendMessage(text, MessageType::Info);}
void ConsoleWidget::appendDebug(const QString &text) {appendMessage(text, MessageType::Debug);}
void ConsoleWidget::clearConsole() {clear();}

void ConsoleWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Copy)) QPlainTextEdit::keyPressEvent(event); 
    else QPlainTextEdit::keyPressEvent(event);
}

void ConsoleWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();
    menu->addSeparator();
    
    QAction *clearAction = menu->addAction("Clear Console");
    connect(clearAction, &QAction::triggered, this, &ConsoleWidget::clearConsole);
    
    menu->exec(event->globalPos());
    delete menu;
}