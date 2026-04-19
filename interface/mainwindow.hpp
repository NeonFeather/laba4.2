#pragma once

#include <QMainWindow>
#include <memory>

#include <QSplitter>
#include <QPlainTextEdit>
#include <QToolBar>
#include <QLabel>
#include <QProgressBar>
#include <QAction>

#include "syntax_highlighter.hpp"
#include "variable_panel.hpp"

class ConsoleWidget;
class Executor;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void openFile();
    bool saveFile();
    bool saveFileAs();
    void executeCode();
    void stopExecution();
    void showAST(); 
    
    // Обработчики состояния выполнения
    void onExecutionStarted();
    void onExecutionFinished(int exitCode);
    void onExecutionError(const QString &error);
    void onInputRequested();

private:
    void setupUi();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void updateWindowTitle();
    bool maybeSave();
    void highlightError(int line, int column, const QString &message);
    void clearErrorHighlight();
    QString generateASTString(ASTNode *node, int indent = 0);
    
    // UI компоненты
    QPlainTextEdit *m_editor;
    ConsoleWidget *m_console;
    QSplitter *m_splitter;
    VariablePanel *m_variablePanel; 
    QSplitter *m_horizontalSplitter; 
    
    // Actions
    QAction *m_newAction;
    QAction *m_openAction;
    QAction *m_saveAction;
    QAction *m_saveAsAction;
    QAction *m_exitAction;
    QAction *m_runAction;
    QAction *m_stopAction;
    
    
    // Состояние
    QString m_currentFile;
    bool m_isModified;
    
    // Статус бар
    QLabel *m_statusLabel;
    QProgressBar *m_progressBar;
    
    // Исполнитель
    std::unique_ptr<Executor> m_executor;

    SyntaxHighlighter *m_highlighter;
};
