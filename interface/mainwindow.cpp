#include "mainwindow.hpp"
#include "consolewidget.hpp"
#include "core/executor.hpp"
#include "theme/theme.hpp"
#include "core/parser/parser.hpp"

#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QSplitter>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QTextStream>
#include <QSettings>
#include <QProgressBar>
#include <QLabel>
#include <QAction>
#include <QKeySequence>
#include <QFontDatabase>
#include <QInputDialog>
#include <QFontMetrics>
#include <QTimer>
#include <QDebug>
#include <QRegularExpression>
#include <QVBoxLayout>
#include <QDialog>
#include <QPushButton>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
	m_editor(new QPlainTextEdit(this)),
	m_console(new ConsoleWidget(this)),
	m_splitter(new QSplitter(Qt::Vertical, this)),
	m_isModified(false),
	m_executor(std::make_unique<Executor>(this)),
	m_highlighter(new SyntaxHighlighter(m_editor->document()))
{
	setupUi();
	setupMenuBar();
	setupToolBar();
	setupStatusBar();
	
	setStyleSheet(
		Theme::StyleSheets::getMainWindowStyle() +
		Theme::StyleSheets::getToolBarStyle() +
		Theme::StyleSheets::getStatusBarStyle()
	);
	
  
	//Применение темы
	m_editor->setFont(Theme::Fonts::getEditorFont());
	m_editor->setLineWrapMode(QPlainTextEdit::NoWrap);
	m_editor->setStyleSheet(Theme::StyleSheets::getEditorStyle());
  QFontMetrics metrics(m_editor->font());
  m_editor->setTabStopDistance(metrics.horizontalAdvance(' ') * 4);
	
	QPalette editorPalette = m_editor->palette();
	editorPalette.setColor(QPalette::Base, Theme::Colors::EditorBackground);
	editorPalette.setColor(QPalette::Text, Theme::Colors::TextPrimary);
	m_editor->setPalette(editorPalette);
	
	m_splitter->setStyleSheet(Theme::StyleSheets::getSplitterStyle());
	m_splitter->setHandleWidth(2);
	
	// Подключение сигнала
	connect(m_editor, &QPlainTextEdit::textChanged, this, [this]() {
		m_isModified = true;
		updateWindowTitle();
	});

	//Консоль
	connect(m_executor.get(), &Executor::started, this, &MainWindow::onExecutionStarted);
	connect(m_executor.get(), &Executor::finished, this, &MainWindow::onExecutionFinished);
	connect(m_executor.get(), &Executor::errorOccurred, this, &MainWindow::onExecutionError);
	connect(m_executor.get(), &Executor::outputReceived, m_console, &ConsoleWidget::appendOutput);
	connect(m_executor.get(), &Executor::errorReceived, m_console, &ConsoleWidget::appendError);
	connect(m_executor.get(), &Executor::inputRequested, this, &MainWindow::onInputRequested);
	connect(m_executor.get(), &Executor::variablesUpdated, 
        m_variablePanel, &VariablePanel::updateVariables);
	// Размеры
	QSettings settings;
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("windowState").toByteArray());
	m_splitter->restoreState(settings.value("splitterState").toByteArray());
	
	updateWindowTitle();
}

MainWindow::~MainWindow()
{
	QSettings settings;
	settings.setValue("geometry", saveGeometry());
	settings.setValue("windowState", saveState());
	settings.setValue("splitterState", m_splitter->saveState());
}

void MainWindow::setupUi()
{
	setWindowTitle("Pascal IDE");
	setMinimumSize(1000, 700);
	resize(1200, 800);

	m_variablePanel = new VariablePanel(this);

  QSplitter *editorSplitter = new QSplitter(Qt::Vertical);
  editorSplitter->addWidget(m_editor);
  editorSplitter->addWidget(m_console);
  editorSplitter->setSizes({500, 200});
  editorSplitter->setChildrenCollapsible(false);

  m_horizontalSplitter = new QSplitter(Qt::Horizontal);
  m_horizontalSplitter->addWidget(editorSplitter);
  m_horizontalSplitter->addWidget(m_variablePanel);
  m_horizontalSplitter->setSizes({800, 300});
  m_horizontalSplitter->setChildrenCollapsible(false);
	
	setCentralWidget(m_horizontalSplitter);
  
  editorSplitter->setStyleSheet(Theme::StyleSheets::getSplitterStyle());
  m_horizontalSplitter->setStyleSheet(Theme::StyleSheets::getSplitterStyle());

  m_editor -> setFocus();
}

