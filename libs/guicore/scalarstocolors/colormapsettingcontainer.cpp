#include "colormapsettingcontainer.h"
#include "discretevaluesutil.h"

#include <guibase/iricactivecellfilter.h>

#include <QDomNode>
#include <QXmlStreamWriter>

#include <vtkAppendPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkColorTransferFunction.h>
#include <vtkGeometryFilter.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>

#include <unordered_map>

namespace {

const std::string VNAME = "VALUES";

vtkDataArray* getActiveScalarOrFirst(vtkDataSetAttributes* atts)
{
	if (atts->GetNumberOfArrays() == 0) {return nullptr;}

	auto active = atts->GetScalars();
	if (active != nullptr) {return active;}

	return atts->GetArray(0);
}

void addPolyDataWithValue(vtkAppendPolyData* append, vtkPolyData* polydata, int value)
{
	auto newdata = vtkPolyData::New();
	newdata->CopyStructure(polydata);

	auto vals = vtkIntArray::New();
	vals->SetName(VNAME.c_str());
	for (vtkIdType j = 0; j < newdata->GetNumberOfCells(); ++j) {
		vals->InsertNextValue(value);
	}
	newdata->GetCellData()->AddArray(vals);
	newdata->GetCellData()->SetActiveScalars(vals->GetName());
	append->AddInputData(newdata);
	newdata->Delete();
}

} // namespace

ColorMapSettingContainer::ColorMapSettingContainer() :
	CompositeContainer({&transitionMode, &valueMode, &autoValueRange,
										 &minValue, &maxValue, &fillUpper, &fillLower, &colorTableMinValue, &legend}),
	transitionMode {"transitionMode"},
	valueMode {"valueMode"},
	autoValueRange {"autoValueRange"},
	minValue {"minValue"},
	maxValue {"maxValue"},
	fillUpper {"fillUppeer"},
	fillLower {"fillLower"},
	colorTableMinValue {"colorTableMinValue"},
	legend {}
{
	legend.setColorMapSetting(this);

	transitionMode = TransitionMode::Continuous;
	valueMode = ValueMode::Relative;
	autoValueRange = true;
	minValue = 0;
	maxValue = 1;
	fillUpper = true;
	fillLower = true;

	colors.push_back(ColorMapSettingValueColorPairContainer(0, QColor(0, 0, 255)));
	colors.push_back(ColorMapSettingValueColorPairContainer(0.25, QColor(0, 255, 255)));
	colors.push_back(ColorMapSettingValueColorPairContainer(0.5, QColor(0, 255, 0)));
	colors.push_back(ColorMapSettingValueColorPairContainer(0.75, QColor(255, 255, 0)));
	colors.push_back(ColorMapSettingValueColorPairContainer(1, QColor(255, 0, 0)));
}

ColorMapSettingContainer::ColorMapSettingContainer(const ColorMapSettingContainer& c) :
	ColorMapSettingContainer {}
{
	copyValue(c);
}

ColorMapSettingContainer& ColorMapSettingContainer::operator=(const ColorMapSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& ColorMapSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ColorMapSettingContainer&>(c));
}

void ColorMapSettingContainer::load(const QDomNode& node)
{
	CompositeContainer::load(node);

	colors.clear();
	const auto& children = node.childNodes();
	for (int i = 0; i < children.size(); ++i) {
		QDomNode itemNode = children.at(i);
		if (itemNode.nodeName() != "Item") {continue;}

		ColorMapSettingValueColorPairContainer pair;
		pair.load(itemNode);
		colors.push_back(pair);
	}
}

void ColorMapSettingContainer::save(QXmlStreamWriter& writer) const
{
	CompositeContainer::save(writer);

	for (const auto& c : colors) {
		writer.writeStartElement("Item");
		c.save(writer);
		writer.writeEndElement();
	}
}

