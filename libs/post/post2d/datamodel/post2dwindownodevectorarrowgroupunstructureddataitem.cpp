#include "post2dwindowarrowunstructuredsettingdialog.h"
#include "post2dwindownodevectorarrowgroupunstructureddataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindownodevectorarrowgroupunstructureddataitem_setsettingcommand.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>

#include <QDomElement>
#include <QSet>
#include <QUndoCommand>

#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkStructuredGrid.h>
#include <vtkVertex.h>

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Post2dWindowNodeVectorArrowGroupUnstructuredDataItem(Post2dWindowDataItem* p) :
	Post2dWindowNodeVectorArrowGroupDataItem(p)
{
	m_arrowMask = vtkSmartPointer<vtkMaskPoints>::New();
}

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::~Post2dWindowNodeVectorArrowGroupUnstructuredDataItem()
{}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::updateActivePoints()
{
	auto& s = m_setting;

	m_activePoints->Initialize();
	vtkSmartPointer<vtkPoints> outPoints = vtkSmartPointer<vtkPoints>::New();
	outPoints->SetDataTypeToDouble();
	m_activePoints->SetPoints(outPoints);

	vtkPointSet* ps = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer()->data();
	m_arrowMask->SetInputData(ps);

	if (s.samplingMode == ArrowSettingContainer::SamplingMode::All) {
		m_arrowMask->SetOnRatio(1);
		m_arrowMask->RandomModeOff();
		m_arrowMask->SetMaximumNumberOfPoints(ps->GetNumberOfPoints());
	} else if (s.samplingMode == ArrowSettingContainer::SamplingMode::Rate) {
		m_arrowMask->SetOnRatio(s.samplingRate);
		m_arrowMask->RandomModeOff();
		m_arrowMask->SetMaximumNumberOfPoints(ps->GetNumberOfPoints());
	} else if (s.samplingMode == ArrowSettingContainer::SamplingMode::Number) {
		m_arrowMask->RandomModeOn();
		m_arrowMask->SetMaximumNumberOfPoints(s.samplingNumber);
	} else {
		m_arrowMask->SetOnRatio(1);
		m_arrowMask->RandomModeOff();
		m_arrowMask->SetMaximumNumberOfPoints(ps->GetNumberOfPoints());
	}
	m_arrowMask->Update();
	vtkPointSet* tmpgrid = m_arrowMask->GetOutput();

	vtkDataArray* da = tmpgrid->GetPointData()->GetArray(iRIC::toStr(PostZoneDataContainer::IBC).c_str());
	vtkIntArray* IBCArray = nullptr;
	if (da != nullptr) {
		IBCArray = vtkIntArray::SafeDownCast(da);
	}
	vtkDoubleArray* vectorArray = vtkDoubleArray::SafeDownCast(tmpgrid->GetPointData()->GetArray(iRIC::toStr(s.target).c_str()));
	if (vectorArray == nullptr) {
		s.target = "";
		return;
	}
	QSet<vtkIdType> pointIds;
	double min = s.minimumValue;
	double minlimitsqr = min * min;
	for (vtkIdType i = 0; i < tmpgrid->GetNumberOfPoints(); ++i) {
		bool active = true;
		if (s.regionMode == StructuredGridRegion::rmActive && IBCArray->GetValue(i) == 0) {
			active = false;
		}
		double val = 0;
		for (int j = 0; j < vectorArray->GetNumberOfComponents(); ++j) {
			double tmpval = vectorArray->GetComponent(i, j);
			val += tmpval * tmpval;
		}
		if (val < minlimitsqr) {
			active = false;
		}
		if (active) {
			pointIds.insert(i);
		}
	}
	vtkPointData* inPD = tmpgrid->GetPointData();
	vtkPointData* outPD = m_activePoints->GetPointData();
	vtkPoints* inPoints = tmpgrid->GetPoints();
	vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();

	outPD->CopyAllocate(inPD, pointIds.size());
	vtkIdType newId = 0;
	for (auto it = pointIds.begin(); it != pointIds.end(); ++it) {
		vtkIdType pointid = *it;
		outPoints->InsertNextPoint(inPoints->GetPoint(pointid));
		outPD->CopyData(inPD, pointid, newId);
		ca->InsertNextCell(1, &newId);
		++ newId;
	}
	m_activePoints->SetVerts(ca);
	m_activePoints->Modified();
}

Post2dWindowNodeVectorArrowSetting& Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::setting()
{
	return m_setting;
}

QDialog* Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::propertyDialog(QWidget* p)
{
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {
		return nullptr;
	}
	if (! cont->vectorValueExists()) {
		return nullptr;
	}
	Post2dWindowArrowUnstructuredSettingDialog* dialog = new Post2dWindowArrowUnstructuredSettingDialog(p);
	dialog->setZoneData(cont);
	if (! cont->IBCExists()) {
		dialog->disableActive();
	}
	dialog->setSetting(m_setting);
	dialog->setColorBarTitleMap(m_colorbarTitleMap);

	return dialog;
}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post2dWindowArrowUnstructuredSettingDialog* dialog = dynamic_cast<Post2dWindowArrowUnstructuredSettingDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), dialog->scalarBarTitle(), this), this, true);
}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	updateScaleFactor();
	setTarget(m_setting.target);
}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}
