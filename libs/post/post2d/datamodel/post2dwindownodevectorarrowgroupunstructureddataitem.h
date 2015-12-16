#ifndef POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_H
#define POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_H

#include "post2dwindownodevectorarrowgroupdataitem.h"
#include "post2dwindownodevectorarrowunstructuredsetting.h"

#include <vtkSmartPointer.h>
#include <vtkExtractGrid.h>
#include <vtkMaskPoints.h>
#include <vtkUnstructuredGrid.h>

class Post2dWindowNodeVectorArrowGroupUnstructuredDataItem : public Post2dWindowNodeVectorArrowGroupDataItem
{
	Q_OBJECT

public:
	Post2dWindowNodeVectorArrowGroupUnstructuredDataItem(Post2dWindowDataItem* parent);
	virtual ~Post2dWindowNodeVectorArrowGroupUnstructuredDataItem();

private:
	void updateActivePoints() override;
	Post2dWindowNodeVectorArrowSetting& setting() override;

	QDialog* propertyDialog(QWidget* p) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	vtkSmartPointer<vtkMaskPoints> m_arrowMask;

	Post2dWindowNodeVectorArrowUnstructuredSetting m_setting;

	class SetSettingCommand;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_H
