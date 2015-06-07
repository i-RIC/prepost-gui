#include "post2dwindowarrowstructuredsettingdialog.h"
#include "post2dwindownodevectorarrowgroupstructureddataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/arrowsettingcontainer.h>
#include <misc/iricundostack.h>
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

Post2dWindowNodeVectorArrowGroupStructuredDataItem::Post2dWindowNodeVectorArrowGroupStructuredDataItem(Post2dWindowDataItem* p)
	: Post2dWindowNodeVectorArrowGroupDataItem(p)
{
	m_iSampleRate = 1;
	m_jSampleRate = 1;

	int dim[3];
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	dynamic_cast<vtkStructuredGrid*>(cont->data())->GetDimensions(dim);
	m_range.iMin = 0;
	m_range.iMax = dim[0] - 1;
	m_range.jMin = 0;
	m_range.jMax = dim[1] - 1;

	m_arrowExtract = vtkSmartPointer<vtkExtractGrid>::New();
}

Post2dWindowNodeVectorArrowGroupStructuredDataItem::~Post2dWindowNodeVectorArrowGroupStructuredDataItem()
{

}

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
	m_arrowExtract->SetSampleRate(m_iSampleRate, m_jSampleRate, 1);

	if (m_regionMode == StructuredGridRegion::rmCustom) {
		m_arrowExtract->SetVOI(m_range.iMin, m_range.iMax, m_range.jMin, m_range.jMax, 0, 0);
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
	vtkDoubleArray* vectorArray = vtkDoubleArray::SafeDownCast(tmpgrid->GetPointData()->GetArray(iRIC::toStr(m_currentSolution).c_str()));
	if (vectorArray == nullptr) {
		m_currentSolution = "";
		return;
	}
	QSet<vtkIdType> pointIds;
	double minlimitsqr = m_minimumValue * m_minimumValue;
	for (vtkIdType i = 0; i < tmpgrid->GetNumberOfPoints(); ++i) {
		bool active = true;
		if (m_regionMode == StructuredGridRegion::rmActive && IBCArray->GetValue(i) == 0) {
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
	dialog->setMapping(m_mapping);
	dialog->setColor(m_color);
	dialog->setScalarValue(m_scalarValueName);
	if (! cont->IBCExists()) {
		dialog->disableActive();
	}
	dialog->setRegionMode(m_regionMode);
	dialog->setRange(m_range);
	dialog->setSamplingRates(m_iSampleRate, m_jSampleRate);
	dialog->setLengthMode(m_lengthMode);
	dialog->setStandardValue(m_standardValue);
	dialog->setLegendLength(m_legendLength);
	dialog->setMinimumValue(m_minimumValue);
	dialog->setArrowSetting(m_arrowSetting);

	return dialog;
}

class Post2dWindowArrowStructuredSetProperty : public QUndoCommand
{
public:
	Post2dWindowArrowStructuredSetProperty(const QString& solutionName, Post2dWindowNodeVectorArrowGroupDataItem::Mapping mapping, const QColor& color, const QString& scalarName, StructuredGridRegion::RegionMode rm, StructuredGridRegion::Range2d range, int irate, int jrate, Post2dWindowNodeVectorArrowGroupDataItem::LengthMode lm, double stdVal, int legendLen, double minVal, const ArrowSettingContainer& acon, Post2dWindowNodeVectorArrowGroupStructuredDataItem* item)
		: QUndoCommand(QObject::tr("Update Arrow Setting")) {
		m_newSolutionName = solutionName;
		m_newMapping = mapping;
		m_newColor = color;
		m_newScalarName = scalarName;
		m_newRegionMode = rm;
		m_newRange = range;
		m_newISampleRate = irate;
		m_newJSampleRate = jrate;
		m_newLengthMode = lm;
		m_newStandardValue = stdVal;
		m_newLegendLength = legendLen;
		m_newMinimumValue = minVal;
		m_newArrowSetting = acon;

		m_oldSolutionName = item->m_currentSolution;
		m_oldMapping = item->m_mapping;
		m_oldColor = item->m_color;
		m_oldScalarName = item->m_scalarValueName;
		m_oldRegionMode = item->m_regionMode;
		m_oldRange = item->m_range;
		m_oldISampleRate = item->m_iSampleRate;
		m_oldJSampleRate = item->m_jSampleRate;
		m_oldLengthMode = item->m_lengthMode;
		m_oldStandardValue = item->m_standardValue;
		m_oldLegendLength = item->m_legendLength;
		m_oldMinimumValue = item->m_minimumValue;
		m_oldArrowSetting = item->m_arrowSetting;

		m_item = item;
	}
	void redo() {
		m_item->setIsCommandExecuting(true);
		m_item->setCurrentSolution(m_newSolutionName);
		m_item->m_mapping = m_newMapping;
		m_item->m_color = m_newColor;
		m_item->m_scalarValueName = m_newScalarName;
		m_item->m_regionMode = m_newRegionMode;
		m_item->m_range = m_newRange;
		m_item->m_iSampleRate = m_newISampleRate;
		m_item->m_jSampleRate = m_newJSampleRate;
		m_item->m_lengthMode = m_newLengthMode;
		m_item->m_standardValue = m_newStandardValue;
		m_item->m_legendLength = m_newLegendLength;
		m_item->m_minimumValue = m_newMinimumValue;
		m_item->m_arrowSetting = m_newArrowSetting;

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
	void undo() {
		m_item->setIsCommandExecuting(true);
		m_item->setCurrentSolution(m_oldSolutionName);
		m_item->m_mapping = m_oldMapping;
		m_item->m_color = m_oldColor;
		m_item->m_scalarValueName = m_oldScalarName;
		m_item->m_regionMode = m_oldRegionMode;
		m_item->m_range = m_oldRange;
		m_item->m_iSampleRate = m_oldISampleRate;
		m_item->m_jSampleRate = m_oldJSampleRate;
		m_item->m_lengthMode = m_oldLengthMode;
		m_item->m_standardValue = m_oldStandardValue;
		m_item->m_legendLength = m_oldLegendLength;
		m_item->m_minimumValue = m_oldMinimumValue;
		m_item->m_arrowSetting = m_oldArrowSetting;

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
private:
	QString m_newSolutionName;
	Post2dWindowNodeVectorArrowGroupDataItem::Mapping m_newMapping;
	QColor m_newColor;
	QString m_newScalarName;
	StructuredGridRegion::RegionMode m_newRegionMode;
	StructuredGridRegion::Range2d m_newRange;
	int m_newISampleRate;
	int m_newJSampleRate;
	Post2dWindowNodeVectorArrowGroupDataItem::LengthMode m_newLengthMode;
	double m_newStandardValue;
	int m_newLegendLength;
	double m_newMinimumValue;
	ArrowSettingContainer m_newArrowSetting;

	QString m_oldSolutionName;
	Post2dWindowNodeVectorArrowGroupDataItem::Mapping m_oldMapping;
	QColor m_oldColor;
	QString m_oldScalarName;
	StructuredGridRegion::RegionMode m_oldRegionMode;
	StructuredGridRegion::Range2d m_oldRange;
	int m_oldISampleRate;
	int m_oldJSampleRate;
	Post2dWindowNodeVectorArrowGroupDataItem::LengthMode m_oldLengthMode;
	double m_oldStandardValue;
	int m_oldLegendLength;
	double m_oldMinimumValue;
	ArrowSettingContainer m_oldArrowSetting;

	Post2dWindowNodeVectorArrowGroupStructuredDataItem* m_item;
};

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post2dWindowArrowStructuredSettingDialog* dialog = dynamic_cast<Post2dWindowArrowStructuredSettingDialog*>(propDialog);
	iRICUndoStack::instance().push(new Post2dWindowArrowStructuredSetProperty(dialog->solution(), dialog->mapping(), dialog->color(), dialog->scalarValue(), dialog->regionMode(), dialog->range(), dialog->iSamplingRate(), dialog->jSamplingRate(), dialog->lengthMode(), dialog->standardValue(), dialog->legendLength(), dialog->minimumValue(), dialog->arrowSetting(), this));
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Post2dWindowNodeVectorArrowGroupDataItem::doLoadFromProjectMainFile(node);

	QDomElement elem = node.toElement();
	m_iSampleRate = elem.attribute("iSampleRate").toInt();
	m_jSampleRate = elem.attribute("jSampleRate").toInt();
	m_range.iMin = elem.attribute("regionIMin").toInt();
	m_range.iMax = elem.attribute("regionIMax").toInt();
	m_range.jMin = elem.attribute("regionJMin").toInt();
	m_range.jMax = elem.attribute("regionJMax").toInt();
	updateActorSettings();
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post2dWindowNodeVectorArrowGroupDataItem::doSaveToProjectMainFile(writer);

	writer.writeAttribute("iSampleRate", QString::number(m_iSampleRate));
	writer.writeAttribute("jSampleRate", QString::number(m_jSampleRate));
	writer.writeAttribute("regionIMin", QString::number(m_range.iMin));
	writer.writeAttribute("regionIMax", QString::number(m_range.iMax));
	writer.writeAttribute("regionJMin", QString::number(m_range.jMin));
	writer.writeAttribute("regionJMax", QString::number(m_range.jMax));
}
