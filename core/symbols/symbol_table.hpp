#pragma once

#include <QHash>
#include <QString>
#include <QVariant>
#include "ast/ast_node.hpp"

struct SymbolInfo {
    QString name;
    DataType type;
    bool isConstant;
    QVariant value;
    
    SymbolInfo() : type(DataType::Unknown), isConstant(false) {}
    SymbolInfo(const QString &n, DataType t, bool constant, const QVariant &v = QVariant())
        : name(n), type(t), isConstant(constant), value(v) {}
};

class SymbolTable {
public:
    SymbolTable();
    
    bool addConstant(const QString &name, DataType type, const QVariant &value);
    bool addVariable(const QString &name, DataType type);
    bool exists(const QString &name) const;
    SymbolInfo* find(const QString &name);
    const SymbolInfo* find(const QString &name) const;
    
    void setValue(const QString &name, const QVariant &value);
    QVariant getValue(const QString &name) const;
    
    void clear();
    void print() const;
    const QHash<QString, SymbolInfo>& getAllSymbols() const { return m_symbols; }
    
private:
    QHash<QString, SymbolInfo> m_symbols;
};
