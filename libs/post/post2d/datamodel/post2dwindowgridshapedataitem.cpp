#include "post2dwindowgridshapedataitem.h"
#include "post2dwindowzonedataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/coloreditwidget.h>
#include <guibase/graphicsmisc.h>
#include <guibase/gridshapeeditdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QAction>
#include <QColor>
#include <QColorDialog>
#include <QComboBox>
#include <QDialog>
#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>
#include <QLabel>
#include <QMenu>
#include <QPen>
#include <QPushButton>
#include <QSettings>
#include <QStandardItem>
#include <QString>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkActor.h>
#include <vtkActor2DCollection.h>
#include <vtkActorCollection.h>
#include <vtkCellArray.h>
#include <vtkCollectionIterator.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetTriangleFilter.h>
#include <vtkExtractGrid.h>
#include <vtkMapperCollection.h>
#include <vtkPointData.h>
#include <vtkPointLocator.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkTextProperty.h>
#include <vtkTriangle.h>
#include <vtkVertex.h>

#define LABEL "label2d"

Post2dWindowGridShapeDataItem::Post2dWindowGridShapeDataItem(Post2dWindowDataItem* parent)
	: Post2dWindowDataItem(tr("Grid shape"), QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	m_isDeletable = false;
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();

	m_shape = GridShapeEditDialog::Outline;
	QSettings setting;
	QColor gridColor = setting.value("graphics/gridcolor", QColor(Qt::black)).value<QColor>();
	setColor(gridColor);

	m_indexVisible = false;
	QColor indexColor = setting.value("graphics/gridindexcolor", QColor(Qt::black)).value<QColor>();
	setIndexColor(indexColor);

//	m_axesVisible = false;
//	QColor axesColor = setting.value("graphics/gridaxescolor", Qt::black).value<QColor>();
//	setAxesColor(axesColor);

	setupActors();
}

Post2dWindowGridShapeDataItem::~Post2dWindowGridShapeDataItem()
{
	renderer()->RemoveActor(m_outlineActor);
	renderer()->RemoveActor(m_wireframeActor);
	renderer()->RemoveActor2D(m_indexActor);
}

void Post2dWindowGridShapeDataItem::setupActors()
{
	m_outlineActor = vtkSmartPointer<vtkActor>::New();
	// no lighting is needed.
	m_outlineActor->GetProperty()->SetLighting(false);
	m_outlineActor->GetProperty()->SetLineWidth(normalOutlineWidth);
	renderer()->AddActor(m_outlineActor);

	// build mapper too.
	m_outlineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_outlineActor->SetMapper(m_outlineMapper);

	m_wireframeActor = vtkSmartPointer<vtkActor>::New();

	// setup grid outline filter.
	m_outlineFilter = vtkSmartPointer<vtkStructuredGridOutlineFilter>::New();
	m_outlineMapper->SetInputConnection(m_outlineFilter->GetOutputPort());

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

	m_indexMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
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

	m_indexActor->SetMapper(m_indexMapper);

//	m_axesActor = vtkSmartPointer<vtkCubeAxesActor2D>::New();
//	iRIC::setupCubeActor2D(m_axesActor);
//	m_axesActor->SetZAxisVisibility(0);
//	m_axesActor->SetCamera(renderer()->GetActiveCamera());
//	renderer()->AddActor2D(m_axesActor);

	m_outlineActor->VisibilityOff();
	m_wireframeActor->VisibilityOff();
	m_indexActor->VisibilityOff();
//	m_axesActor->VisibilityOff();

	updateActorSettings();
}

void Post2dWindowGridShapeDataItem::update()
{
	updateActorSettings();
}