void ColorMapSettingContainer::copyValue(const XmlAttributeContainer& c)
{
	const auto& c2 = dynamic_cast<const ColorMapSettingContainer&>(c);
	CompositeContainer::copyValue(c2);

	colors = c2.colors;
	autoMinValue = c2.autoMinValue;
	autoMaxValue = c2.autoMaxValue;
	valueCaption = c2.valueCaption;
}

vtkMapper* ColorMapSettingContainer::buildCellDataMapper(vtkDataSet* data, bool ignoreTransparent)
{
	if (transitionMode == TransitionMode::Continuous) {
		return buildCellDataMapperContinuous(data);
	} else if (transitionMode == TransitionMode::Discrete) {
		return buildCellDataMapperDiscrete(data, ignoreTransparent);
	}
	return nullptr;
}

vtkMapper* ColorMapSettingContainer::buildPointDataMapper(vtkDataSet* data)
{
	if (transitionMode == TransitionMode::Continuous) {
		return buildPointDataMapperContinuous(data);
	} else if (transitionMode == TransitionMode::Discrete) {
		return buildPointDataMapperDiscrete(data);
	}
	return nullptr;
}

ColorMapLegendSettingContainerI* ColorMapSettingContainer::legendSetting()
{
	return &legend;
}

ColorMapLegendSettingContainerI* ColorMapSettingContainer::copyLegendSetting()
{
	auto l = new ColorMapLegendSettingContainer();
	*l = legend;
	l->setColorMapSetting(this);

	return l;
}

vtkMapper* ColorMapSettingContainer::buildCellDataMapperContinuous(vtkDataSet* data)
{
	auto mapper = vtkPolyDataMapper::New();

	auto geomFilter = vtkSmartPointer<vtkGeometryFilter>::New();
	geomFilter->SetInputData(data);
	geomFilter->Update();
	auto polyData = geomFilter->GetOutput();

	// copy structure and active scalar only
	auto polyData2 = vtkSmartPointer<vtkPolyData>::New();
	polyData2->CopyStructure(polyData);

	auto activeScalar = getActiveScalarOrFirst(polyData->GetCellData());
	if (activeScalar != nullptr) {
		polyData2->GetCellData()->AddArray(activeScalar);
		polyData2->GetCellData()->SetActiveScalars(activeScalar->GetName());
	}

	if (! fillLower) {
		auto lowerClip = vtkSmartPointer<iricActiveCellFilter>::New();
		lowerClip->SetInputData(polyData2);
		lowerClip->CellClippingOn();
		lowerClip->SetValue(getMinValue());
		lowerClip->Update();
		polyData2 = lowerClip->GetOutput();
	}

	if (! fillUpper) {
		auto upperClip = vtkSmartPointer<iricActiveCellFilter>::New();
		upperClip->SetInputData(polyData2);
		upperClip->CellClippingOn();
		upperClip->SetValue(getMaxValue());
		upperClip->InsideOutOn();
		upperClip->Update();
		polyData2 = upperClip->GetOutput();
	}

	mapper->SetInputData(polyData2);
	mapper->ScalarVisibilityOn();

	auto stc = buildScalarsToColorsContinuous();
	mapper->SetLookupTable(stc);
	stc->Delete();

	return mapper;
}

