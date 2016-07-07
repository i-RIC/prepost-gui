#include "post2dwindowgridshapedataitem.h"
#include "post2dwindowzonedataitem.h"
#include "post2dwindowzonedataitem.h"

#include "private/post2dwindowgridshapedataitem_setsettingcommand.h"

#include <guibase/widget/coloreditwidget.h>
#include <guibase/graphicsmisc.h>
#include <guibase/widget/gridshapeeditdialog.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

#include <vtkActor.h>
#include <vtkActor2DCollection.h>
#include <vtkActorCollection.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
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

#define LABEL "label2d"

Post2dWindowGridShapeDataItem::Post2dWindowGridShapeDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {tr("Grid shape"), QIcon(":/libs/guibase/images/iconPaper.png"), parent}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

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
	iRIC::setupGridIndexTextProperty(m_indexMapper->GetLabelTextProperty());

	m_indexActor->SetMapper(m_indexMapper);

	m_outlineActor->VisibilityOff();
	m_wireframeActor->VisibilityOff();
	m_indexActor->VisibilityOff();

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

	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}
	vtkPointSet* ps = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->filteredData();

	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*>(cont->data());
	if (grid != 0){
		vtkSmartPointer<vtkStructuredGrid> tmpgrid = grid;
		switch (GridShapeEditDialog::Shape(m_setting.shape)) {
		case GridShapeEditDialog::Outline:
			m_outlineFilter->SetInputData(tmpgrid);
			m_outlineActor->GetProperty()->SetColor(m_setting.color);
			m_actorCollection->AddItem(m_outlineActor);
			break;

		case GridShapeEditDialog::Wireframe:
			m_outlineFilter->SetInputData(tmpgrid);
			m_outlineActor->GetProperty()->SetColor(m_setting.color);
			m_actorCollection->AddItem(m_outlineActor);
			m_wireframeMapper->SetInputData(ps);
			m_wireframeActor->GetProperty()->SetColor(m_setting.color);
			m_actorCollection->AddItem(m_wireframeActor);
			break;
		}
	} else {
		m_wireframeMapper->SetInputData(ps);
		m_wireframeActor->GetProperty()->SetColor(m_setting.color);
		m_actorCollection->AddItem(m_wireframeActor);
	}
	if (m_setting.indexVisible) {
		vtkPointSet* labeldata = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer()->labelData();
		m_indexMapper->SetInputData(labeldata);
		m_indexMapper->GetLabelTextProperty()->SetColor(m_setting.indexColor);
		m_actor2DCollection->AddItem(m_indexActor);
	}
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
	m_setting.load(shapeNode);

	updateActorSettings();
}

void Post2dWindowGridShapeDataItem::saveShapeToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void Post2dWindowGridShapeDataItem::handleStandardItemDoubleClicked()
{
	showPropertyDialog();
}

QDialog* Post2dWindowGridShapeDataItem::propertyDialog(QWidget* p)
{
	GridShapeEditDialog* dialog = new GridShapeEditDialog(p);
	dialog->setSetting(m_setting);

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (dynamic_cast<vtkStructuredGrid*> (cont->data()) == nullptr) {
		dialog->hideShape();
	}
	return dialog;
}

void Post2dWindowGridShapeDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	GridShapeEditDialog* dialog = dynamic_cast<GridShapeEditDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), this), this, true);
}

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
