#ifndef POST3DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_STARTPOSITIONEDITWIDGET_H
#define POST3DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_STARTPOSITIONEDITWIDGET_H

#include "post3dwindownodevectorstreamlinegroupstructureddataitem_setting.h"
#include "post3dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget.h"

#include <QWidget>

namespace Ui {
class Post3dWindowNodeVectorStreamlineGroupStructuredDataItem_SettingEditWidget_StartPositionEditWidget;
}

class Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit StartPositionEditWidget(QWidget *parent = nullptr);
	~StartPositionEditWidget();

	void setDimensions(int* dims);

	Setting::StartPosition setting() const;
	void setSetting(const Setting::StartPosition& pos);

private:
	Ui::Post3dWindowNodeVectorStreamlineGroupStructuredDataItem_SettingEditWidget_StartPositionEditWidget *ui;
};

#endif // POST3DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_STARTPOSITIONEDITWIDGET_H