void MainWindow::setupMenuBar()
{
	// Меню File
	QMenu *fileMenu = menuBar()->addMenu("&File");
	
	// Исправленный порядок аргументов: текст, shortcut, объект, слот
	m_newAction = fileMenu->addAction("&New", QKeySequence::New, this, &MainWindow::newFile);
	m_openAction = fileMenu->addAction("&Open...", QKeySequence::Open, this, &MainWindow::openFile);
	m_saveAction = fileMenu->addAction("&Save", QKeySequence::Save, this, &MainWindow::saveFile);
	m_saveAsAction = fileMenu->addAction("Save &As...", QKeySequence::SaveAs, this, &MainWindow::saveFileAs);
	
	fileMenu->addSeparator();
	m_exitAction = fileMenu->addAction("E&xit", QKeySequence::Quit, qApp, &QApplication::closeAllWindows);
	
	// Меню Edit
	QMenu *editMenu = menuBar()->addMenu("&Edit");
	editMenu->addAction("&Undo", QKeySequence::Undo, m_editor, &QPlainTextEdit::undo);
	editMenu->addAction("&Redo", QKeySequence::Redo, m_editor, &QPlainTextEdit::redo);
	editMenu->addSeparator();
	editMenu->addAction("Cu&t", QKeySequence::Cut, m_editor, &QPlainTextEdit::cut);
	editMenu->addAction("&Copy", QKeySequence::Copy, m_editor, &QPlainTextEdit::copy);
	editMenu->addAction("&Paste", QKeySequence::Paste, m_editor, &QPlainTextEdit::paste);
	
	// Меню Run
	QMenu *runMenu = menuBar()->addMenu("&Run");
	m_runAction = runMenu->addAction("&Run", Qt::Key_F5, this, &MainWindow::executeCode);
	m_stopAction = runMenu->addAction("&Stop", Qt::Key_Escape, this, &MainWindow::stopExecution);
	m_stopAction->setEnabled(false);
	
  QMenu *viewMenu = menuBar()->addMenu("&View");
  viewMenu->addAction("Show &AST", this, &MainWindow::showAST);
}

void MainWindow::setupToolBar()
{
	QToolBar *toolBar = addToolBar("Main Toolbar");
	toolBar->setMovable(false);
	
	toolBar->addAction(m_newAction);
	toolBar->addAction(m_openAction);
	toolBar->addAction(m_saveAction);
	toolBar->addSeparator();
	toolBar->addAction(m_runAction);
	toolBar->addAction(m_stopAction);
}

void MainWindow::setupStatusBar()
{
	m_statusLabel = new QLabel("Готово", this);
	m_progressBar = new QProgressBar(this);
	m_progressBar->setVisible(false);
	m_progressBar->setMaximumSize(100, 20);
	
	statusBar()->addPermanentWidget(m_progressBar);
	statusBar()->addWidget(m_statusLabel);
}

void MainWindow::updateWindowTitle()
{
	QString title = "Pascal IDE";
	if (!m_currentFile.isEmpty()) {
		title += " - " + QFileInfo(m_currentFile).fileName();
	} else {
		title += " - Untitled";
	}
	
	if (m_isModified) {
		title += " *";
	}
	
	setWindowTitle(title);
}

bool MainWindow::maybeSave()
{
	if (!m_isModified) {
		return true;
	}
	
	QMessageBox::StandardButton ret = QMessageBox::warning(
		this, "Pascal IDE",
		"Документ был изменен. Сохранить изменения?",
		QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
	);
	
	switch (ret) {
	case QMessageBox::Save:
		return saveFile();
	case QMessageBox::Cancel:
		return false;
	default:
		break;
	}
	return true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (maybeSave()) event->accept();
	else event->ignore();
}

void MainWindow::newFile()
{
	if (maybeSave()) {
		m_editor->clear();
		m_currentFile.clear();
		m_isModified = false;
		updateWindowTitle();
	}
}

void MainWindow::openFile()
{
	if (!maybeSave()) return;

	QString fileName = QFileDialog::getOpenFileName(
		this, "Open File", QString(),
		"Pascal Files (*.pas);;Text Files (*.txt);;All Files (*)"
	);
	
	if (fileName.isEmpty()) return;

	
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::warning(this, "Error", "Не удалось открыть файл: " + file.errorString());
		return;
	}
	
	QTextStream stream(&file);
	m_editor->setPlainText(stream.readAll());
	
	m_currentFile = fileName;
	m_isModified = false;
	updateWindowTitle();
	
	m_console->appendInfo("Загружен файл: " + fileName + "\n");
}

bool MainWindow::saveFile()
{
	if (m_currentFile.isEmpty()) return saveFileAs();
	
	QFile file(m_currentFile);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::warning(this, "Error", "Не удалось сохранить файл: " + file.errorString());
		return false;
	}
	
	QTextStream stream(&file);
	stream << m_editor->toPlainText();
	
	m_isModified = false;
	updateWindowTitle();
	
	m_console->appendSuccess("Файл сохранен: " + m_currentFile + "\n");
	return true;
}

