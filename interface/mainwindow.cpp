#include "mainwindow.hpp"
#include "consolewidget.hpp"
#include "core/executor.hpp"
#include "theme/theme.hpp"

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
	
	m_console->clearConsole();
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
	else m_executor->provideInput("0");
}