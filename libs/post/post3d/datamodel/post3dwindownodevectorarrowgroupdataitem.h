#ifndef POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_H
#define POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_H

#include "post3dwindownodevectorarrowsettingdialog.h"
#include "../post3dwindowdataitem.h"

#include <guibase/arrowsettingcontainer.h>
#include <guibase/vtktool/vtkarrowlegendactors.h>

class Post3dWindowNodeVectorArrowGroupDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowNodeVectorArrowGroupDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowNodeVectorArrowGroupDataItem();

	void setSetting(const ArrowSettingContainer& arrowSetting, const std::vector<Post3dWindowNodeVectorArrowSettingDialog::FaceSetting>& faceSettings);

	const ArrowSettingContainer& setting() const;

	void update();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QDialog* propertyDialog(QWidget* p) override;
	void handlePropertyDialogAccepted(QDialog* dialog) override;

	void updateActorSettings();

	vtkArrowLegendActors m_legendActors;

	ArrowSettingContainer m_setting;

	class SetSettingCommand;
};

#endif // POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_H
