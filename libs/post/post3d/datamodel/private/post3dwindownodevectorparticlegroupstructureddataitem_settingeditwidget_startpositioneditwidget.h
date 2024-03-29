#ifndef POST3DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_STARTPOSITIONEDITWIDGET_H
#define POST3DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_STARTPOSITIONEDITWIDGET_H

#include "post3dwindownodevectorparticlegroupstructureddataitem_setting.h"
#include "post3dwindownodevectorparticlegroupstructureddataitem_settingeditwidget.h"

#include <QWidget>

namespace Ui {
class Post3dWindowNodeVectorParticleGroupStructuredDataItem_SettingEditWidget_StartPositionEditWidget;
}

class Post3dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit StartPositionEditWidget(QWidget *parent = nullptr);
	~StartPositionEditWidget();

	void setDimensions(int* dims);

	Setting::StartPosition setting() const;
	void setSetting(const Setting::StartPosition& pos);

private:
	Ui::Post3dWindowNodeVectorParticleGroupStructuredDataItem_SettingEditWidget_StartPositionEditWidget *ui;
};

#endif // POST3DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_STARTPOSITIONEDITWIDGET_H
