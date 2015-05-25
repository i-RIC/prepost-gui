#ifndef STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONWINDOWDELEGATE_H
#define STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONWINDOWDELEGATE_H

#include <QStyledItemDelegate>

class Structured15DGridWithCrossSectionCrossSectionWindowDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	Structured15DGridWithCrossSectionCrossSectionWindowDelegate();
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
												const QModelIndex& index) const;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model,
										const QModelIndex& index) const;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
														const QModelIndex& index) const;
};

#endif // STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONWINDOWDELEGATE_H
