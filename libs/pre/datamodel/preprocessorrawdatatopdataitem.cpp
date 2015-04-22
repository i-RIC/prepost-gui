#include "preprocessorrawdatatopdataitem.h"
#include "preprocessorrawdatagroupdataitem.h"
#include "preprocessorrawdatacomplexgroupdataitem.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridrelatedconditionnodegroupdataitem.h"
#include "preprocessorgridrelatedconditionnodedataitem.h"
#include "preprocessorgridrelatedconditioncellgroupdataitem.h"
#include "preprocessorgridrelatedconditioncelldataitem.h"
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridrelatedcondition.h>
#include <guicore/solverdef/solverdefinitiongridrelatedcomplexcondition.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include "../misc/preprocessorscalarbarlegendboxsettingdialog.h"
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <guicore/scalarstocolors/colortransferfunctioncontainer.h>
#include <misc/stringtool.h>
#include <guibase/graphicsmisc.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/pre/base/preprocessorrawdatagroupdataiteminterface.h>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkTextProperty.h>
#include <vtkColorTransferFunction.h>
#include <vtkSphereSource.h>
#include <vtkScalarBarActor.h>

#include <QIcon>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <iriclib.h>

PreProcessorRawDataTopDataItem::PreProcessorRawDataTopDataItem(PreProcessorDataItem* parent)
	: PreProcessorRawDataTopDataItemInterface(tr("Geographic Data"), QIcon(":/libs/guibase/images/iconFolder.png"), parent)
{
	m_subFolder = "geographicdata";

	m_isDeletable = false;
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);
	m_standardItemCopy = m_standardItem->clone();

	// add child nodes.
	QList<SolverDefinitionGridRelatedCondition*> list = gridType()->gridRelatedConditions();
	QList<SolverDefinitionGridRelatedComplexCondition*> list2 = gridType()->gridRelatedComplexConditions();

	// node simple items
	for (auto it = list.begin(); it != list.end(); ++it){
		SolverDefinitionGridRelatedCondition* cond = *it;
		if (cond->position() != SolverDefinitionGridRelatedCondition::Node){continue;}
		PreProcessorRawDataGroupDataItem* i = new PreProcessorRawDataGroupDataItem(cond, this);
		m_childItems.append(i);
		m_itemNameMap.insert((*it)->name(), i);
	}
	// node complex items
	for (auto it2 = list2.begin(); it2 != list2.end(); ++it2){
		SolverDefinitionGridRelatedComplexCondition* cond = *it2;
		if (cond->position() != SolverDefinitionGridRelatedCondition::Node){continue;}
		PreProcessorRawDataComplexGroupDataItem* i = new PreProcessorRawDataComplexGroupDataItem(cond, this);
		m_childItems.append(i);
		m_itemNameMap.insert((*it2)->name(), i);
	}
	// cell simple items
	for (auto it = list.begin(); it != list.end(); ++it){
		SolverDefinitionGridRelatedCondition* cond = *it;
		if (cond->position() != SolverDefinitionGridRelatedCondition::CellCenter){continue;}
		PreProcessorRawDataGroupDataItem* i = new PreProcessorRawDataGroupDataItem(cond, this);
		m_childItems.append(i);
		m_itemNameMap.insert((*it)->name(), i);
	}
	// cell complex items
	for (auto it2 = list2.begin(); it2 != list2.end(); ++it2){
		SolverDefinitionGridRelatedComplexCondition* cond = *it2;
		if (cond->position() != SolverDefinitionGridRelatedCondition::CellCenter){continue;}
		PreProcessorRawDataComplexGroupDataItem* i = new PreProcessorRawDataComplexGroupDataItem(cond, this);
		m_childItems.append(i);
		m_itemNameMap.insert((*it2)->name(), i);
	}
	m_titleTextSetting.setPrefix("title");
	m_labelTextSetting.setPrefix("label");

	// for scalar bar / legend box
	m_visible = true;
	// first, no scalar bar / legend box shown.
	m_condition = nullptr;
	setupActors();
}

PreProcessorRawDataTopDataItem::~PreProcessorRawDataTopDataItem()
{
	m_scalarBarWidget->SetInteractor(nullptr);
	m_legendBoxWidget->SetInteractor(nullptr);
}

void PreProcessorRawDataTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_titleTextSetting.load(node);
	m_labelTextSetting.load(node);
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i){
		QDomElement child = children.at(i).toElement();
		QString name = child.attribute("name");
		PreProcessorRawDataGroupDataItemInterface* item = m_itemNameMap.value(name);
		if (item != nullptr){
			item->loadFromProjectMainFile(child);
		}
	}
}

void PreProcessorRawDataTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_titleTextSetting.save(writer);
	m_labelTextSetting.save(writer);
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		writer.writeStartElement("RawDataGroup");
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

const QList<PreProcessorRawDataGroupDataItemInterface*> PreProcessorRawDataTopDataItem::groupDataItems() const
{
	QList<PreProcessorRawDataGroupDataItemInterface*> ret;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		PreProcessorRawDataGroupDataItemInterface* item = dynamic_cast<PreProcessorRawDataGroupDataItemInterface*>(*it);
		ret.append(item);
	}
	return ret;
}

PreProcessorRawDataGroupDataItemInterface* PreProcessorRawDataTopDataItem::groupDataItem(const QString& name)
{
	return m_itemNameMap.value(name, nullptr);
}

void PreProcessorRawDataTopDataItem::informValueRangeChange(const QString& name)
{
	emit valueRangeChanged(name);
}

void PreProcessorRawDataTopDataItem::informDataChange()
{
	emit dataChanged();
}

void PreProcessorRawDataTopDataItem::setupScalarBar()
{
	PreProcessorScalarBarLegendBoxSettingDialog* dialog = new PreProcessorScalarBarLegendBoxSettingDialog(preProcessorWindow());
	if (m_condition != nullptr){
		if (dynamic_cast<SolverDefinitionGridRelatedComplexCondition*>(m_condition) != nullptr || m_condition->isOption()){
			PreProcessorRawDataGroupDataItem* gItem = dynamic_cast<PreProcessorRawDataGroupDataItem*> (groupDataItem(m_condition->name()));
			ScalarBarSetting& setting = gItem->scalarBarSetting();
			setting.loadFromRepresentation(m_legendBoxWidget->GetLegendBoxRepresentation());
		} else {
			PreProcessorRawDataGroupDataItem* gItem = dynamic_cast<PreProcessorRawDataGroupDataItem*> (groupDataItem(m_condition->name()));
			ScalarBarSetting& setting = gItem->scalarBarSetting();
			setting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
		}
	}
	dialog->setupComboBox(this);
	dialog->setActor2DVisibility(m_visible);
	dialog->setCondition(m_condition);
	dialog->setTitleTextSetting(m_titleTextSetting);
	dialog->setLabelTextSetting(m_labelTextSetting);
	if (dialog->exec() == QDialog::Accepted){
		m_visible = dialog->actor2DVisibility();
		m_condition = dialog->condition();
		m_titleTextSetting = dialog->titleTextSetting();
		m_labelTextSetting = dialog->labelTextSetting();
		updateActorSettings();
	}
	delete dialog;
}

void PreProcessorRawDataTopDataItem::setupActors()
{
	vtkRenderWindowInteractor* iren = renderer()->GetRenderWindow()->GetInteractor();
	m_scalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
	m_scalarBarWidget->SetEnabled(0);
	m_scalarBarWidget->SetInteractor(iren);

	m_legendBoxWidget = vtkSmartPointer<vtkLegendBoxWidget>::New();
	m_legendBoxWidget->SetEnabled(0);
	m_legendBoxWidget->SetInteractor(iren);

	QList<PreProcessorRawDataGroupDataItemInterface*> groups = groupDataItems();
	QString attName;

	// for legend box
	for (auto it = groups.begin(); it != groups.end(); ++it){
		if (! (*it)->condition()->isOption()) continue;
		attName = (*it)->condition()->name();
		break;
	}
	updateActorSettings();
}

