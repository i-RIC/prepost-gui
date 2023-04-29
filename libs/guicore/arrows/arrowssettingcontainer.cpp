#include "arrowssettingcontainer.h"

#include "../datamodel/vtkgraphicsview.h"

#include <misc/stringtool.h>

#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkGlyph3D.h>
#include <vtkHedgeHog.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkWarpVector.h>

#include <QSettings>

namespace {

const int AUTO_AVERAGECOUNT = 20; // Use 20 points with the longest arrow lengths to calculate standard length

} // namespace

const double ArrowsSettingContainer::DEFAULT_LEGEND_STANDARD = 1;
const int ArrowsSettingContainer::DEFAULT_LEGEND_LENGTH = 100;
const double ArrowsSettingContainer::DEFAULT_LEGEND_MINIMUM = 0.001;

const int ArrowsSettingContainer::DEFAULT_ARROWSIZE = 8;
const int ArrowsSettingContainer::DEFAULT_LINEWIDTH = 1;

ArrowsSettingContainer::ArrowsSettingContainer() :
	CompositeContainer({&target,
										 &colorMode, &customColor, &colorTarget,
										 &lengthMode, &standardValue, &legendLength, &minimumValue,
										 &arrowSize, &lineWidth, &legend}),
	target {"attribute"},
	colorMode {"colorMode", ColorMode::Custom},
	customColor {"customColor", Qt::black},
	colorTarget {"colorAttribute"},
	lengthMode {"lengthMode", LengthMode::Auto},
	standardValue {"standardValue", DEFAULT_LEGEND_STANDARD},
	legendLength {"legendLength", DEFAULT_LEGEND_LENGTH},
	minimumValue {"minimumValue", DEFAULT_LEGEND_MINIMUM},
	arrowSize {"arrowSize", DEFAULT_ARROWSIZE},
	lineWidth {"arrowLineWidth", DEFAULT_LINEWIDTH},
	legend {}
{
	QSettings settings;

	customColor = settings.value("graphics/arrow_color", QColor(Qt::black)).value<QColor>();
	arrowSize = settings.value("graphics/arrow_size", DEFAULT_ARROWSIZE).toInt();
	lineWidth = settings.value("graphics/arrow_linewidth", DEFAULT_LINEWIDTH).toInt();

	legend.setArrowsSetting(this);
}

ArrowsSettingContainer::ArrowsSettingContainer(const ArrowsSettingContainer& c) :
	ArrowsSettingContainer {}
{
	copyValue(c);
}

ArrowsSettingContainer& ArrowsSettingContainer::operator=(const ArrowsSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& ArrowsSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ArrowsSettingContainer&> (c));
}

void ArrowsSettingContainer::copyValue(const XmlAttributeContainer& c)
{
	CompositeContainer::copyValue(c);

	emit updated();
}

double ArrowsSettingContainer::scaleFactor(double stdDistance) const
{
	double a = 1.0 / stdDistance;
	return legendLength / (a * standardValue);
}

vtkPolyData* ArrowsSettingContainer::buildFilteredData(vtkPointSet* data) const
{
	auto ret = vtkPolyData::New();

	auto src = data->GetPointData();
	auto tgt = ret->GetPointData();

	tgt->CopyAllocate(src);

	auto inPoints = data->GetPoints();
	auto outPoints = vtkSmartPointer<vtkPoints>::New();
	outPoints->SetDataTypeToDouble();

	double minSqr = minimumValue * minimumValue;
	vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();
	vtkDataArray* vecArray = src->GetArray(iRIC::toStr(target).c_str());

	if (inPoints == nullptr) {return ret;}
	if (vecArray == nullptr) {return ret;}

	vtkIdType id = 0;
	for (vtkIdType i = 0; i < inPoints->GetNumberOfPoints(); ++i) {
		double val = 0;
		for (int j = 0; j < vecArray->GetNumberOfComponents(); ++j) {
			double tmpv = vecArray->GetComponent(i, j);
			val += tmpv * tmpv;
		}
		if (val < minSqr) {continue;}

		outPoints->InsertNextPoint(inPoints->GetPoint(i));
		tgt->CopyData(src, i, id);
		ca->InsertNextCell(1, &id);
		++ id;
	}
	ret->SetPoints(outPoints);
	ret->SetVerts(ca);

	return ret;
}

vtkPolyData* ArrowsSettingContainer::buildArrowsPolygonData(vtkPointSet* data, VTKGraphicsView* view, double zscale) const
{
	auto atts = data->GetPointData();

	atts->SetActiveVectors(iRIC::toStr(target).c_str());
	vtkDataArray* vecArray = atts->GetArray(iRIC::toStr(target).c_str());
	if (vecArray == nullptr) {
		return vtkPolyData::New();
	}

	auto transformFilter = vtkSmartPointer<vtkTransformFilter>::New();
	auto t = vtkSmartPointer<vtkTransform>::New();
	t->Scale(1, 1, zscale);
	transformFilter->SetTransform(t);
	transformFilter->SetInputData(data);

	double factor = scaleFactor(view->stdDistance(1.0) * view->devicePixelRatioF());

	auto hedgeHog = vtkSmartPointer<vtkHedgeHog>::New();
	hedgeHog->SetVectorModeToUseVector();
	hedgeHog->SetScaleFactor(factor);
	hedgeHog->SetInputConnection(transformFilter->GetOutputPort());

	auto warp = vtkSmartPointer<vtkWarpVector>::New();
	warp->SetScaleFactor(factor);
	warp->SetInputConnection(transformFilter->GetOutputPort());

	auto glyph = vtkSmartPointer<vtkGlyph3D>::New();
	glyph->SetInputConnection(warp->GetOutputPort());
	glyph->SetScaleModeToDataScalingOff();
	glyph->SetVectorModeToUseVector();

	auto cone = buildConeSource(view);
	glyph->SetSourceConnection(cone->GetOutputPort());
	cone->Delete();

	auto append = vtkSmartPointer<vtkAppendPolyData>::New();
	append->AddInputConnection(hedgeHog->GetOutputPort());
	append->AddInputConnection(glyph->GetOutputPort());
	append->Update();

	auto ret = append->GetOutput();
	ret->Register(nullptr);
	return ret;
}

void ArrowsSettingContainer::updateStandardValueIfNeeded(vtkDataSetAttributes* atts)
{
	if (lengthMode == ArrowsSettingContainer::LengthMode::Custom) {return;}
	if (target == "") {return;}
	if (atts == nullptr) {return;}

	vtkDataArray* da = atts->GetArray(iRIC::toStr(target).c_str());
	if (da == nullptr) {return;}

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
	if (count > lens.size()) {count = static_cast<int>(lens.size());}

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

vtkConeSource* ArrowsSettingContainer::buildConeSource(VTKGraphicsView* view) const
{
	auto source = vtkConeSource::New();
	double height = view->stdDistance(arrowSize.value() * view->devicePixelRatioF());
	source->SetHeight(height);
	source->SetAngle(15);

	return source;
}