bool MainWindow::saveFileAs()
{
	QString fileName = QFileDialog::getSaveFileName(
		this, "Save File As", QString(),
		"Pascal Files (*.pas);;Text Files (*.txt);;All Files (*)"
	);
	
	if (fileName.isEmpty()) {
		return false;
	}
	
	m_currentFile = fileName;
	return saveFile();
}

void MainWindow::executeCode()
{
	if (m_editor->toPlainText().trimmed().isEmpty()) {
		m_console->appendWarning("Код для выполнения отсутствует.\n");
		return;
	}

	clearErrorHighlight();
	m_console->clearConsole();

  Parser testParser;
    if (!testParser.parse(m_editor->toPlainText())) {
        QString error = testParser.errorString();
        m_console->appendError("Syntax error: " + error + "\n");
        
        // Пытаемся извлечь номер строки из ошибки
        // Формат ошибки: "... at X:Y"
        QRegularExpression re("at (\\d+):(\\d+)");
        QRegularExpressionMatch match = re.match(error);
        if (match.hasMatch()) {
            int line = match.captured(1).toInt();
            int column = match.captured(2).toInt();
            highlightError(line, column, error);
        }
        return;
    }

	m_console->appendInfo("Код выполняется...\n");
	m_console->appendInfo("----------------------------------------\n");
	
	m_executor->execute(m_editor->toPlainText());
}

void MainWindow::stopExecution()
{
	m_executor->stop();
}

void MainWindow::onExecutionStarted()
{
	m_runAction->setEnabled(false);
	m_stopAction->setEnabled(true);
	m_statusLabel->setText("Запуск...");
	m_progressBar->setVisible(true);
	m_progressBar->setRange(0, 0);
}

void MainWindow::onExecutionFinished(int exitCode)
{
	m_runAction->setEnabled(true);
	m_stopAction->setEnabled(false);
	m_statusLabel->setText("Готово");
	m_progressBar->setVisible(false);
	
	m_console->appendInfo("\n----------------------------------------\n");
	if (exitCode == 0) m_console->appendSuccess(QString("Выполнение завершено успешно (код выхода: %1)\n").arg(exitCode));
	else m_console->appendError(QString("Выполнение не удалось с кодом выхода: %1\n").arg(exitCode));
}

void MainWindow::onExecutionError(const QString &error)
{
	m_console->appendError("Ошибка выполнения: " + error + "\n");
}
void MainWindow::onInputRequested()
{
	bool ok;
	QString input = QInputDialog::getText(this, "Input Required", 
	  "Enter value:", QLineEdit::Normal, "", &ok);
	
	if (ok) m_executor->provideInput(input);
	else throw std::runtime_error("Input cancelled by user");
}
void MainWindow::highlightError(int line, int column, const QString &message)
{
  clearErrorHighlight();
  
  QTextCursor cursor(m_editor->document());
  cursor.movePosition(QTextCursor::Start);
  cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line - 1);
  cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
  
  QTextCharFormat errorFormat;
  errorFormat.setBackground(QColor(239, 83, 80, 100));  
  errorFormat.setUnderlineColor(Theme::Colors::ConsoleError);
  errorFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);
  cursor.mergeCharFormat(errorFormat);
  
  m_statusLabel->setText("Error: " + message);
}

void MainWindow::clearErrorHighlight()
{
  QTextCursor cursor(m_editor->document());
  cursor.select(QTextCursor::Document);
  QTextCharFormat clearFormat;
  clearFormat.setBackground(Qt::transparent);
  clearFormat.setUnderlineStyle(QTextCharFormat::NoUnderline);
  cursor.mergeCharFormat(clearFormat);
}
void MainWindow::showAST()
{
  QString code = m_editor->toPlainText();
  if (code.trimmed().isEmpty()) {
      m_console->appendWarning("No code to parse.\n");
      return;
  }
  
  Parser parser;
  if (!parser.parse(code)) {
      m_console->appendError("Cannot generate AST: " + parser.errorString() + "\n");
      return;
  }
  
  ASTProgram *program = parser.getAST();
  if (!program) {
      m_console->appendError("Failed to build AST\n");
      return;
  }
  
  // Генерируем текстовое представление AST
  QString astText = generateASTString(program);
  
  // Показываем в отдельном диалоге
  QDialog *dialog = new QDialog(this);
  dialog->setWindowTitle("Abstract Syntax Tree");
  dialog->resize(600, 400);
  
  QVBoxLayout *layout = new QVBoxLayout(dialog);
  
  QPlainTextEdit *textEdit = new QPlainTextEdit(dialog);
  textEdit->setFont(Theme::Fonts::getConsoleFont());
  textEdit->setPlainText(astText);
  textEdit->setReadOnly(true);
  
  // Применяем тему
  textEdit->setStyleSheet(Theme::StyleSheets::getConsoleStyle());
  QPalette palette = textEdit->palette();
  palette.setColor(QPalette::Base, Theme::Colors::EditorBackground);
  palette.setColor(QPalette::Text, Theme::Colors::TextPrimary);
  textEdit->setPalette(palette);
  
  QPushButton *closeBtn = new QPushButton("Close", dialog);
  connect(closeBtn, &QPushButton::clicked, dialog, &QDialog::accept);
  
  layout->addWidget(textEdit);
  layout->addWidget(closeBtn);
  
  dialog->exec();
  delete dialog;
}

