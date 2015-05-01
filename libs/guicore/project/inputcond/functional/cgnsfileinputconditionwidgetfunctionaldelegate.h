#ifndef CGNSFILEINPUTCONDITIONWIDGETFUNCTIONALDELEGATE_H
#define CGNSFILEINPUTCONDITIONWIDGETFUNCTIONALDELEGATE_H

#include <QStyledItemDelegate>

class CgnsFileInputConditionWidgetFunctionalDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	CgnsFileInputConditionWidgetFunctionalDelegate(QObject* = 0) {}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
												const QModelIndex& index) const;

	void setEditorData(QWidget* editor, const QModelIndex& index) const;
	void setModelData(QWidget* editor, QAbstractItemModel* model,
										const QModelIndex& index) const;

	void updateEditorGeometry(QWidget* editor,
														const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // CGNSFILEINPUTCONDITIONWIDGETFUNCTIONALDELEGATE_H
