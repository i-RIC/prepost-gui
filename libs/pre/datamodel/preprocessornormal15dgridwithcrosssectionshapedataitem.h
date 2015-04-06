#ifndef PREPROCESSORNORMAL15DGRIDWITHCROSSSECTIONSHAPEDATAITEM_H
#define PREPROCESSORNORMAL15DGRIDWITHCROSSSECTIONSHAPEDATAITEM_H

#include "preprocessorgridshapedataitem.h"

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>

class Structured15DGridWithCrossSectionCrossSection;
class Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem;

class PreProcessorNormal15DGridWithCrossSectionShapeDataItem : public PreProcessorGridShapeDataItem
{
	Q_OBJECT
private:
	static const int normalLineWidth = 1;
	static const int selectedLineWidth = 2;
	static const int normalPointSize = 1;
	static const int selectedPointSize = 3.5;
public:
	PreProcessorNormal15DGridWithCrossSectionShapeDataItem(PreProcessorDataItem* parent);
	~PreProcessorNormal15DGridWithCrossSectionShapeDataItem();
	void informGridUpdate();
	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	void updateZDepthRangeItemCount(){m_zDepthRange.setItemCount(2);}
	void addCustomMenuItems(QMenu* menu);
	void updateActionStatus();
	void updateCrossSectionWindows();
	void requestCrossSectionWindowDelete(Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem* item);
	void mouseReleaseEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);
protected:
	void assignActionZValues(const ZDepthRange& range);
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void setupActors();
	void updateActorSettings();
private slots:
	void openCrossSectionWindow();
private:
	Structured15DGridWithCrossSectionCrossSection* selectedCrossSection();

	vtkSmartPointer<vtkActor> m_edgeActor;
	vtkSmartPointer<vtkActor> m_vertexActor;
	vtkSmartPointer<vtkDataSetMapper> m_edgeMapper;
	vtkSmartPointer<vtkDataSetMapper> m_vertexMapper;
	QAction* m_openCrossSectionWindowAction;
	QList<Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem*> m_crosssectionWindows;
public:
	friend class Structured15DGridCrossSectionEditCommand;
	friend class Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog;
};

#endif // PREPROCESSORNORMAL15DGRIDWITHCROSSSECTIONSHAPEDATAITEM_H
