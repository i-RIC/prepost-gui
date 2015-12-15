#ifndef POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_H
#define POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_H

#include "post2dwindownodevectorarrowgroupdataitem.h"

#include <misc/intcontainer.h>
#include <misc/enumcontainert.h>
#include <misc/compositecontainer.h>

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

	struct Setting : public CompositeContainer
	{
		Setting();
		Setting(const Setting& s);
		Setting& operator=(const Setting& s);

		EnumContainerT<SamplingMode> samplingMode;
		IntContainer samplingRate;
		IntContainer samplingNumber;
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
	Setting m_unsSetting;
	vtkSmartPointer<vtkMaskPoints> m_arrowMask;

	class SetSettingCommand;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_H