vtkMapper* ColorMapSettingContainer::buildCellDataMapperDiscrete(vtkDataSet* data, bool ignoreTransparent)
{
	auto actualColors = getColors();

	auto mapper = vtkPolyDataMapper::New();

	auto geomFilter = vtkSmartPointer<vtkGeometryFilter>::New();
	geomFilter->SetInputData(data);
	geomFilter->Update();
	auto polyData = geomFilter->GetOutput();

	// copy structure and active scalar only
	auto polyData2 = vtkSmartPointer<vtkPolyData>::New();
	polyData2->CopyStructure(polyData);

	auto activeScalar = getActiveScalarOrFirst(polyData->GetCellData());
	if (activeScalar != nullptr) {
		auto scalar = DiscreteValuesUtil::buildValues(activeScalar, actualColors, ignoreTransparent);
		polyData2->GetCellData()->AddArray(scalar);
		polyData2->GetCellData()->SetActiveScalars(scalar->GetName());
		scalar->Delete();
	}

	if (! fillLower) {
		auto lowerClip = vtkSmartPointer<iricActiveCellFilter>::New();
		lowerClip->SetInputData(polyData2);
		lowerClip->CellClippingOn();
		lowerClip->SetValue(getMinValue());
		lowerClip->Update();
		polyData2 = lowerClip->GetOutput();
	}

	if (! fillUpper) {
		auto upperClip = vtkSmartPointer<iricActiveCellFilter>::New();
		upperClip->SetInputData(polyData2);
		upperClip->CellClippingOn();
		upperClip->SetValue(getMaxValue());
		upperClip->InsideOutOn();
		upperClip->Update();
		polyData2 = upperClip->GetOutput();
	}

	auto lowerClip = vtkSmartPointer<iricActiveCellFilter>::New();
	lowerClip->SetInputData(polyData2);
	lowerClip->CellClippingOn();
	lowerClip->SetValue(1); // value with 0 is transparent
	lowerClip->Update();
	polyData2 = lowerClip->GetOutput();

	mapper->SetInputData(polyData2);
	mapper->ScalarVisibilityOn();

	auto stc = DiscreteValuesUtil::buildScalarsToColors(actualColors, ignoreTransparent);
	mapper->SetLookupTable(stc);
	stc->Delete();

	return mapper;
}

vtkMapper* ColorMapSettingContainer::buildPointDataMapperContinuous(vtkDataSet* data)
{
	auto mapper = vtkPolyDataMapper::New();

	auto geomFilter = vtkSmartPointer<vtkGeometryFilter>::New();
	geomFilter->SetInputData(data);
	geomFilter->Update();
	auto polyData = geomFilter->GetOutput();

	// copy structure and active scalar only
	auto polyData2 = vtkSmartPointer<vtkPolyData>::New();
	polyData2->CopyStructure(polyData);

	auto activeScalar = getActiveScalarOrFirst(polyData->GetPointData());
	if (activeScalar != nullptr) {
		polyData2->GetPointData()->AddArray(activeScalar);
		polyData2->GetPointData()->SetActiveScalars(activeScalar->GetName());
	}

	if (! fillLower) {
		auto lowerClip = vtkSmartPointer<vtkClipPolyData>::New();
		lowerClip->SetInputData(polyData2);
		lowerClip->SetValue(getMinValue());
		lowerClip->Update();
		polyData2 = lowerClip->GetOutput();
	}

	if (! fillUpper) {
		auto upperClip = vtkSmartPointer<vtkClipPolyData>::New();
		upperClip->SetInputData(polyData2);
		upperClip->SetValue(getMaxValue());
		upperClip->InsideOutOn();
		upperClip->Update();
		polyData2 = upperClip->GetOutput();
	}

	mapper->SetInputData(polyData2);
	mapper->ScalarVisibilityOn();

	auto stc = buildScalarsToColorsContinuous();
	mapper->SetLookupTable(stc);
	stc->Delete();

	return mapper;
}

