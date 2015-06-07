#ifndef PREPROCESSORUNSTRUCTURED2DGRIDSHAPEDATAITEM_H
#define PREPROCESSORUNSTRUCTURED2DGRIDSHAPEDATAITEM_H

#include "preprocessorgridshapedataitem.h"
#include <vtkSmartPointer.h>
#include <vtkActor2D.h>
#include <vtkMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkLabeledDataMapper.h>

class PreProcessorUnstructured2dGridShapeDataItem : public PreProcessorGridShapeDataItem
{

private:
	static const int normalLineWidth = 1;

public:
	PreProcessorUnstructured2dGridShapeDataItem(PreProcessorDataItem* parent);
	~PreProcessorUnstructured2dGridShapeDataItem();
	/// Inform that the grid is updated.
	void informGridUpdate() override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void updateZDepthRangeItemCount() override {m_zDepthRange.setItemCount(1);}

protected:
	void assignActorZValues(const ZDepthRange& range) override;
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void setupActors();
	void updateActorSettings();

private:
	void setupIndexArray();
	vtkSmartPointer<vtkActor> m_wireframeActor;
	vtkSmartPointer<vtkDataSetMapper> m_wireframeMapper;
	vtkSmartPointer<vtkActor2D> m_indexActor;
	vtkSmartPointer<vtkLabeledDataMapper> m_indexMapper;
};

#endif // PREPROCESSORUNSTRUCTURED2DGRIDSHAPEDATAITEM_H
