#ifndef POST3DWINDOWCELLCONTOURGROUPDATAITEM_H
#define POST3DWINDOWCELLCONTOURGROUPDATAITEM_H

#include "../post3dcellrangesettingcontainer.h"
#include "../post3dwindowdataitem.h"

#include <guibase/scalarsettingcontainer.h>

class LookupTableContainer;
class vtkScalarBarWidget;

class Post3dWindowCellContourGroupDataItem : public Post3dWindowDataItem
{
public:
	Post3dWindowCellContourGroupDataItem(Post3dWindowDataItem* p);
	~Post3dWindowCellContourGroupDataItem();

	void setSetting(const ScalarSettingContainer& scalarSetting, const std::vector<Post3dCellRangeSettingContainer>& rangeSettings);
	const ScalarSettingContainer& setting() const;

	void update();

	LookupTableContainer* lookupTable() const;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QDialog* propertyDialog(QWidget* p) override;
	void handlePropertyDialogAccepted(QDialog* dialog) override;

	void updateActorSettings();

	void setupScalarBarActor();
	void updateScalarBarActorSetting();


	vtkScalarBarWidget* m_scalarBarWidget;

	ScalarSettingContainer m_setting;

	class SetSettingCommand;
};

#endif // POST3DWINDOWCELLCONTOURGROUPDATAITEM_H
