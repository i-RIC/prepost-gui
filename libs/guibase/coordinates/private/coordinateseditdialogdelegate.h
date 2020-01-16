#ifndef COORDINATESEDITDIALOGDELEGATE_H
#define COORDINATESEDITDIALOGDELEGATE_H

#include <QStyledItemDelegate>

class CoordinatesEditDialogDelegate : public QStyledItemDelegate
{
public:
	CoordinatesEditDialogDelegate(QObject* parent = nullptr);

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	void setEditorData(QWidget *editor, const QModelIndex &index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // COORDINATESEDITDIALOGDELEGATE_H
