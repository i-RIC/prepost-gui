#ifndef POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_H
#define POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_H

#include "post2dwindownodevectorarrowgroupdataitem.h"
#include <vtkSmartPointer.h>
#include <vtkExtractGrid.h>
#include <vtkMaskPoints.h>
#include <vtkUnstructuredGrid.h>

class Post2dWindowNodeVectorArrowGroupUnstructuredDataItem : public Post2dWindowNodeVectorArrowGroupDataItem
{
	Q_OBJECT
public:
	enum SamplingMode {
		smAll,
		smRate,
		smNumber,
	};
	Post2dWindowNodeVectorArrowGroupUnstructuredDataItem(Post2dWindowDataItem* parent);
	virtual ~Post2dWindowNodeVectorArrowGroupUnstructuredDataItem();
protected:
	void updateActivePoints() override;
	void informGridUpdate();
	void updateFilterSettings();
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	QDialog* propertyDialog(QWidget* p) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
private:
	SamplingMode m_samplingMode;
	int m_samplingRate;
	int m_samplingNumber;
	vtkSmartPointer<vtkMaskPoints> m_arrowMask;
public:
	friend class Post2dWindowArrowUnstructuredSetProperty;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_H
