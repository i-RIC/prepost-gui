#ifndef ADDIBLEGCPTABLEMODEL_H
#define ADDIBLEGCPTABLEMODEL_H

#include "gcptablemodel.h"

class AddibleGcpTableModel : public GcpTableModel
{
	Q_OBJECT

public:
	AddibleGcpTableModel();

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

	void addRow(const GcpTableRow& row);
};

#endif // ADDIBLEGCPTABLEMODEL_H
