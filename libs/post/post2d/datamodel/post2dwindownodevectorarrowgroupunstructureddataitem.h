#ifndef POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_H
#define POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_H

#include "post2dwindownodevectorarrowgroupdataitem.h"

#include <guicore/filter/generalfilteringsettingcontainer.h>
#include <guicore/region/region2dsettingcontainer.h>

class Post2dWindowNodeVectorArrowGroupUnstructuredDataItem : public Post2dWindowNodeVectorArrowGroupDataItem
{
	Q_OBJECT

public:
	Post2dWindowNodeVectorArrowGroupUnstructuredDataItem(const std::string& name, Post2dWindowDataItem* parent);
	QDialog* propertyDialog(QWidget* p) override;

public slots:
	void showPropertyDialog() override;

private:
	vtkPointSet* buildFilteredData() override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	Region2dSettingContainer m_regionSetting;
	GeneralFilteringSettingContainer m_filteringSetting;

	class PropertyDialog;
	class SettingEditWidget;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_H
