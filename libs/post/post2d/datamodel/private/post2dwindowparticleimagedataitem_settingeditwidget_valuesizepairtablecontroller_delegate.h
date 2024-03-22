#ifndef POST2DWINDOWPARTICLEIMAGEDATAITEM_SETTINGEDITWIDGET_VALUESIZEPAIRTABLECONTROLLER_DELEGATE_H
#define POST2DWINDOWPARTICLEIMAGEDATAITEM_SETTINGEDITWIDGET_VALUESIZEPAIRTABLECONTROLLER_DELEGATE_H

#include "post2dwindowparticleimagedataitem_settingeditwidget_valuesizepairtablecontroller.h"

#include <QItemDelegate>

class Post2dWindowParticleImageDataItem::SettingEditWidget::ValueSizePairTableController::Delegate : public QItemDelegate
{
public:
	Delegate(ValueSizePairTableController* controller, QObject* parent = nullptr);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
	Setting* setting() const;

	ValueSizePairTableController* m_controller;
};

#endif // POST2DWINDOWPARTICLEIMAGEDATAITEM_SETTINGEDITWIDGET_VALUESIZEPAIRTABLECONTROLLER_DELEGATE_H
