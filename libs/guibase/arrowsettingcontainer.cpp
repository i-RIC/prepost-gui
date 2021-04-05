#include "arrowsettingcontainer.h"

#include <misc/stringtool.h>

#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkPointSet.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <QSettings>

#include <algorithm>

const int AUTO_AVERAGECOUNT = 20;

const int ArrowSettingContainer::DEFAULT_SAMPLING_RATE = 1;
const int ArrowSettingContainer::DEFAULT_SAMPLING_NUMBER = 100;

const double ArrowSettingContainer::DEFAULT_LEGEND_STANDARD = 1;
const int ArrowSettingContainer::DEFAULT_LEGEND_LENGTH = 100;
const double ArrowSettingContainer::DEFAULT_LEGEND_MINIMUM = 0.001;

const int ArrowSettingContainer::DEFAULT_ARROWSIZE = 8;
const int ArrowSettingContainer::DEFAULT_LINEWIDTH = 1;

ArrowSettingContainer::ArrowSettingContainer() :
	CompositeContainer({&target, &samplingMode, &samplingRate, &samplingNumber,
			&colorMode, &customColor, &colorTarget,
			&lengthMode, &standardValue, &legendLength, &minimumValue,
			&arrowSize, &lineWidth, &scalarBarSetting}),
	target {"attribute"},
	samplingMode {"samplingMode", SamplingMode::All},
	samplingRate {"samplingRate", DEFAULT_SAMPLING_RATE},
	samplingNumber {"samplingNumber", DEFAULT_SAMPLING_NUMBER},
	colorMode {"colorMode", ColorMode::Custom},
	customColor {"customColor", Qt::black},
	colorTarget {"colorAttribute"},
	lengthMode {"lengthMode", LengthMode::Auto},
	standardValue {"standardValue", DEFAULT_LEGEND_STANDARD},
	legendLength {"legendLength", DEFAULT_LEGEND_LENGTH},
	minimumValue {"minimumValue", DEFAULT_LEGEND_MINIMUM},
	arrowSize {"arrowSize", DEFAULT_ARROWSIZE},
	lineWidth {"arrowLineWidth", DEFAULT_LINEWIDTH},
	oldCameraScale {"oldCameraScale", 1}
{
	QSettings setting;
	customColor = setting.value("graphics/vectorcolor", QColor(Qt::black)).value<QColor>();
}

ArrowSettingContainer::ArrowSettingContainer(const ArrowSettingContainer& c) :
	ArrowSettingContainer()
{
	copyValue(c);
}

ArrowSettingContainer::~ArrowSettingContainer()
{}

ArrowSettingContainer& ArrowSettingContainer::operator=(const ArrowSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& ArrowSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ArrowSettingContainer&> (c));
}

double ArrowSettingContainer::scaleFactor(double stdDistance) const
{
	double a = 1.0 / stdDistance;
	return legendLength / (a * standardValue);
}

vtkPolyData* ArrowSettingContainer::buildFilteredData(vtkPointSet* data) const
{
	auto ret = vtkPolyData::New();
	ret->GetPointData()->CopyAllocate(data->GetPointData());

	auto inPoints = data->GetPoints();
	auto outPoints = vtkSmartPointer<vtkPoints>::New();
	outPoints->SetDataTypeToDouble();

	double minSqr = minimumValue * minimumValue;
	vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();
	vtkDataArray* vecArray = data->GetPointData()->GetArray(iRIC::toStr(target).c_str());

	vtkIdType id = 0;
	for (vtkIdType i = 0; i < inPoints->GetNumberOfPoints(); ++i) {
		double val = 0;
		for (int j = 0; j < vecArray->GetNumberOfComponents(); ++j) {
			double tmpv = vecArray->GetComponent(i, j);
			val += tmpv * tmpv;
		}
		if (val < minSqr) {continue;}

		outPoints->InsertNextPoint(inPoints->GetPoint(i));
		ret->GetPointData()->CopyData(data->GetPointData(), i, id);
		ca->InsertNextCell(1, &id);
		++ id;
	}
	ret->SetPoints(outPoints);
	ret->SetVerts(ca);

	return ret;
}

void ArrowSettingContainer::updateStandardValueIfNeeded(vtkPointSet* data)
{
	if (lengthMode == ArrowSettingContainer::LengthMode::Custom) {return;}
	if (target == "") {return;}
	if (data == nullptr) {return;}

	vtkPointData* pd = data->GetPointData();
	vtkDataArray* da = pd->GetArray(iRIC::toStr(target).c_str());

	std::vector<double> lens;

	for (vtkIdType i = 0; i < da->GetNumberOfTuples(); ++i) {
		double val = 0;
		for (int j = 0; j < da->GetNumberOfComponents(); ++j) {
			double tmpv = da->GetComponent(i, j);
			val += tmpv * tmpv;
		}
		lens.push_back(sqrt(val));
	}

	std::sort(lens.begin(), lens.end());

	int count = AUTO_AVERAGECOUNT;
	if (count > lens.size()) {count = lens.size();}

	double sum = 0;
	for (int i = 0; i < count; ++i) {
		sum += lens.at(lens.size() - i - 1);
	}
	double average = sum / count;
	if (average == 0) {average = 1.0;}

	int p = 0;
	double p2 = 1;
	while (average > 10) {
		average /= 10.;
		++ p;
		p2 = 10;
	}
	while (average < 1) {
		average *= 10;
		++ p;
		p2 = 0.1;
	}
	average = static_cast<int>(average);
	for (int i = 0; i < p; ++i) {
		average *= p2;
	}
	// now average is calculated.
	standardValue = average;
}
