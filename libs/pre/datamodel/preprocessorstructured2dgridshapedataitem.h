#ifndef PREPROCESSORSTRUCTURED2DGRIDSHAPEDATAITEM_H
#define PREPROCESSORSTRUCTURED2DGRIDSHAPEDATAITEM_H

#include "preprocessorgridshapedataitem.h"
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkMapper.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkLabeledDataMapper.h>

class PreProcessorStructured2dGridShapeDataItem : public PreProcessorGridShapeDataItem
{

private:
	static const int normalOutlineWidth = 1;
	static const int selectedOutlineWidth = 2;

public:
	PreProcessorStructured2dGridShapeDataItem(PreProcessorDataItem* parent);
	~PreProcessorStructured2dGridShapeDataItem();
	/// Inform that the grid is updated.
	void informGridUpdate() override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void updateZDepthRangeItemCount() override {m_zDepthRange.setItemCount(2);}

protected:
	void assignActorZValues(const ZDepthRange& range) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void setupActors();
	void updateActorSettings();

private:
	vtkSmartPointer<vtkActor> m_outlineActor;
	vtkSmartPointer<vtkActor> m_wireframeActor;
	vtkSmartPointer<vtkMapper> m_outlineMapper;
	vtkSmartPointer<vtkPolyDataMapper> m_wireframeMapper;
	vtkSmartPointer<vtkStructuredGridOutlineFilter> m_outlineFilter;
	vtkSmartPointer<vtkActor2D> m_indexActor;
	vtkSmartPointer<vtkLabeledDataMapper> m_indexMapper;
};

#endif // PREPROCESSORSTRUCTURED2DGRIDSHAPEDATAITEM_H
