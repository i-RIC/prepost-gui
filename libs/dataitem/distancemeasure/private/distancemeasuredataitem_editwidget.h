#ifndef DISTANCEMEASUREDATAITEM_EDITWIDGET_H
#define DISTANCEMEASUREDATAITEM_EDITWIDGET_H

#include "../distancemeasuredataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class DistanceMeasureDataItem_EditWidget;
}

class DistanceMeasureDataItem::EditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	EditWidget(DistanceMeasureDataItem* item, QWidget *parent);
	~EditWidget();

	QString name() const;
	void setName(const QString& name);

	DistanceMeasureSetting setting() const;
	void setSetting(const DistanceMeasureSetting& setting);

	QUndoCommand* createModifyCommand(bool apply) override;

public slots:
	void setStartPoint(const QPointF& point);
	void setEndPoint(const QPointF& point);

private slots:
	void updateAutoLabel();

private:
	QString autoLabel() const;

	DistanceMeasureSetting m_setting;
	DistanceMeasureDataItem* m_item;

	Ui::DistanceMeasureDataItem_EditWidget *ui;
};

#endif // DISTANCEMEASUREDATAITEM_EDITWIDGET_H
