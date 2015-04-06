#ifndef POSTTIMEDATAITEM_H
#define POSTTIMEDATAITEM_H

#include "../postbase_global.h"
#include <guicore/datamodel/graphicswindowdataitem.h>
#include <misc/timeformat.h>
#include "posttimesetting.h"

#include <vtkSmartPointer.h>
#include <vtkTextActor.h>

class PostCommonTimeEditCommand;

class POSTBASEDLL_EXPORT PostTimeDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT
public:
	/// Constructor
	PostTimeDataItem(GraphicsWindowDataItem* parent);
	virtual ~PostTimeDataItem();
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void update();
protected:
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);
private:
	const static int FONTSIZE = 13;
	void setupActors();
	void updateActorSettings();
	vtkSmartPointer<vtkTextActor> m_timeActor;
	PostTimeSetting m_setting;
public:
	friend class PostCommonTimeEditCommand;
};

#endif // POSTTIMEDATAITEM_H