vtkMapper* ColorMapSettingContainer::buildPointDataMapperDiscrete(vtkDataSet* data)
{
	auto mapper = vtkPolyDataMapper::New();

	auto geomFilter = vtkSmartPointer<vtkGeometryFilter>::New();
	geomFilter->SetInputData(data);
	geomFilter->Update();
	auto polyData = geomFilter->GetOutput();

	// copy structure and active scalar only
	auto polyData2 = vtkSmartPointer<vtkPolyData>::New();
	polyData2->CopyStructure(polyData);

	auto activeScalar = getActiveScalarOrFirst(polyData->GetPointData());
	if (activeScalar != nullptr) {
		polyData2->GetPointData()->AddArray(activeScalar);
		polyData2->GetPointData()->SetActiveScalars(activeScalar->GetName());
	}

	if (! fillLower) {
		auto lowerClip = vtkSmartPointer<vtkClipPolyData>::New();
		lowerClip->SetInputData(polyData2);
		lowerClip->SetValue(getMinValue());
		lowerClip->Update();
		polyData2 = lowerClip->GetOutput();
	}

	if (! fillUpper) {
		auto upperClip = vtkSmartPointer<vtkClipPolyData>::New();
		upperClip->SetInputData(polyData2);
		upperClip->SetValue(getMaxValue());
		upperClip->InsideOutOn();
		upperClip->Update();
		polyData2 = upperClip->GetOutput();
	}

	auto append = vtkSmartPointer<vtkAppendPolyData>::New();

	auto actualColors = getColors();
	std::vector<int> values;

	int value = 1;
	for (const auto& pair : actualColors) {
		if (pair.transparent) {
			values.push_back(0);
		} else {
			values.push_back(value);
			value += 1;
		}
	}

	for (int i = 0; i < actualColors.size() - 1; ++i) {
		auto c = actualColors.at(i);

		auto clip = vtkSmartPointer<vtkClipPolyData>::New();
		clip->SetInputData(polyData2);
		clip->SetValue(c.value);
		clip->GenerateClippedOutputOn();
		clip->Update();

		int v = values.at(i);
		if (v != 0) {
			addPolyDataWithValue(append, clip->GetClippedOutput(), v);
		}
		polyData2 = clip->GetOutput();
	}
	int v = values.at(actualColors.size() - 1);
	if (v != 0) {
		addPolyDataWithValue(append, polyData2, values.at(actualColors.size() - 1));
	}

	auto clean = vtkSmartPointer<vtkCleanPolyData>::New();
	clean->SetInputConnection(append->GetOutputPort());
	mapper->SetInputConnection(clean->GetOutputPort());
	mapper->ScalarVisibilityOn();
	auto stc = DiscreteValuesUtil::buildScalarsToColors(actualColors, false);
	mapper->SetLookupTable(stc);
	stc->Delete();

	return mapper;
}

vtkScalarsToColors* ColorMapSettingContainer::buildScalarsToColorsContinuous()
{
	auto ctf = vtkColorTransferFunction::New();

	auto cols = getColors();
	for (const auto& pair : cols) {
		QColor color = pair.color;
		ctf->AddRGBPoint(pair.value, color.redF(), color.greenF(), color.blueF());
	}

	return ctf;
}

double ColorMapSettingContainer::getColorTableMinValue() const
{
	if (valueMode == ValueMode::Absolute) {
		return colorTableMinValue;
	} else {
		if (autoValueRange) {
			return autoMinValue;
		} else {
			return minValue;
		}
	}
}

std::vector<ColorMapSettingValueColorPairContainer> ColorMapSettingContainer::getColors() const
{
	if (valueMode == ValueMode::Absolute) {
		return colors;
	}
	std::vector<ColorMapSettingValueColorPairContainer> ret;

	double min = minValue;
	double max = maxValue;
	if (autoValueRange) {
		min = autoMinValue;
		max = autoMaxValue;
	}
	auto scale = (max - min);

	for (auto origPair : colors) {
		ColorMapSettingValueColorPairContainer pair;
		pair.value = min + scale * origPair.value;
		pair.color = origPair.color;
		ret.push_back(pair);
	}

	return ret;
}

double ColorMapSettingContainer::getMinValue() const
{
	if (valueMode == ValueMode::Absolute) {
		if (transitionMode == TransitionMode::Continuous) {
			return colors.at(0).value;
		} else {
			return colorTableMinValue;
		}
	} else {
		if (autoValueRange) {
			return autoMinValue;
		} else {
			return minValue;
		}
	}
}

double ColorMapSettingContainer::getMaxValue() const
{
	if (valueMode == ValueMode::Absolute) {
		return colors.at(colors.size() - 1).value;
	} else {
		if (autoValueRange) {
			return autoMaxValue;
		} else {
			return maxValue;
		}
	}
}
