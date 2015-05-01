#include "post3dwindowgridshapedataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guibase/graphicsmisc.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QSettings>
#include <QStandardItem>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkActor2DCollection.h>
#include <vtkExtractGrid.h>
#include <vtkMapperCollection.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>
#include <vtkTextProperty.h>
#include <vtkTransformFilter.h>

Post3dWindowGridShapeDataItem::Post3dWindowGridShapeDataItem(Post3dWindowDataItem* parent)
	: Post3dWindowDataItem(tr("Grid shape"), QIcon(":/libs/guibase/images/iconPaper.png"), parent)
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

	setupActors();
}

Post3dWindowGridShapeDataItem::~Post3dWindowGridShapeDataItem()
{
	renderer()->RemoveActor(m_outlineActor);
	renderer()->RemoveActor(m_wireframeActor);
	renderer()->RemoveActor2D(m_indexActor);
}

void Post3dWindowGridShapeDataItem::setupActors()
{
	m_outlineActor = vtkSmartPointer<vtkActor>::New();
	// no lighting is needed.
	m_outlineActor->GetProperty()->SetLighting(false);
	m_outlineActor->GetProperty()->SetLineWidth(normalOutlineWidth);
	renderer()->AddActor(m_outlineActor);

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

	/*
	m_axesActor = vtkSmartPointer<vtkCubeAxesActor2D>::New();
	iRIC::setupCubeActor2D(m_axesActor);
	m_axesActor->SetZAxisVisibility(0);
	m_axesActor->SetCamera(renderer()->GetActiveCamera());
	renderer()->AddActor2D(m_axesActor);
	*/

	m_indexActor->SetMapper(m_indexMapper);
	m_outlineActor->VisibilityOff();
	m_wireframeActor->VisibilityOff();
	m_indexActor->VisibilityOff();
//	m_axesActor->VisibilityOff();

	updateActorSettings();
}

void Post3dWindowGridShapeDataItem::update()
{
	updateActorSettings();
}

void Post3dWindowGridShapeDataItem::updateActorSettings()
{
	m_outlineActor->VisibilityOff();
	m_wireframeActor->VisibilityOff();
	m_indexActor->VisibilityOff();
//	m_axesActor->VisibilityOff();

	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr) {return;}
	if (cont->data() == nullptr) {return;}

	vtkPointSet* ps = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer()->data();
	vtkPointSet* labeldata = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer()->labelData();
	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*>(ps);

	vtkSmartPointer<vtkStructuredGrid> tmpgrid;
	switch (m_shape) {
	case GridShapeEditDialog::Outline:
		m_outlineFilter->SetInputData(ps);
		m_outlineActor->GetProperty()->SetColor(m_color);
		m_actorCollection->AddItem(m_outlineActor);
		break;
	case GridShapeEditDialog::Wireframe:
		m_outlineFilter->SetInputData(ps);
		m_wireframeMapper->SetInputData(ps);
		m_wireframeActor->GetLODMappers()->RemoveAllItems();
		tmpgrid = grid;
		for (int i = 0; i < 3; ++i) {
			vtkSmartPointer<vtkExtractGrid> extractor = vtkSmartPointer<vtkExtractGrid>::New();
			extractor->SetInputData(tmpgrid);
			extractor->SetSampleRate(2, 2, 2);
			vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
			mapper->SetInputConnection(extractor->GetOutputPort());
			m_wireframeActor->AddLODMapper(mapper);
			tmpgrid = extractor->GetOutput();
		}
		m_outlineActor->GetProperty()->SetColor(m_color);
		m_actorCollection->AddItem(m_outlineActor);
		m_wireframeActor->GetProperty()->SetColor(m_color);
		m_actorCollection->AddItem(m_wireframeActor);
		break;
	}
	if (m_indexVisible) {
		m_indexTransformFilter->SetInputData(labeldata);
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

void Post3dWindowGridShapeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode shapeNode = iRIC::getChildNode(node, "Shape");
	if (! shapeNode.isNull()) {
		loadShapeFromProjectMainFile(shapeNode);
	}
}

void Post3dWindowGridShapeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	// save the color and the shape of the grid
	writer.writeStartElement("Shape");
	saveShapeToProjectMainFile(writer);
	writer.writeEndElement();
}

