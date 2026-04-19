#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include "symbols/symbol_table.hpp"

class VariablePanel : public QWidget
{
    Q_OBJECT

public:
    explicit VariablePanel(QWidget *parent = nullptr);
    
public slots:
    void updateVariables(const SymbolTable *symbolTable);
    void clear();

private:
    QTableWidget *m_table;
    QLabel *m_titleLabel;
    
    QString hashString(const QString &str);
};
