#ifndef GCPTABLEMODEL_H
#define GCPTABLEMODEL_H

#include <QAbstractTableModel>

#include <memory>

class GcpTableRow;

class GcpTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	GcpTableModel();

/*
	virtual QModelIndex index(int row, int column,
														const QModelIndex &parent = QModelIndex()) const = 0;
	virtual QModelIndex parent(const QModelIndex &child) const = 0;

	virtual QModelIndex sibling(int row, int column, const QModelIndex &idx) const;
*/
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	/*
	virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
*/

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

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
	virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
/*
	virtual bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex());
*/
	virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
/*
	virtual bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());
	virtual bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
												const QModelIndex &destinationParent, int destinationChild);
	virtual bool moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count,
													 const QModelIndex &destinationParent, int destinationChild);

	virtual void fetchMore(const QModelIndex &parent);
	virtual bool canFetchMore(const QModelIndex &parent) const;
*/
	Qt::ItemFlags flags(const QModelIndex &index) const override;
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

	std::vector<GcpTableRow>* gcpTable();
	void replaceSourceData(const std::vector<std::vector<GcpTableRow>::size_type>& indices, const std::vector<QPointF>& points);
	void replaceDestinationData(const std::vector<std::vector<GcpTableRow>::size_type>& indices, const std::vector<QPointF>& points);

/*
public Q_SLOTS:
		virtual bool submit();
		virtual void revert();
*/

protected:
	std::unique_ptr<std::vector<GcpTableRow>> m_gcpTable;

	bool setRowData(GcpTableRow* row, const QModelIndex& index, const QVariant& value);
};

#endif // GCPTABLEMODEL_H
