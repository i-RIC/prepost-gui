#include "post2dbirdeyewindowgridshapedataitem.h"
#include "post2dbirdeyewindowzonedataitem.h"
#include <guibase/coloreditwidget.h>
#include <guibase/gridshapeeditdialog.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <misc/iricundostack.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include "post2dbirdeyewindowzonedataitem.h"
#include <guibase/graphicsmisc.h>

#include <guicore/project/projectdata.h>
#include <QPen>
#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>
#include <QStandardItem>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QColorDialog>
#include <QColor>
#include <QMenu>
#include <QString>
#include <QAction>
#include <QUndoCommand>
#include <QSettings>

#include <QXmlStreamWriter>
#include <vtkActorCollection.h>
#include <vtkCollectionIterator.h>
#include <vtkLODActor.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkExtractGrid.h>
#include <vtkDataSet.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkDataSetTriangleFilter.h>
#include <vtkVertex.h>
#include <vtkTriangle.h>
#include <vtkCellArray.h>
#include <vtkPointLocator.h>
#include <vtkActor2DCollection.h>
#include <vtkProperty2D.h>
#include <vtkTextProperty.h>
#include <vtkPointData.h>
#include <vtkMapperCollection.h>

#include <vtkUnstructuredGridWriter.h>

#define LABEL "label2d"

Post2dBirdEyeWindowGridShapeDataItem::Post2dBirdEyeWindowGridShapeDataItem(Post2dBirdEyeWindowDataItem* parent)
	: Post2dBirdEyeWindowDataItem(tr("Grid shape"), QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	m_isDeletable = false;
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();

	m_shape = GridShapeEditDialog::Outline;
	PostZoneDataContainer* cont = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent)->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	if (gt->defaultGridType() == SolverDefinitionGridType::gtUnstructured2DGrid){
		m_shape = GridShapeEditDialog::Wireframe;
	}
	QSettings setting;
	QColor gridColor = setting.value("graphics/gridcolor", QColor(Qt::black)).value<QColor>();
	setColor(gridColor);
	m_indexVisible = false;
	QColor indexColor = setting.value("graphics/gridindexcolor", QColor(Qt::black)).value<QColor>();
	setIndexColor(indexColor);

	setupActors();
}

Post2dBirdEyeWindowGridShapeDataItem::~Post2dBirdEyeWindowGridShapeDataItem()
{
	renderer()->RemoveActor(m_outlineActor);
	renderer()->RemoveActor(m_wireframeActor);
	renderer()->RemoveActor2D(m_indexActor);
}

void Post2dBirdEyeWindowGridShapeDataItem::setupActors()
{
	m_warp = vtkSmartPointer<vtkWarpScalar>::New();
	m_warp->SetUseNormal(1);
	m_warp->SetNormal(0, 0, 1);
	m_warp->SetScaleFactor(1);

	m_labelWarp = vtkSmartPointer<vtkWarpScalar>::New();
	m_labelWarp->SetUseNormal(1);
	m_labelWarp->SetNormal(0, 0, 1);
	m_labelWarp->SetScaleFactor(1);

	m_outlineActor = vtkSmartPointer<vtkActor>::New();
	// no lighting is needed.
	m_outlineActor->GetProperty()->SetLighting(false);
	m_outlineActor->GetProperty()->SetLineWidth(normalOutlineWidth);

	PostZoneDataContainer* cont = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	if (gt->defaultGridType() != SolverDefinitionGridType::gtUnstructured2DGrid){
		renderer()->AddActor(m_outlineActor);
	}
	// build mapper too.
	m_outlineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_outlineActor->SetMapper(m_outlineMapper);

	// setup grid outline filter.
	m_outlineFilter = vtkSmartPointer<vtkStructuredGridOutlineFilter>::New();
	m_outlineMapper->SetInputConnection(m_outlineFilter->GetOutputPort());

	m_wireframeActor = vtkSmartPointer<vtkLODActor>::New();
	// Draw 5000 points from grid vertices.
	m_wireframeActor->SetNumberOfCloudPoints(5000);
	// Make the point size a little big.
	m_wireframeActor->GetProperty()->SetPointSize(2);
	m_wireframeActor->GetProperty()->SetLighting(false);
	renderer()->AddActor(m_wireframeActor);

	m_wireframeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_wireframeMapper->SetScalarVisibility(false);
	m_wireframeActor->SetMapper(m_wireframeMapper);
	m_wireframeActor->GetProperty()->SetRepresentationToWireframe();

	m_indexActor = vtkSmartPointer<vtkActor2D>::New();
	renderer()->AddActor2D(m_indexActor);

	m_indexTransform = vtkSmartPointer<vtkTransform>::New();

	m_indexTransformFilter = vtkSmartPointer<vtkTransformFilter>::New();
	m_indexTransformFilter->SetTransform(m_indexTransform);

	m_indexMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
	m_indexMapper->SetInputConnection(m_indexTransformFilter->GetOutputPort());
	m_indexMapper->SetLabelModeToLabelFieldData();
	m_indexMapper->SetFieldDataName(iRIC::toStr(PostZoneDataContainer::labelName).c_str());
	vtkTextProperty* prop = m_indexMapper->GetLabelTextProperty();
	prop->SetColor(0, 0, 0);
	prop->SetFontSize(12);
	prop->BoldOff();
	prop->ItalicOff();
	prop->ShadowOff();
	prop->SetJustificationToLeft();
	prop->SetVerticalJustificationToCentered();

//	m_axesActor = vtkSmartPointer<vtkCubeAxesActor2D>::New();
//	iRIC::setupCubeActor2D(m_axesActor);
//	m_axesActor->SetZAxisVisibility(0);
//	m_axesActor->SetCamera(renderer()->GetActiveCamera());
//	renderer()->AddActor2D(m_axesActor);

	m_indexActor->SetMapper(m_indexMapper);
	m_outlineActor->VisibilityOff();
	m_wireframeActor->VisibilityOff();
	m_indexActor->VisibilityOff();
//	m_axesActor->VisibilityOff();

	updateActorSettings();
}