void Post2dWindowGridShapeDataItem::updateActorSettings()
{
	m_outlineActor->VisibilityOff();
	m_wireframeActor->VisibilityOff();
	m_indexActor->VisibilityOff();
//	m_axesActor->VisibilityOff();

	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}
	vtkPointSet* ps = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->filteredData();

	SolverDefinitionGridType* gt = cont->gridType();
	if (gt->defaultGridType() == SolverDefinitionGridType::gtUnstructured2DGrid) {
		m_wireframeMapper->SetInputData(ps);
		m_wireframeActor->GetProperty()->SetColor(m_color);
		m_actorCollection->AddItem(m_wireframeActor);
	} else {
		vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*>(cont->data());
		vtkSmartPointer<vtkStructuredGrid> tmpgrid = grid;
		vtkSmartPointer<vtkDataSetMapper> mapper;
		switch (m_shape) {
		case GridShapeEditDialog::Outline:
			m_outlineFilter->SetInputData(tmpgrid);
			m_outlineActor->GetProperty()->SetColor(m_color);
			m_actorCollection->AddItem(m_outlineActor);
			break;

		case GridShapeEditDialog::Wireframe:
			m_outlineFilter->SetInputData(tmpgrid);
			m_outlineActor->GetProperty()->SetColor(m_color);
			m_actorCollection->AddItem(m_outlineActor);
			m_wireframeMapper->SetInputData(ps);
			m_wireframeActor->GetProperty()->SetColor(m_color);
			m_actorCollection->AddItem(m_wireframeActor);
			break;
		}
	}
	if (m_indexVisible) {
		vtkPointSet* labeldata = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer()->labelData();
		m_indexMapper->SetInputData(labeldata);
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

void Post2dWindowGridShapeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode shapeNode = iRIC::getChildNode(node, "Shape");
	if (! shapeNode.isNull()) {
		loadShapeFromProjectMainFile(shapeNode);
	}
}

void Post2dWindowGridShapeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	// save the color and the shape of the grid
	writer.writeStartElement("Shape");
	saveShapeToProjectMainFile(writer);
	writer.writeEndElement();
}

void Post2dWindowGridShapeDataItem::loadShapeFromProjectMainFile(const QDomNode& shapeNode)
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

void Post2dWindowGridShapeDataItem::saveShapeToProjectMainFile(QXmlStreamWriter& writer)
{
	QString qstr;
	writer.writeAttribute("shape", qstr.setNum((int)m_shape));
	writeColorAttribute("color", color(), writer);
	writer.writeAttribute("indexVisible", QString::number(static_cast<int>(m_indexVisible)));
	writeColorAttribute("indexColor", indexColor(), writer);
}

void Post2dWindowGridShapeDataItem::handleStandardItemDoubleClicked()
{
	showPropertyDialog();
}

QDialog* Post2dWindowGridShapeDataItem::propertyDialog(QWidget* p)
{
	GridShapeEditDialog* dialog = new GridShapeEditDialog(p);
	// It's enabled ALWAYS.
	dialog->setEnabled(true);
	dialog->setGridColor(color());
	dialog->setShape(shape());
	dialog->setIndexVisible(m_indexVisible);
	dialog->setIndexColor(indexColor());
//	dialog->setAxesVisible(m_axesVisible);
//	dialog->setAxesColor(axesColor());

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	if (gt->defaultGridType() == SolverDefinitionGridType::gtUnstructured2DGrid) {
		dialog->hideShape();
	}
	return dialog;
}