void Post3dWindowGridShapeDataItem::loadShapeFromProjectMainFile(const QDomNode& shapeNode)
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

void Post3dWindowGridShapeDataItem::saveShapeToProjectMainFile(QXmlStreamWriter& writer)
{
	QString qstr;
	writer.writeAttribute("shape", qstr.setNum((int)m_shape));
	writeColorAttribute("color", color(), writer);
	writer.writeAttribute("indexVisible", QString::number(static_cast<int>(m_indexVisible)));
	writeColorAttribute("indexColor", indexColor(), writer);
}

void Post3dWindowGridShapeDataItem::handleStandardItemDoubleClicked()
{
	showPropertyDialog();
}

QDialog* Post3dWindowGridShapeDataItem::propertyDialog(QWidget* parent)
{
	GridShapeEditDialog* dialog = new GridShapeEditDialog(parent);
	dialog->setGridColor(color());
	dialog->setShape(shape());
	dialog->setIndexVisible(m_indexVisible);
	dialog->setIndexColor(indexColor());
//	dialog->setAxesVisible(m_axesVisible);
//	dialog->setAxesColor(axesColor());

	return dialog;
}

class Post3dWindowGridShapeDataSetSetting : public QUndoCommand
{
public:
	Post3dWindowGridShapeDataSetSetting(bool enabled, GridShapeEditDialog::Shape shape, QColor color, bool indexVisible, QColor indexColor, Post3dWindowGridShapeDataItem* item)
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

	Post3dWindowGridShapeDataItem* m_item;
};

void Post3dWindowGridShapeDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	GridShapeEditDialog* dialog = dynamic_cast<GridShapeEditDialog*>(propDialog);
	iRICUndoStack::instance().push(new Post3dWindowGridShapeDataSetSetting(dialog->isEnabled(), dialog->shape(), dialog->gridColor(), dialog->indexVisible(), dialog->indexColor(), this));
}

QColor Post3dWindowGridShapeDataItem::color()
{
	return QColor(
					 (int)(m_color[0] * 255),
					 (int)(m_color[1] * 255),
					 (int)(m_color[2] * 255));
}

void Post3dWindowGridShapeDataItem::setColor(const QColor& color)
{
	m_color[0] = color.red()   / 255.;
	m_color[1] = color.green() / 255.;
	m_color[2] = color.blue()  / 255.;
}

QColor Post3dWindowGridShapeDataItem::indexColor()
{
	return QColor(
					 (int)(m_indexColor[0] * 255),
					 (int)(m_indexColor[1] * 255),
					 (int)(m_indexColor[2] * 255));
}

void Post3dWindowGridShapeDataItem::setIndexColor(const QColor& color)
{
	m_indexColor[0] = color.redF();
	m_indexColor[1] = color.greenF();
	m_indexColor[2] = color.blueF();
}

/*
QColor Post3dWindowGridShapeDataItem::axesColor()
{
	return QColor (
			(int)(m_axesColor[0] * 255),
			(int)(m_axesColor[1] * 255),
			(int)(m_axesColor[2] * 255));
}

void Post3dWindowGridShapeDataItem::setAxesColor(const QColor& color)
{
	m_axesColor[0] = color.redF();
	m_axesColor[1] = color.greenF();
	m_axesColor[2] = color.blueF();
}
*/

void Post3dWindowGridShapeDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	m_outlineActor->GetProperty()->SetLineWidth(selectedOutlineWidth);
	updateVisibility();
}

void Post3dWindowGridShapeDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	m_outlineActor->GetProperty()->SetLineWidth(normalOutlineWidth);
}

void Post3dWindowGridShapeDataItem::innerUpdateZScale(double scale)
{
	m_outlineActor->SetScale(1, 1, scale);
	m_wireframeActor->SetScale(1, 1, scale);
	m_indexTransform->Identity();
	m_indexTransform->Scale(1, 1, scale);
}

/*
void Post3dWindowGridShapeDataItem::updateAxesRegion()
{
	vtkPointSet* ps = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer()->data();
	if (ps == nullptr){return;}
	double bounds[6];
	ps->GetBounds(bounds);
	iRIC::fattenBounds(bounds, 0.1);
	m_axesActor->SetBounds(bounds);
}
*/
