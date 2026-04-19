#include "variable_panel.hpp"
#include "theme/theme.hpp"
#include <QCryptographicHash>
#include <QFontDatabase>

VariablePanel::VariablePanel(QWidget *parent): QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    m_titleLabel = new QLabel("📊 Variables", this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet(QString(
        "QLabel {"
        "    background-color: %1;"
        "    color: %2;"
        "    padding: 8px;"
        "    font-weight: bold;"
        "    font-size: 12px;"
        "    border-bottom: 1px solid %3;"
        "}"
    ).arg(
        Theme::Colors::SidebarBackground.name(),
        Theme::Colors::TextPrimary.name(),
        Theme::Colors::BorderLight.name()
    ));
    layout->addWidget(m_titleLabel);
    
    // Таблица
    m_table = new QTableWidget(this);
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({"Name", "Type", "Value", "Hash"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_table->setColumnWidth(0, 80);
    m_table->setColumnWidth(1, 60);
    m_table->setColumnWidth(2, 80);
    m_table->setColumnWidth(3, 100);
    m_table->verticalHeader()->setVisible(false);
    m_table->setAlternatingRowColors(true);
    m_table->setEditTriggers(QTableWidget::NoEditTriggers);
    m_table->setSelectionBehavior(QTableWidget::SelectRows);
    m_table->setStyleSheet(QString(
        "QTableWidget {"
        "    background-color: %1;"
        "    color: %2;"
        "    border: none;"
        "    gridline-color: %3;"
        "}"
        "QTableWidget::item {"
        "    padding: 4px;"
        "}"
        "QTableWidget::item:selected {"
        "    background-color: %4;"
        "}"
        "QHeaderView::section {"
        "    background-color: %5;"
        "    color: %2;"
        "    padding: 6px;"
        "    border: none;"
        "    border-bottom: 1px solid %3;"
        "    border-right: 1px solid %3;"
        "    font-weight: bold;"
        "}"
        "QTableWidget::item:alternate {"
        "    background-color: %6;"
        "}"
    ).arg(
        Theme::Colors::EditorBackground.name(),
        Theme::Colors::TextPrimary.name(),
        Theme::Colors::BorderDark.name(),
        Theme::Colors::SelectionBackground.name(),
        Theme::Colors::SidebarBackground.name(),
        Theme::Colors::CurrentLineHighlight.name()
    ));
    
    layout->addWidget(m_table);
    
    setMinimumWidth(300);
}

QString VariablePanel::hashString(const QString &str)
{
    QByteArray hash = QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Md5);
    return hash.toHex().left(8); // Первые 8 символов MD5 хеша
}

void VariablePanel::updateVariables(const SymbolTable *symbolTable)
{
    if (!symbolTable) {
        clear();
        return;
    }
    
    // Получаем все символы
    auto symbols = symbolTable->getAllSymbols();
    
    m_table->setRowCount(symbols.size());
    
    int row = 0;
    for (auto it = symbols.begin(); it != symbols.end(); ++it, ++row) {
        const SymbolInfo &info = it.value();
        
        // Имя
        QTableWidgetItem *nameItem = new QTableWidgetItem(info.name);
        nameItem->setForeground(info.isConstant ? Theme::Colors::Keyword : Theme::Colors::Variable);
        m_table->setItem(row, 0, nameItem);
        
        // Тип
        QString typeStr;
        switch (info.type) {
        case DataType::Integer: typeStr = "integer"; break;
        case DataType::Double: typeStr = "double"; break;
        case DataType::Boolean: typeStr = "boolean"; break;
        case DataType::String: typeStr = "string"; break;
        default: typeStr = "unknown";
        }
        QTableWidgetItem *typeItem = new QTableWidgetItem(typeStr);
        typeItem->setForeground(Theme::Colors::Type);
        m_table->setItem(row, 1, typeItem);
        
        // Значение
        QString valueStr;
        if (info.value.isValid()) {
            if (info.type == DataType::Boolean) {
                valueStr = info.value.toBool() ? "true" : "false";
            } else if (info.type == DataType::Double) {
                valueStr = QString::number(info.value.toDouble(), 'g', 4);
            } else {
                valueStr = info.value.toString();
            }
        } else {
            valueStr = "—";
        }
        QTableWidgetItem *valueItem = new QTableWidgetItem(valueStr);
        valueItem->setForeground(Theme::Colors::Number);
        m_table->setItem(row, 2, valueItem);
        
        // Хеш
        QString hashInput = info.name + ":" + typeStr + ":" + valueStr;
        QString hash = hashString(hashInput);
        QTableWidgetItem *hashItem = new QTableWidgetItem(hash);
        hashItem->setForeground(Theme::Colors::Comment);
        hashItem->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
        m_table->setItem(row, 3, hashItem);
    }
    
    m_titleLabel->setText(QString("📊 Variables (%1)").arg(symbols.size()));
}

void VariablePanel::clear()
{
    m_table->setRowCount(0);
    m_titleLabel->setText("📊 Variables (0)");
}