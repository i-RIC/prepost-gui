#ifndef POST2DWINDOWPARTICLEIMAGEDATAITEM_SETTINGEDITWIDGET_VALUESIZEPAIRTABLECONTROLLER_H
#define POST2DWINDOWPARTICLEIMAGEDATAITEM_SETTINGEDITWIDGET_VALUESIZEPAIRTABLECONTROLLER_H

#include "post2dwindowparticleimagedataitem_settingeditwidget.h"

#include <QStandardItemModel>
#include <QObject>

class Post2dWindowParticleImageDataItem::SettingEditWidget::ValueSizePairTableController : public QObject
{
	Q_OBJECT

public:
	ValueSizePairTableController(SettingEditWidget* widget);
	~ValueSizePairTableController();

	void applyToTable();

private slots:
	void handleItemClick(const QModelIndex& index);
	void handleTableSelectionChange(const QItemSelection& selected, const QItemSelection& deselected);

private:
	SettingEditWidget* m_widget;
	QStandardItemModel m_model;

	class Delegate;
	Delegate* m_delegate;
};

#endif // POST2DWINDOWPARTICLEIMAGEDATAITEM_SETTINGEDITWIDGET_VALUESIZEPAIRTABLECONTROLLER_H
