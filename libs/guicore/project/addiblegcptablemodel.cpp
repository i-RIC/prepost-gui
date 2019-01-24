#include "addiblegcptablemodel.h"
#include "gcptablerow.h"

AddibleGcpTableModel::AddibleGcpTableModel() :
	GcpTableModel {}
{}

int AddibleGcpTableModel::rowCount(const QModelIndex &parent) const
{
	return GcpTableModel::rowCount(parent) + 1;
}

QVariant AddibleGcpTableModel::data(const QModelIndex &index, int role) const
{
	if(! index.isValid()) {return QVariant {};}
	if(m_gcpTable == nullptr) {return QVariant {};}
	if(role != Qt::DisplayRole && role != Qt::EditRole) {return QVariant {};}
	if (index.row() == GcpTableModel::rowCount()) {
		// For the bottom (new) row.
		return QVariant {};
	}

	return GcpTableModel::data(index, role);
}

bool AddibleGcpTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (! index.isValid()) {return false;}
	if (role != Qt::EditRole && role != Qt::CheckStateRole) {return false;}
	if (m_gcpTable == nullptr) {return false;}
	if (index.row() == GcpTableModel::rowCount()) {

		auto row = GcpTableRow {};
		setRowData(&row, index, value);

		beginInsertRows(QModelIndex(), index.row(), index.row());
		m_gcpTable->push_back(row);
		endInsertRows();
		return true;
	}

	return GcpTableModel::setData(index, value, role);
}

void AddibleGcpTableModel::addRow(const GcpTableRow& row)
{
	beginInsertRows(QModelIndex(), GcpTableModel::rowCount(), GcpTableModel::rowCount());
	m_gcpTable->push_back(row);
	endInsertRows();
}
