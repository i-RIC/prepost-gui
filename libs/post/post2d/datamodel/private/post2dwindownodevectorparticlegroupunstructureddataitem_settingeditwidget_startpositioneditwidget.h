#ifndef POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_STARTPOSITIONEDITWIDGET_H
#define POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_STARTPOSITIONEDITWIDGET_H

#include "post2dwindownodevectorparticlegroupunstructureddataitem_setting.h"
#include "post2dwindownodevectorparticlegroupunstructureddataitem_settingeditwidget.h"

#include <QWidget>

namespace Ui {
class Post2dWindowNodeVectorParticleGroupUnstructuredDataItem_SettingEditWidget_StartPositionEditWidget;
}

class Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::StartPositionEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit StartPositionEditWidget(QWidget *parent = nullptr);
	~StartPositionEditWidget();

	Setting::StartPosition setting() const;
	void setSetting(const Setting::StartPosition& pos);

	bool handleMousePoint1Change(const QPointF point);
	bool handleMousePoint2Change(const QPointF point);

private:
	Ui::Post2dWindowNodeVectorParticleGroupUnstructuredDataItem_SettingEditWidget_StartPositionEditWidget *ui;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_STARTPOSITIONEDITWIDGET_H
