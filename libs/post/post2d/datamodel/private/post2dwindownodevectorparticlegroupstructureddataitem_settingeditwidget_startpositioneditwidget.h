#ifndef POST2DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_STARTPOSITIONEDITWIDGET_H
#define POST2DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_STARTPOSITIONEDITWIDGET_H

#include "post2dwindownodevectorparticlegroupstructureddataitem_setting.h"
#include "post2dwindownodevectorparticlegroupstructureddataitem_settingeditwidget.h"

#include <QWidget>

namespace Ui {
class Post2dWindowNodeVectorParticleGroupStructuredDataItem_SettingEditWidget_StartPositionEditWidget;
}

class Post2dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit StartPositionEditWidget(QWidget *parent = nullptr);
	~StartPositionEditWidget();

	void setDimensions(int idim, int jdim);

	Setting::StartPosition setting() const;
	void setSetting(const Setting::StartPosition& pos);

private:
	Ui::Post2dWindowNodeVectorParticleGroupStructuredDataItem_SettingEditWidget_StartPositionEditWidget *ui;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_STARTPOSITIONEDITWIDGET_H
