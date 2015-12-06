#include "post2dwindowarrowstructuredsettingdialog.h"
#include "post2dwindownodevectorarrowgroupstructureddataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/arrowsettingcontainer.h>
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

Post2dWindowNodeVectorArrowGroupStructuredDataItem::Setting::Setting() :
	CompositeContainer ({&iSampleRate, &jSampleRate, &range}),
	iSampleRate {"iSampleRate", 1},
	jSampleRate {"jSampleRate", 1},
	range {}
{
	range.setPrefix("region");
}

Post2dWindowNodeVectorArrowGroupStructuredDataItem::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

Post2dWindowNodeVectorArrowGroupStructuredDataItem::Setting& Post2dWindowNodeVectorArrowGroupStructuredDataItem::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

Post2dWindowNodeVectorArrowGroupStructuredDataItem::Post2dWindowNodeVectorArrowGroupStructuredDataItem(Post2dWindowDataItem* p) :
	Post2dWindowNodeVectorArrowGroupDataItem {p}
{
	int dim[3];
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	dynamic_cast<vtkStructuredGrid*>(cont->data())->GetDimensions(dim);
	m_stSetting.range.iMin = 0;
	m_stSetting.range.iMax = dim[0] - 1;
	m_stSetting.range.jMin = 0;
	m_stSetting.range.jMax = dim[1] - 1;

	m_arrowExtract = vtkSmartPointer<vtkExtractGrid>::New();
}

Post2dWindowNodeVectorArrowGroupStructuredDataItem::~Post2dWindowNodeVectorArrowGroupStructuredDataItem()
{}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::updateActivePoints()
{
	m_activePoints->Initialize();
	vtkSmartPointer<vtkPoints> outPoints = vtkSmartPointer<vtkPoints>::New();
	outPoints->SetDataTypeToDouble();
	m_activePoints->SetPoints(outPoints);

	vtkPointSet* ps = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer()->data();
	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*>(ps);
	int dims[3];
	grid->GetDimensions(dims);

	m_arrowExtract->SetInputData(ps);
	m_arrowExtract->SetSampleRate(m_stSetting.iSampleRate, m_stSetting.jSampleRate, 1);

	if (m_setting.regionMode == StructuredGridRegion::rmCustom) {
		const auto& r = m_stSetting.range;
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
	vtkDoubleArray* vectorArray = vtkDoubleArray::SafeDownCast(tmpgrid->GetPointData()->GetArray(iRIC::toStr(m_setting.currentSolution).c_str()));
	if (vectorArray == nullptr) {
		m_setting.currentSolution = "";
		return;
	}
	QSet<vtkIdType> pointIds;
	const double& min = m_setting.minimumValue;
	double minlimitsqr = min * min;
	for (vtkIdType i = 0; i < tmpgrid->GetNumberOfPoints(); ++i) {
		bool active = true;
		if (m_setting.regionMode == StructuredGridRegion::rmActive && IBCArray->GetValue(i) == 0) {
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
	dialog->setSettings(m_setting, m_stSetting);

	return dialog;
}

class Post2dWindowNodeVectorArrowGroupStructuredDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const Post2dWindowNodeVectorArrowGroupDataItem::Setting& s, const Post2dWindowNodeVectorArrowGroupStructuredDataItem::Setting& sts, Post2dWindowNodeVectorArrowGroupStructuredDataItem* item) :
		QUndoCommand {QObject::tr("Update Arrow Setting")},
		m_newSetting {s},
		m_newStSetting {sts},
		m_oldSetting {item->m_setting},
		m_oldStSetting {item->m_stSetting},
		m_item {item}
	{}
	void redo() {
		m_item->m_setting = m_newSetting;
		m_item->m_stSetting = m_newStSetting;
		m_item->setTarget(m_newSetting.currentSolution);
		m_item->updateActorSettings();
	}
	void undo() {
		m_item->m_setting = m_oldSetting;
		m_item->m_stSetting = m_oldStSetting;
		m_item->setTarget(m_oldSetting.currentSolution);
		m_item->updateActorSettings();
	}
private:
	Post2dWindowNodeVectorArrowGroupDataItem::Setting m_newSetting;
	Post2dWindowNodeVectorArrowGroupStructuredDataItem::Setting m_newStSetting;

	Post2dWindowNodeVectorArrowGroupDataItem::Setting m_oldSetting;
	Post2dWindowNodeVectorArrowGroupStructuredDataItem::Setting m_oldStSetting;

	Post2dWindowNodeVectorArrowGroupStructuredDataItem* m_item;
};

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post2dWindowArrowStructuredSettingDialog* dialog = dynamic_cast<Post2dWindowArrowStructuredSettingDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), dialog->stSetting(), this), this, true);
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Post2dWindowNodeVectorArrowGroupDataItem::doLoadFromProjectMainFile(node);

	m_stSetting.load(node);
	updateActorSettings();
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post2dWindowNodeVectorArrowGroupDataItem::doSaveToProjectMainFile(writer);

	m_stSetting.save(writer);
}
