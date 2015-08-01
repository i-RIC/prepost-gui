#ifndef POSTTIMEDATAITEM_H
#define POSTTIMEDATAITEM_H

#include "../postbase_global.h"

#include <guicore/datamodel/graphicswindowdataitem.h>
#include <misc/timeformat.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/colorcontainer.h>

#include <vtkSmartPointer.h>
#include <vtkTextActor.h>

class POSTBASEDLL_EXPORT PostTimeDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	struct Setting : public CompositeContainer
	{
		/// Constructor
		Setting();

		/// Time format
		EnumContainerT<TimeFormat::Format> timeFormat;
		/// Color
		ColorContainer color;
	};

	/// Constructor
	PostTimeDataItem(GraphicsWindowDataItem* parent);
	virtual ~PostTimeDataItem();
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void update();

protected:
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

private:
	const static int FONTSIZE = 13;
	void setupActors();
	void updateActorSettings();

	vtkSmartPointer<vtkTextActor> m_timeActor;
	Setting m_setting;

	class SetSettingCommand;
};

#endif // POSTTIMEDATAITEM_H
