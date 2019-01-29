#include "gcptablemodel.h"
#include "gcptablerow.h"

namespace {
const static int COLUMNCOUNT = 5;
}

GcpTableModel::GcpTableModel() :
	m_gcpTable {new std::vector<GcpTableRow> {}}
{}

/*
	virtual QModelIndex index(int row, int column,
														const QModelIndex &parent = QModelIndex()) const = 0;
	virtual QModelIndex parent(const QModelIndex &child) const = 0;

	virtual QModelIndex sibling(int row, int column, const QModelIndex &idx) const;
*/
int GcpTableModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid()) {return 0;}

	if (m_gcpTable == nullptr) {return 0;}
	return m_gcpTable->size();
}

int GcpTableModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid()) {return 0;}
	return COLUMNCOUNT;
}

/*
	virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

*/

QVariant GcpTableModel::data(const QModelIndex &index, int role) const
{
	if (! index.isValid()) {
		return QVariant {};
	}
	if (m_gcpTable == nullptr || index.row() >= m_gcpTable->size() || index.row() < 0) {
		return QVariant {};
	}

	auto& row = m_gcpTable->at(index.row());

	if (role == Qt::CheckStateRole) {
		switch (index.column()) {
		case 0:
			if (row.enabled) {
				return Qt::Checked;
			} else {
				return Qt:: Unchecked;
			}
		}
	} else if (role == Qt::DisplayRole || role == Qt::EditRole) {
		switch (index.column()) {
		case 1:
			return QVariant {row.sourceX};
		case 2:
			return QVariant {row.sourceY};
		case 3:
			return QVariant {row.destX};
		case 4:
			return QVariant {row.destY};
		}
	}

	return QVariant {};
}

bool GcpTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (! index.isValid()) {return false;}
	if (role != Qt::EditRole && role != Qt::CheckStateRole) {return false;}
	if (m_gcpTable == nullptr) {return false;}
	if (index.row() >= m_gcpTable->size() || index.row() < 0) {return false;}

	int rowIndex = index.row();
	setRowData(&((*m_gcpTable)[rowIndex]), index, value);

	emit dataChanged(index, index);
	return true;
}

QVariant GcpTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role != Qt::DisplayRole) {
		return QVariant {};
	}

	if (orientation == Qt::Horizontal) {
		switch (section) {
		case 0:
			return QVariant {};
		case 1:
			return tr("srcX");
		case 2:
			return tr("srcY");
		case 3:
			return tr("dstX");
		case 4:
			return tr("dstY");
		default:
			return QVariant {};
		}
	}
	return QVariant {};
}

/*
	virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
														 int role = Qt::EditRole);

	virtual QMap<int, QVariant> itemData(const QModelIndex &index) const;
	virtual bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles);

	virtual QStringList mimeTypes() const;
	virtual QMimeData *mimeData(const QModelIndexList &indexes) const;
	virtual bool canDropMimeData(const QMimeData *data, Qt::DropAction action,
															 int row, int column, const QModelIndex &parent) const;
	virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action,
														int row, int column, const QModelIndex &parent);
	virtual Qt::DropActions supportedDropActions() const;

	virtual Qt::DropActions supportedDragActions() const;
*/

bool GcpTableModel::insertRows(int row, int count, const QModelIndex&)
{
	beginInsertRows(QModelIndex(), row, row + count - 1);

	for (int i = 0; i < count; ++i) {
		m_gcpTable->insert(m_gcpTable->begin() + row + i, GcpTableRow {});
	}

	endInsertRows();

	return true;
}

/*
	virtual bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex());
*/

bool GcpTableModel::removeRows(int row, int count, const QModelIndex&)
{
	beginRemoveRows( QModelIndex(), row, row + count - 1);

	for (int i = 0; i < count; ++i) {
		m_gcpTable->erase(m_gcpTable->begin() + row + count - i - 1);
	}

	endRemoveRows();

	return true;
}

/*
	virtual bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());
	virtual bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
												const QModelIndex &destinationParent, int destinationChild);
	virtual bool moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count,
													 const QModelIndex &destinationParent, int destinationChild);

	virtual void fetchMore(const QModelIndex &parent);
	virtual bool canFetchMore(const QModelIndex &parent) const;
*/
Qt::ItemFlags GcpTableModel::flags(const QModelIndex &index) const
{
	if (!index.isValid()) {
		return Qt::ItemIsEnabled;
	}

	auto ret = QAbstractTableModel::flags(index) | Qt::ItemIsEditable;

	if (index.column() == 0) {
		return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;
	} else {
		return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
	}
}

/*
	virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
	virtual QModelIndex buddy(const QModelIndex &index) const;
	virtual QModelIndexList match(const QModelIndex &start, int role,
																const QVariant &value, int hits = 1,
																Qt::MatchFlags flags =
																Qt::MatchFlags(Qt::MatchStartsWith|Qt::MatchWrap)) const;
	virtual QSize span(const QModelIndex &index) const;

	virtual QHash<int,QByteArray> roleNames() const;
*/

std::vector<GcpTableRow>* GcpTableModel::gcpTable()
{
	return m_gcpTable.get();
}

/*
public Q_SLOTS:
		virtual bool submit();
		virtual void revert();
*/

void GcpTableModel::replaceSourceData(const std::vector<std::vector<GcpTableRow>::size_type>& indices, const std::vector<QPointF>& points)
{
	emit beginResetModel();

	auto i = 0;
	for (auto index : indices) {
		const auto& point = points.at(i);

		(*m_gcpTable)[index].sourceX = point.x();
		(*m_gcpTable)[index].sourceY = point.y();
		++i;
	}

	emit endResetModel();
}

void GcpTableModel::replaceDestinationData(const std::vector<std::vector<GcpTableRow>::size_type>& indices, const std::vector<QPointF>& points)
{
	emit beginResetModel();

	auto i = 0;
	for (auto index : indices) {
		const auto& point = points.at(i);

		(*m_gcpTable)[index].destX = point.x();
		(*m_gcpTable)[index].destY = point.y();
		++i;
	}

	emit endResetModel();
}

bool GcpTableModel::setRowData(GcpTableRow* row, const QModelIndex& index, const QVariant& value)
{
	switch (index.column()) {
	case 0:
		row->enabled = value.toBool();
		return true;
	case 1:
		row->sourceX = value.toDouble();
		return true;
	case 2:
		row->sourceY = value.toDouble();
		return true;
	case 3:
		row->destX = value.toDouble();
		return true;
	case 4:
		row->destY = value.toDouble();
		return true;
	default:
		return false;
	}
}
