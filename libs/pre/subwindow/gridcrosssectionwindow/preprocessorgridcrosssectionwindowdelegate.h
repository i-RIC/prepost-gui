#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOWDELEGATE_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOWDELEGATE_H

#include <QStyledItemDelegate>
class RawDataRiverCrosssection;
class CenteredCheckBox;

class PreProcessorGridCrosssectionWindowDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	PreProcessorGridCrosssectionWindowDelegate()
		: QStyledItemDelegate()
	{}
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
		const QModelIndex &index) const;

	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const;

	void updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOWDELEGATE_H
