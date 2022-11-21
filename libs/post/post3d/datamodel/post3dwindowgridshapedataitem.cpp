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

Post3dWindowGridShapeDataItem::Post3dWindowGridShapeDataItem(Post3dWindowDataItem* parent) :
	Post3dWindowDataItem {tr("Grid shape"), QIcon(":/libs/guibase/images/iconPaper.svg"), parent}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

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
    vtkTextProperty* textProp = m_indexMapper->GetLabelTextProperty();
    iRIC::setupGridIndexTextProperty(textProp);
    m_setting.indexTextSetting.applySetting(textProp);

	m_indexActor->SetMapper(m_indexMapper);

	m_outlineActor->VisibilityOff();
	m_wireframeActor->VisibilityOff();
	m_indexActor->VisibilityOff();

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

	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr) {return;}
	if (cont->data() == nullptr) {return;}

	vtkPointSet* ps = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer()->data();
	vtkPointSet* labeldata = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer()->labelData();
	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*>(ps);

	vtkSmartPointer<vtkStructuredGrid> tmpgrid;
	switch (GridShapeEditDialog::Shape(m_setting.shape)) {
	case GridShapeEditDialog::Outline:
		m_outlineFilter->SetInputData(ps);
		m_outlineActor->GetProperty()->SetColor(m_setting.color);
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
		m_outlineActor->GetProperty()->SetColor(m_setting.color);
		m_actorCollection->AddItem(m_outlineActor);
		m_wireframeActor->GetProperty()->SetColor(m_setting.color);
		m_actorCollection->AddItem(m_wireframeActor);
		break;
	}
	if (m_setting.indexVisible) {
		m_indexTransformFilter->SetInputData(labeldata);
        m_setting.indexTextSetting.applySetting(m_indexMapper->GetLabelTextProperty());
		m_actor2DCollection->AddItem(m_indexActor);
	}
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
	m_setting.load(shapeNode);

	updateActorSettings();
}

void Post3dWindowGridShapeDataItem::saveShapeToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void Post3dWindowGridShapeDataItem::handleStandardItemDoubleClicked()
{
	showPropertyDialog();
}

QDialog* Post3dWindowGridShapeDataItem::propertyDialog(QWidget* parent)
{
	GridShapeEditDialog* dialog = new GridShapeEditDialog(parent);
	dialog->setSetting(m_setting);

	return dialog;
}

class Post3dWindowGridShapeDataSetSetting : public QUndoCommand
{
public:
	Post3dWindowGridShapeDataSetSetting(const GridShapeEditDialog::Setting& setting, Post3dWindowGridShapeDataItem* item) :
		QUndoCommand {QObject::tr("Update Grid Shape Setting")},
		m_newSetting {setting},
		m_oldSetting {item->m_setting},
		m_oldEnabled {item->isEnabled()},
		m_item {item}
	{}
	void redo() {
		m_item->setIsCommandExecuting(true);
		m_item->m_setting = m_newSetting;
		m_item->setEnabled(true);

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
	void undo() {
		m_item->setIsCommandExecuting(true);
		m_item->m_setting = m_oldSetting;
		m_item->setEnabled(m_oldEnabled);

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
private:
	GridShapeEditDialog::Setting m_newSetting;
	GridShapeEditDialog::Setting m_oldSetting;
	bool m_oldEnabled;

	Post3dWindowGridShapeDataItem* m_item;
};

void Post3dWindowGridShapeDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	GridShapeEditDialog* dialog = dynamic_cast<GridShapeEditDialog*>(propDialog);
	iRICUndoStack::instance().push(new Post3dWindowGridShapeDataSetSetting(dialog->setting(), this));
}

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
