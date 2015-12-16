#ifndef POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_H
#define POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_H

#include "post2dwindownodevectorarrowgroupdataitem.h"
#include "post2dwindownodevectorarrowstructuredsetting.h"

#include <guibase/structuredgridregion.h>
#include <misc/compositecontainer.h>
#include <misc/intcontainer.h>

#include <vtkSmartPointer.h>
#include <vtkExtractGrid.h>
#include <vtkMaskPoints.h>
#include <vtkUnstructuredGrid.h>

class Post2dWindowNodeVectorArrowGroupStructuredDataItem : public Post2dWindowNodeVectorArrowGroupDataItem
{
	Q_OBJECT

public:
	Post2dWindowNodeVectorArrowGroupStructuredDataItem(Post2dWindowDataItem* parent);
	virtual ~Post2dWindowNodeVectorArrowGroupStructuredDataItem();

private:
	QDialog* propertyDialog(QWidget* p) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

	virtual void updateActivePoints() override;
	Post2dWindowNodeVectorArrowSetting& setting() override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	vtkSmartPointer<vtkMaskPoints> m_arrowMask;
	vtkSmartPointer<vtkExtractGrid> m_arrowExtract;

	Post2dWindowNodeVectorArrowStructuredSetting m_setting;

	class SetSettingCommand;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_H