void Post2dBirdEyeWindowGridShapeDataItem::update()
{
	updateActorSettings();
}

void Post2dBirdEyeWindowGridShapeDataItem::updateActorSettings()
{
	m_outlineActor->VisibilityOff();
	m_wireframeActor->VisibilityOff();
	m_indexActor->VisibilityOff();
//	m_axesActor->VisibilityOff();

	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();

	PostZoneDataContainer* cont = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == 0){return;}
	if (cont->data() == 0){return;}

	vtkPointSet* ps = cont->data();
	vtkPointSet* labeldata = cont->labelData();
	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*>(ps);

	m_warp->SetInputData(ps);
	m_warp->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(cont->elevationName()).c_str());
	m_labelWarp->SetInputData(labeldata);
	m_labelWarp->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(cont->elevationName()).c_str());

	vtkSmartPointer<vtkStructuredGrid> tmpgrid;
	switch (m_shape){
	case GridShapeEditDialog::Outline:
		m_outlineFilter->SetInputConnection(m_warp->GetOutputPort());
		m_outlineActor->GetProperty()->SetColor(m_color);
		m_actorCollection->AddItem(m_outlineActor);
		break;
	case GridShapeEditDialog::Wireframe:
		m_outlineFilter->SetInputConnection(m_warp->GetOutputPort());
		m_wireframeMapper->SetInputConnection(m_warp->GetOutputPort());
		m_wireframeActor->GetLODMappers()->RemoveAllItems();
		tmpgrid = grid;
		m_outlineActor->GetProperty()->SetColor(m_color);
		m_actorCollection->AddItem(m_outlineActor);
		m_wireframeActor->GetProperty()->SetColor(m_color);
		m_actorCollection->AddItem(m_wireframeActor);
		break;
	}
	if (m_indexVisible){
		m_labelWarp->Update();
		vtkPointSet* ps = m_labelWarp->GetOutput();
		vtkSmartPointer<vtkUnstructuredGrid> tmpGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
		tmpGrid->DeepCopy(ps);

		vtkAbstractArray* labelArray = labeldata->GetPointData()->GetAbstractArray(iRIC::toStr(PostZoneDataContainer::labelName).c_str());
		tmpGrid->GetPointData()->AddArray(labelArray);

		m_indexTransformFilter->SetInputData(tmpGrid);
		m_indexTransformFilter->Update();
/*
		vtkSmartPointer<vtkUnstructuredGridWriter> writer = vtkSmartPointer<vtkUnstructuredGridWriter>::New();
		writer->SetFileName("tmpgrid.txt");
		writer->SetInput(m_indexTransformFilter->GetOutput());
		writer->Update();
*/
		m_indexMapper->GetLabelTextProperty()->SetColor(m_indexColor);
		m_actor2DCollection->AddItem(m_indexActor);
	}
/*
	if (m_axesVisible){
		m_axesActor->GetProperty()->SetColor(m_axesColor);
		m_axesActor->GetAxisLabelTextProperty()->SetColor(m_axesColor);
		m_axesActor->GetAxisTitleTextProperty()->SetColor(m_axesColor);
		m_actor2DCollection->AddItem(m_axesActor);
		updateAxesRegion();
	}
*/
	updateVisibilityWithoutRendering();
}

void Post2dBirdEyeWindowGridShapeDataItem::doLoadFromProjectMainFile(const QDomNode& shapeNode)
{
	QDomElement elem = shapeNode.toElement();
	QColor tmpcol;
	m_shape = (GridShapeEditDialog::Shape)elem.attribute("shape", "0").toInt();
	tmpcol = loadColorAttribute("color", shapeNode, Qt::black);
	setColor(tmpcol);
	m_indexVisible = static_cast<bool>(elem.attribute("indexVisible").toInt());
	tmpcol = loadColorAttribute("indexColor", shapeNode, Qt::black);
	setIndexColor(tmpcol);

	updateActorSettings();
}

void Post2dBirdEyeWindowGridShapeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	QString qstr;
	writer.writeAttribute("shape", qstr.setNum((int)m_shape));
	writeColorAttribute("color", color(), writer);
	writer.writeAttribute("indexVisible", QString::number(static_cast<int>(m_indexVisible)));
	writeColorAttribute("indexColor", indexColor(), writer);
}

void Post2dBirdEyeWindowGridShapeDataItem::handleStandardItemDoubleClicked()
{
	showPropertyDialog();
}

QDialog* Post2dBirdEyeWindowGridShapeDataItem::propertyDialog(QWidget* p)
{
	GridShapeEditDialog* dialog = new GridShapeEditDialog(p);
	// It's enabled ALWAYS.
	dialog->setEnabled(true);
	dialog->setGridColor(color());
	dialog->setShape(shape());
	dialog->setIndexVisible(m_indexVisible);
	dialog->setIndexColor(indexColor());

	PostZoneDataContainer* cont = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	if (gt->defaultGridType() == SolverDefinitionGridType::gtUnstructured2DGrid){
		dialog->hideShape();
	}
	return dialog;
}

class Post2dBirdEyeWindowGridShapeDataSetSetting : public QUndoCommand
{
public:
	Post2dBirdEyeWindowGridShapeDataSetSetting(bool enabled, GridShapeEditDialog::Shape shape, QColor color, bool indexVisible, QColor indexColor, Post2dBirdEyeWindowGridShapeDataItem* item)
		: QUndoCommand(QObject::tr("Update Grid Shape Setting"))
	{
		m_newEnabled = enabled;
		m_newShape = shape;
		m_newColor = color;
		m_newIndexVisible = indexVisible;
		m_newIndexColor = indexColor;

		m_oldEnabled = item->isEnabled();
		m_oldShape = item->m_shape;
		m_oldColor = item->color();
		m_oldIndexVisible = item->m_indexVisible;
		m_oldIndexColor = item->indexColor();

		m_item = item;
	}
	void redo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->setEnabled(m_newEnabled);
		m_item->m_shape = m_newShape;
		m_item->setColor(m_newColor);
		m_item->m_indexVisible = m_newIndexVisible;
		m_item->setIndexColor(m_newIndexColor);

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
	void undo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->setEnabled(m_oldEnabled);
		m_item->m_shape = m_oldShape;
		m_item->setColor(m_oldColor);
		m_item->m_indexVisible = m_oldIndexVisible;
		m_item->setIndexColor(m_oldIndexColor);

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
private:
	bool m_oldEnabled;
	GridShapeEditDialog::Shape m_oldShape;
	QColor m_oldColor;
	bool m_oldIndexVisible;
	QColor m_oldIndexColor;

	bool m_newEnabled;
	GridShapeEditDialog::Shape m_newShape;
	QColor m_newColor;
	bool m_newIndexVisible;
	QColor m_newIndexColor;

	Post2dBirdEyeWindowGridShapeDataItem* m_item;
};

void Post2dBirdEyeWindowGridShapeDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	GridShapeEditDialog* dialog = dynamic_cast<GridShapeEditDialog*>(propDialog);
	iRICUndoStack::instance().push(new Post2dBirdEyeWindowGridShapeDataSetSetting(true, dialog->shape(), dialog->gridColor(), dialog->indexVisible(), dialog->indexColor(), this));
}


QColor Post2dBirdEyeWindowGridShapeDataItem::color()
{
	return QColor (
			(int)(m_color[0] * 255),
			(int)(m_color[1] * 255),
			(int)(m_color[2] * 255));
}

void Post2dBirdEyeWindowGridShapeDataItem::setColor(const QColor& color)
{
	m_color[0] = color.redF();
	m_color[1] = color.greenF();
	m_color[2] = color.blueF();
}

QColor Post2dBirdEyeWindowGridShapeDataItem::indexColor()
{
	return QColor (
			(int)(m_indexColor[0] * 255),
			(int)(m_indexColor[1] * 255),
			(int)(m_indexColor[2] * 255));
}

void Post2dBirdEyeWindowGridShapeDataItem::setIndexColor(const QColor& color)
{
	m_indexColor[0] = color.redF();
	m_indexColor[1] = color.greenF();
	m_indexColor[2] = color.blueF();
}

void Post2dBirdEyeWindowGridShapeDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	m_outlineActor->GetProperty()->SetLineWidth(selectedOutlineWidth);
	updateVisibility();
}

void Post2dBirdEyeWindowGridShapeDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	m_outlineActor->GetProperty()->SetLineWidth(normalOutlineWidth);
}

void Post2dBirdEyeWindowGridShapeDataItem::innerUpdateZScale(double scale)
{
	m_outlineActor->SetScale(1, 1, scale);
	m_wireframeActor->SetScale(1, 1, scale);
	m_indexTransform->Identity();
	m_indexTransform->Scale(1, 1, scale);
}