void PreProcessorRawDataTopDataItem::updateActorSettings()
{
	if (m_scalarBarWidget == nullptr){return;}
	if (m_legendBoxWidget == nullptr){return;}
	m_scalarBarWidget->SetEnabled(0);
	m_legendBoxWidget->SetEnabled(0);

	if (m_condition == nullptr){return;}
	PreProcessorRawDataGroupDataItem* rdgItem = dynamic_cast<PreProcessorRawDataGroupDataItem*> (groupDataItem(m_condition->name()));
	if (rdgItem == nullptr) return;

	if (dynamic_cast<SolverDefinitionGridRelatedComplexCondition*>(m_condition) != nullptr  || m_condition->isOption()){
		// discrete
		if (! m_visible) return;
		m_legendBoxWidget->SetEnabled(1);

		PreProcessorRawDataGroupDataItemInterface* gItem = groupDataItem(m_condition->name());
		if (gItem == nullptr) return;
		ScalarBarSetting& sbSetting = dynamic_cast<PreProcessorRawDataGroupDataItem*>(gItem)->scalarBarSetting();
		sbSetting.saveToRepresentation(m_legendBoxWidget->GetLegendBoxRepresentation());
		m_labelTextSetting.applySetting(m_legendBoxWidget->GetLegendBoxActor()->GetEntryTextProperty());

		updateLegendBoxItems();
	} else {
		// continuous
		if (! m_visible) return;
		m_scalarBarWidget->SetEnabled(1);
		PreProcessorRawDataGroupDataItemInterface* gItem = groupDataItem(m_condition->name());
		if (gItem == nullptr) return;
		ScalarBarSetting& sbSetting = dynamic_cast<PreProcessorRawDataGroupDataItem*>(gItem)->scalarBarSetting();
		sbSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
		PreProcessorGridTypeDataItem* gtItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent());
		LookupTableContainer* cont = dynamic_cast<LookupTableContainer*>(gtItem->scalarsToColors(m_condition->name()));
		if (cont == nullptr) return;
		vtkScalarBarActor* scalarBarActor = m_scalarBarWidget->GetScalarBarActor();
		scalarBarActor->SetLookupTable(cont->vtkObj());
		scalarBarActor->SetNumberOfLabels(sbSetting.numberOfLabels);
		scalarBarActor->SetTitle(iRIC::toStr(rdgItem->title()).c_str());
		scalarBarActor->SetLabelFormat(iRIC::toStr(sbSetting.labelFormat).c_str());
		m_titleTextSetting.applySetting(scalarBarActor->GetTitleTextProperty());
		m_labelTextSetting.applySetting(scalarBarActor->GetLabelTextProperty());
	}
}

void PreProcessorRawDataTopDataItem::updateLegendBoxItems()
{
	if (m_condition == nullptr){return;}
	PreProcessorGridTypeDataItem* gtItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent());
	ColorTransferFunctionContainer* ctfCont = dynamic_cast<ColorTransferFunctionContainer*>(gtItem->scalarsToColors(m_condition->name()));
	vtkLegendBoxActor* lActor = m_legendBoxWidget->GetLegendBoxActor();
	lActor->SetNumberOfEntries(ctfCont->englishEnumerations().size());
	vtkColorTransferFunction* ctf = dynamic_cast<vtkColorTransferFunction*>(ctfCont->vtkObj());
	int index = 0;
	double color[3] = {0, 0, 0};
	// set entries
	for (auto mapIt = ctfCont->englishEnumerations().begin(); mapIt != ctfCont->englishEnumerations().end(); mapIt++){
		vtkSphereSource* sphere = vtkSphereSource::New();
		double num = mapIt.key();
		QString label = mapIt.value();
		ctf->GetColor(num, color);
		sphere->Update();
		lActor->SetEntry(index++, sphere->GetOutput(), iRIC::toStr(label).c_str(), color);
		sphere->Delete();
	}
}

QStringList PreProcessorRawDataTopDataItem::getRawDatasNotMapped()
{
	QStringList ret;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		PreProcessorRawDataGroupDataItem* item = dynamic_cast<PreProcessorRawDataGroupDataItem*>(*it);
		ret.append(item->getRawDatasNotMapped());
	}
	return ret;
}

void PreProcessorRawDataTopDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	m_scalarBarWidget->SetRepositionable(1);
	m_legendBoxWidget->SetRepositionable(1);
}

void PreProcessorRawDataTopDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	m_scalarBarWidget->SetRepositionable(0);
	m_legendBoxWidget->SetRepositionable(0);
}

void PreProcessorRawDataTopDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	v->standardKeyPressEvent(event);
}

void PreProcessorRawDataTopDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	v->standardKeyReleaseEvent(event);
}

void PreProcessorRawDataTopDataItem::mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseDoubleClickEvent(event);
}

void PreProcessorRawDataTopDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
}

void PreProcessorRawDataTopDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void PreProcessorRawDataTopDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
}

void PreProcessorRawDataTopDataItem::saveToCgnsFile(const int fn)
{
	cg_iRIC_GotoRawDataTop(fn);
	GraphicsWindowDataItem::saveToCgnsFile(fn);

	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		PreProcessorRawDataGroupDataItem* gItem =
				dynamic_cast<PreProcessorRawDataGroupDataItem*>(*it);
		gItem->saveComplexGroupsToCgnsFile(fn);
	}
}