class Post2dWindowGridShapeDataSetSetting : public QUndoCommand
{
public:
	Post2dWindowGridShapeDataSetSetting(bool enabled, GridShapeEditDialog::Shape shape, QColor color, bool indexVisible, QColor indexColor, Post2dWindowGridShapeDataItem* item)
		: QUndoCommand(QObject::tr("Update Grid Shape Setting")) {
		m_newEnabled = enabled;
		m_newShape = shape;
		m_newColor = color;
		m_newIndexVisible = indexVisible;
		m_newIndexColor = indexColor;
//		m_newAxesVisible = axesVisible;
//		m_newAxesColor = axesColor;

		m_oldEnabled = item->isEnabled();
		m_oldShape = item->m_shape;
		m_oldColor = item->color();
		m_oldIndexVisible = item->m_indexVisible;
		m_oldIndexColor = item->indexColor();
//		m_oldAxesVisible = item->m_axesVisible;
//		m_oldAxesColor = item->axesColor();

		m_item = item;
	}
	void redo() {
		m_item->setIsCommandExecuting(true);
		m_item->setEnabled(m_newEnabled);
		m_item->m_shape = m_newShape;
		m_item->setColor(m_newColor);
		m_item->m_indexVisible = m_newIndexVisible;
		m_item->setIndexColor(m_newIndexColor);
//		m_item->m_axesVisible = m_newAxesVisible;
//		m_item->setAxesColor(m_newAxesColor);

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
	void undo() {
		m_item->setIsCommandExecuting(true);
		m_item->setEnabled(m_oldEnabled);
		m_item->m_shape = m_oldShape;
		m_item->setColor(m_oldColor);
		m_item->m_indexVisible = m_oldIndexVisible;
		m_item->setIndexColor(m_oldIndexColor);
//		m_item->m_axesVisible = m_oldAxesVisible;
//		m_item->setAxesColor(m_oldAxesColor);

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
//	bool m_oldAxesVisible;
//	QColor m_oldAxesColor;

	bool m_newEnabled;
	GridShapeEditDialog::Shape m_newShape;
	QColor m_newColor;
	bool m_newIndexVisible;
	QColor m_newIndexColor;
//	bool m_newAxesVisible;
//	QColor m_newAxesColor;

	Post2dWindowGridShapeDataItem* m_item;
};

void Post2dWindowGridShapeDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	GridShapeEditDialog* dialog = dynamic_cast<GridShapeEditDialog*>(propDialog);
	iRICUndoStack::instance().push(new Post2dWindowGridShapeDataSetSetting(true, dialog->shape(), dialog->gridColor(), dialog->indexVisible(), dialog->indexColor(), this));
}


QColor Post2dWindowGridShapeDataItem::color() const
{
	return QColor(
					 (int)(m_color[0] * 255),
					 (int)(m_color[1] * 255),
					 (int)(m_color[2] * 255));
}

void Post2dWindowGridShapeDataItem::setColor(const QColor& color)
{
	m_color[0] = color.redF();
	m_color[1] = color.greenF();
	m_color[2] = color.blueF();
}

QColor Post2dWindowGridShapeDataItem::indexColor() const
{
	return QColor(
					 (int)(m_indexColor[0] * 255),
					 (int)(m_indexColor[1] * 255),
					 (int)(m_indexColor[2] * 255));
}

void Post2dWindowGridShapeDataItem::setIndexColor(const QColor& color)
{
	m_indexColor[0] = color.redF();
	m_indexColor[1] = color.greenF();
	m_indexColor[2] = color.blueF();
}

/*
QColor Post2dWindowGridShapeDataItem::axesColor()
{
	return QColor (
			(int)(m_axesColor[0] * 255),
			(int)(m_axesColor[1] * 255),
			(int)(m_axesColor[2] * 255));
}
*/

/*
void Post2dWindowGridShapeDataItem::setAxesColor(const QColor& color)
{
	m_axesColor[0] = color.redF();
	m_axesColor[1] = color.greenF();
	m_axesColor[2] = color.blueF();
}
*/

void Post2dWindowGridShapeDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	m_outlineActor->GetProperty()->SetLineWidth(selectedOutlineWidth);
	updateVisibility();
}

void Post2dWindowGridShapeDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	m_outlineActor->GetProperty()->SetLineWidth(normalOutlineWidth);
}

void Post2dWindowGridShapeDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_outlineActor->SetPosition(0, 0, range.max());
	m_wireframeActor->SetPosition(0, 0, range.min());
}

/*
void Post2dWindowGridShapeDataItem::updateAxesRegion()
{
	vtkPointSet* ps = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer()->data();
	if (ps == 0){return;}
	double bounds[6];
	ps->GetBounds(bounds);
	iRIC::fattenBounds(bounds, 0.1);
	m_axesActor->SetBounds(bounds);
}
*/
