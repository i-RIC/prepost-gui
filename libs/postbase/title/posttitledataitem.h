#ifndef POSTTITLEDATAITEM_H
#define POSTTITLEDATAITEM_H

#include "../postbase_global.h"
#include <guicore/datamodel/graphicswindowdataitem.h>
#include "posttitlesetting.h"

#include <vtkSmartPointer.h>
#include <vtkTextActor.h>

class PostCommonTitleEditCommand;

class POSTBASEDLL_EXPORT PostTitleDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT
public:
	/// Constructor
	PostTitleDataItem(GraphicsWindowDataItem* parent);
	virtual ~PostTitleDataItem();
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
protected:
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);
private:
	void setupActors();
	void updateActorSettings();
	vtkSmartPointer<vtkTextActor> m_titleActor;
	PostTitleSetting m_setting;
public:
	friend class PostCommonTitleEditCommand;
};

#endif // POSTTITLEDATAITEM_H
