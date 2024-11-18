#ifndef POSTTITLEDATAITEM_H
#define POSTTITLEDATAITEM_H

#include "../postbase_global.h"
#include "posttitlesetting.h"

#include <guicore/datamodel/graphicswindowdataitem.h>

#include <vtkSmartPointer.h>
#include <vtkTextActor.h>

class POSTBASEDLL_EXPORT PostTitleDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT
public:
	PostTitleDataItem(GraphicsWindowDataItem* parent);
	virtual ~PostTitleDataItem();

public slots:
	void showPropertyDialog() override;

private:
	void setupActors();
	void doUpdateActorSetting() override;

	QDialog* propertyDialog(QWidget* parent) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	vtkSmartPointer<vtkTextActor> m_titleActor;
	PostTitleSetting m_setting;

	class EditWidget;
	class SetSettingCommand;
};

#endif // POSTTITLEDATAITEM_H
