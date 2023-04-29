#ifndef POST2DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_STARTPOSITIONEDITWIDGET_H
#define POST2DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_STARTPOSITIONEDITWIDGET_H

#include "post2dwindownodevectorstreamlinegroupstructureddataitem_setting.h"
#include "post2dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget.h"

#include <QWidget>

namespace Ui {
class Post2dWindowNodeVectorStreamlineGroupStructuredDataItem_SettingEditWidget_StartPositionEditWidget;
}

class Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit StartPositionEditWidget(QWidget *parent = nullptr);
	~StartPositionEditWidget();

	void setDimensions(int idim, int jdim);

	Setting::StartPosition setting() const;
	void setSetting(const Setting::StartPosition& pos);

private:
	Ui::Post2dWindowNodeVectorStreamlineGroupStructuredDataItem_SettingEditWidget_StartPositionEditWidget *ui;
};

#endif // POST2DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_STARTPOSITIONEDITWIDGET_H
