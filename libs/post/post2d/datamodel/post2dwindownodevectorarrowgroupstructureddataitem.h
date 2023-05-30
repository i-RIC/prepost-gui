#ifndef POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_H
#define POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_H

#include "post2dwindownodevectorarrowgroupdataitem.h"

#include <guicore/filter/structured2dfilteringsettingcontainer.h>
#include <guicore/region/region2dsettingcontainer.h>

class Post2dWindowNodeVectorArrowGroupStructuredDataItem : public Post2dWindowNodeVectorArrowGroupDataItem
{
	Q_OBJECT

public:
	Post2dWindowNodeVectorArrowGroupStructuredDataItem(Post2dWindowDataItem* parent);

public slots:
	void showPropertyDialog() override;

private:
	QDialog* propertyDialog(QWidget* p) override;

	vtkPointSet* buildFilteredData() override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	Region2dSettingContainer m_regionSetting;
	Structured2dFilteringSettingContainer m_filteringSetting;

	class SettingEditWidget;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_H
