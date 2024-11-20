#ifndef POSTTIMEDATAITEM_H
#define POSTTIMEDATAITEM_H

#include "../postbase_global.h"

#include <guibase/vtktextpropertysettingcontainer.h>
#include <guibase/timeformat/timeformat.h>
#include <guicore/datamodel/graphicswindowdataitem.h>
#include <misc/boolcontainer.h>
#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/stringcontainer.h>

#include <vtkSmartPointer.h>
#include <vtkTextActor.h>

class POSTBASEDLL_EXPORT PostTimeDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	struct Setting : public CompositeContainer
	{
		Setting();
		Setting(const Setting& s);

		Setting& operator=(const Setting& s);

		void load(const QDomNode& node) override;
		void save(QXmlStreamWriter& writer) const override;

		BoolContainer useProjectSetting;
		EnumContainerT<TimeFormat> timeFormat;
		StringContainer customTimeFormat;
		BoolContainer showTimeZone;
		vtkTextPropertySettingContainer fontSetting;
	};

	PostTimeDataItem(GraphicsWindowDataItem* parent);
	~PostTimeDataItem() override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void update();

public slots:
	void showPropertyDialog() override;

private:
	const static int FONTSIZE = 13;

	QDialog* propertyDialog(QWidget* parent) override;

	void setupActors();
	void doUpdateActorSetting() override;

	vtkSmartPointer<vtkTextActor> m_timeActor;
	Setting m_setting;

	class EditWidget;
	class SetSettingCommand;
};

#endif // POSTTIMEDATAITEM_H
