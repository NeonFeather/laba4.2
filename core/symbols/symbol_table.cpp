#include "symbol_table.hpp"
#include <QDebug>

SymbolTable::SymbolTable(){}

bool SymbolTable::addConstant(const QString &name, DataType type, const QVariant &value)
{
	if (exists(name)) return false;
	m_symbols[name] = SymbolInfo(name, type, true, value);
	return true;
}

bool SymbolTable::addVariable(const QString &name, DataType type)
{
	if (exists(name)) return false;
	m_symbols[name] = SymbolInfo(name, type, false);
	return true;
}

bool SymbolTable::exists(const QString &name) const{return m_symbols.contains(name);}

SymbolInfo* SymbolTable::find(const QString &name)
{
	auto it = m_symbols.find(name);
	if (it != m_symbols.end())
		return &it.value();
	return nullptr;
}

const SymbolInfo* SymbolTable::find(const QString &name) const
{
	auto it = m_symbols.find(name);
	if (it != m_symbols.end())
		return &it.value();
	return nullptr;
}

void SymbolTable::setValue(const QString &name, const QVariant &value)
{
	auto it = m_symbols.find(name);
	if (it != m_symbols.end() && !it->isConstant)
		it->value = value;
}

QVariant SymbolTable::getValue(const QString &name) const
{
	auto it = m_symbols.find(name);
	if (it != m_symbols.end())
		return it->value;
	return QVariant();
}

void SymbolTable::clear() {m_symbols.clear();}

void SymbolTable::print() const
{
	qDebug() << "Symbol Table:";
	for (auto it = m_symbols.begin(); it != m_symbols.end(); ++it) {
		qDebug() << "  " << it->name 
				 << (it->isConstant ? "(const)" : "(var)")
				 << "=" << it->value;
	}
}