#include "post2dwindowarrowstructuredsettingdialog.h"
#include "post2dwindownodevectorarrowgroupstructureddataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindownodevectorarrowgroupstructureddataitem_setsettingcommand.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>

#include <QDomElement>
#include <QSet>
#include <QUndoCommand>

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkQuad.h>
#include <vtkStructuredGrid.h>
#include <vtkVertex.h>

Post2dWindowNodeVectorArrowGroupStructuredDataItem::Post2dWindowNodeVectorArrowGroupStructuredDataItem(Post2dWindowDataItem* p) :
	Post2dWindowNodeVectorArrowGroupDataItem {p}
{
	int dim[3];
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	dynamic_cast<vtkStructuredGrid*>(cont->data())->GetDimensions(dim);

	m_setting.range.iMin = 0;
	m_setting.range.iMax = dim[0] - 1;
	m_setting.range.jMin = 0;
	m_setting.range.jMax = dim[1] - 1;

	m_arrowExtract = vtkSmartPointer<vtkExtractGrid>::New();
}

Post2dWindowNodeVectorArrowGroupStructuredDataItem::~Post2dWindowNodeVectorArrowGroupStructuredDataItem()
{}

QDialog* Post2dWindowNodeVectorArrowGroupStructuredDataItem::propertyDialog(QWidget* p)
{
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {
		return nullptr;
	}
	Post2dWindowArrowStructuredSettingDialog* dialog = new Post2dWindowArrowStructuredSettingDialog(p);
	dialog->setZoneData(cont);
	if (! cont->IBCExists()) {
		dialog->disableActive();
	}
	dialog->setSetting(m_setting);

	return dialog;
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post2dWindowArrowStructuredSettingDialog* dialog = dynamic_cast<Post2dWindowArrowStructuredSettingDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), this), this, true);
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::updateActivePoints()
{
	auto& s = m_setting;
	m_activePoints->Initialize();
	vtkSmartPointer<vtkPoints> outPoints = vtkSmartPointer<vtkPoints>::New();
	outPoints->SetDataTypeToDouble();
	m_activePoints->SetPoints(outPoints);

	vtkPointSet* ps = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer()->data();
	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*>(ps);
	int dims[3];
	grid->GetDimensions(dims);

	m_arrowExtract->SetInputData(ps);
	m_arrowExtract->SetSampleRate(s.iSampleRate, s.jSampleRate, 1);

	if (s.regionMode == StructuredGridRegion::rmCustom) {
		const auto& r = s.range;
		m_arrowExtract->SetVOI(r.iMin, r.iMax, r.jMin, r.jMax, 0, 0);
	} else {
		m_arrowExtract->SetVOI(0, dims[0] - 1, 0, dims[1] - 1, 0, dims[2] - 1);
	}
	m_arrowExtract->Update();
	vtkStructuredGrid* tmpgrid = m_arrowExtract->GetOutput();

	vtkDataArray* da = tmpgrid->GetPointData()->GetArray(iRIC::toStr(PostZoneDataContainer::IBC).c_str());
	vtkIntArray* IBCArray = nullptr;
	if (da != nullptr) {
		IBCArray = vtkIntArray::SafeDownCast(da);
	}
	vtkDoubleArray* vectorArray = vtkDoubleArray::SafeDownCast(tmpgrid->GetPointData()->GetArray(iRIC::toStr(m_setting.target).c_str()));
	if (vectorArray == nullptr) {
		s.target = "";
		return;
	}
	QSet<vtkIdType> pointIds;
	const double& min = s.minimumValue;
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

Post2dWindowNodeVectorArrowSetting& Post2dWindowNodeVectorArrowGroupStructuredDataItem::setting()
{
	return m_setting;
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	updateScaleFactor();
	setTarget(m_setting.target);
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}