QString MainWindow::generateASTString(ASTNode *node, int indent)
{
  if (!node) return "";
  
  QString result;
  QString prefix(indent * 2, ' ');
  
  switch (node->nodeType()) {
  case ASTNodeType::Program: {
    auto *prog = static_cast<ASTProgram*>(node);
    result += prefix + "Program\n";
    result += prefix + "├─ Declarations (" + QString::number(prog->declarations.size()) + ")\n";
    for (auto &decl : prog->declarations) 
      result += generateASTString(decl.get(), indent + 1);
    result += prefix + "└─ Body\n";
    result += generateASTString(prog->body.get(), indent + 1);
    break;
  }
  case ASTNodeType::ConstDeclaration: {
    auto *decl = static_cast<ASTConstDecl*>(node);
    result += prefix + "├─ Const: " + decl->name + " = " + decl->value.toString() + "\n";
    break;
  }
  case ASTNodeType::VarDeclaration: {
    auto *decl = static_cast<ASTVarDecl*>(node);
    QString names;
    for (auto &name : decl->names) {
      if (!names.isEmpty()) names += ", ";
      names += name;
    }
    result += prefix + "├─ Var: " + names + " : " + 
             (decl->type == DataType::Integer ? "integer" : 
              decl->type == DataType::Double ? "double" : "other") + "\n";
    break;
  }
  case ASTNodeType::CompoundStatement: {
    auto *compound = static_cast<ASTCompoundStatement*>(node);
    result += prefix + "├─ Begin\n";
    for (auto &stmt : compound->statements) 
      result += generateASTString(stmt.get(), indent + 1);
    result += prefix + "└─ End\n";
    break;
  }
  case ASTNodeType::Assignment: {
    auto *assign = static_cast<ASTAssignment*>(node);
    result += prefix + "├─ Assign: " + assign->name + " :=\n";
    result += generateASTString(assign->expression.get(), indent + 1);
    break;
  }
  case ASTNodeType::IfStatement: {
    auto *ifStmt = static_cast<ASTIfStatement*>(node);
    result += prefix + "├─ If\n";
    result += prefix + "│  ├─ Condition\n";
    result += generateASTString(ifStmt->condition.get(), indent + 2);
    result += prefix + "│  ├─ Then\n";
    result += generateASTString(ifStmt->thenBranch.get(), indent + 2);
    if (ifStmt->elseBranch) {
      result += prefix + "│  └─ Else\n";
      result += generateASTString(ifStmt->elseBranch.get(), indent + 2);
    }
    break;
  }
  case ASTNodeType::WriteStatement: {
    auto *write = static_cast<ASTWriteStatement*>(node);
    result += prefix + "├─ Write" + QString(write->writeln ? "Ln" : "") + "\n";
    for (auto &arg : write->arguments) 
      result += generateASTString(arg.get(), indent + 1);
    break;
  }
  case ASTNodeType::ReadStatement: {
    auto *read = static_cast<ASTReadStatement*>(node);
    result += prefix + "├─ Read: ";
    for (auto &var : read->variables) 
      result += var + " ";
    result += "\n";
    break;
  }
  case ASTNodeType::BinaryOp: {
    auto *binOp = static_cast<ASTBinaryOp*>(node);
    result += prefix + "├─ BinaryOp: " + binOp->op + "\n";
    result += generateASTString(binOp->left.get(), indent + 1);
    result += generateASTString(binOp->right.get(), indent + 1);
    break;
  }
  case ASTNodeType::Identifier: {
    auto *id = static_cast<ASTIdentifier*>(node);
    result += prefix + "├─ ID: " + id->name + "\n";
    break;
  }
  case ASTNodeType::NumberLiteral: {
    auto *num = static_cast<ASTNumberLiteral*>(node);
    result += prefix + "├─ Number: " + num->value.toString() + "\n";
    break;
  }
  case ASTNodeType::StringLiteral: {
    auto *str = static_cast<ASTStringLiteral*>(node);
    result += prefix + "├─ String: \"" + str->value + "\"\n";
    break;
  }
  default:
    result += prefix + "├─ [Unknown node]\n";
  }
  
  return result;
}