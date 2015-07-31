#ifndef POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_H
#define POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_H

#include "post2dwindownodevectorarrowgroupdataitem.h"

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

	struct Setting : public CompositeContainer
	{
		Setting();

		IntContainer iSampleRate;
		IntContainer jSampleRate;
		StructuredGridRegion::Range2d range;
	};

protected:
	virtual void updateActivePoints() override;
	void doLoadFromProjectMainFile(const QDomNode& /*node*/) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) override;
	QDialog* propertyDialog(QWidget* p) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

private:
	Setting m_stSetting;
	vtkSmartPointer<vtkMaskPoints> m_arrowMask;
	vtkSmartPointer<vtkExtractGrid> m_arrowExtract;

	class SetSettingCommand;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_H
