#include "colormapsettingcontainer.h"
#include "discretevaluesutil.h"

#include <guibase/iricactivecellfilter.h>

#include <QDomNode>
#include <QLinearGradient>
#include <QXmlStreamWriter>

#include <vtkAppendPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkColorTransferFunction.h>
#include <vtkGeometryFilter.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>

#include <misc/iricrootpath.h>

#include <unordered_map>

namespace {

const std::string VNAME = "VALUES";
const double VERYSMALL = 1.0E-10;

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

void fillRect(double x1, double x2, double ymin, double ymax, const QColor color, QPainter* painter)
{
	QBrush brush(color);

	double xmin = std::min(x1, x2);
	double xmax = std::max(x1, x2);
	QRectF rect(QPointF(xmin, ymin), QPointF(xmax, ymax));
	painter->fillRect(rect, brush);
}

QColor interpolateColor(double v, double v1, double v2, const QColor& c1, const QColor& c2)
{
	double r = (v - v1) / (v2 - v1);

	QColor ret;
	ret.setRed(int((1 - r) * c1.red() + r * c2.red()));
	ret.setGreen(int((1 - r) * c1.green() + r * c2.green()));
	ret.setBlue(int((1 - r) * c1.blue() + r * c2.blue()));

	return ret;
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
	QObject::connect(&legend.imageSetting, &ImageSettingContainer::updated, this, &ColorMapSettingContainerI::updated);

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

	importDefault();
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
	emit ColorMapSettingContainerI::updated();
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

void ColorMapSettingContainer::copy(const ColorMapSettingContainerI& c)
{
	copyValue(dynamic_cast<const ColorMapSettingContainer&> (c));
}

ColorMapSettingContainerI* ColorMapSettingContainer::copy()
{
	auto s = new ColorMapSettingContainer();
	s->copy(*this);

	return s;
}

void ColorMapSettingContainer::copyValue(const XmlAttributeContainer& c)
{
	const auto& c2 = dynamic_cast<const ColorMapSettingContainer&>(c);
	CompositeContainer::copyValue(c2);

	colors = c2.colors;
	autoMinValue = c2.autoMinValue;
	autoMaxValue = c2.autoMaxValue;
	valueCaption = c2.valueCaption;

	emit ColorMapSettingContainerI::updated();
}

bool ColorMapSettingContainer::importData(const QString& fileName)
{
	QFile f(fileName);
	QDomDocument doc;

	QString errorStr;
	int errorLine;
	int errorColumn;

	bool ok = doc.setContent(&f, &errorStr, &errorLine, &errorColumn);
	if (! ok) {
		return false;
	}
	auto elem = doc.documentElement();
	if (elem.nodeName() != "ColorMapSetting") {
		return false;
	}
	load(elem);
	return true;
}

bool ColorMapSettingContainer::exportData(const QString& fileName)
{
	QFile f(fileName);
	bool ok = f.open(QFile::WriteOnly);
	if (! ok) {return false;}

	QXmlStreamWriter writer(&f);
	writer.writeStartDocument();
	writer.writeStartElement("ColorMapSetting");
	save(writer);
	writer.writeEndElement();
	writer.writeEndDocument();
	f.close();

	return true;
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

void ColorMapSettingContainer::paintNodeData(double x1, double x2, double v1, double v2, double ymin, double ymax, QPainter* painter)
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, false);
	auto colors = getColors();

	if (v2 < v1) {
		std::swap(v1, v2);
		std::swap(x1, x2);
	}

	if (transitionMode == TransitionMode::Continuous) {
		if (v1 <= colors.begin()->value && fillLower) {
			if (v2 <= colors.begin()->value) {
				fillRect(x1, x2, ymin, ymax, colors.begin()->color, painter);
			} else {
				double r = (colors.begin()->value - v1) / (v2 - v1);
				double x = (1 - r) * x1 + r * x2;
				fillRect(x1, x, ymin, ymax, colors.begin()->color, painter);
			}
		}
		if (v2 >= colors.rbegin()->value && fillUpper) {
			if (v1 >= colors.rbegin()->value) {
				fillRect(x1, x2, ymin, ymax, colors.rbegin()->color, painter);
			} else {
				double r = (colors.rbegin()->value - v1) / (v2 - v1);
				double x = (1 - r) * x1 + r * x2;
				fillRect(x, x2, ymin, ymax, colors.rbegin()->color, painter);
			}
		}

		for (int i = 0; i < colors.size() - 1; ++i) {
			const auto& c1 = colors.at(i);
			const auto& c2 = colors.at(i + 1);

			auto color1 = c1.color;
			auto color2 = c2.color;

			if (std::abs(v2 - v1) < VERYSMALL) {
				auto r = (v1 - c1.value) / (c2.value - c1.value);
				if (r < 0) {continue;}
				if (r > 1) {continue;}

				auto color = interpolateColor(r, 0, 1, c1.color, c2.color);
				fillRect(x1, x2, ymin, ymax, color, painter);
			} else {
				auto r1_orig = (c1.value - v1) / (v2 - v1);
				auto r2_orig = (c2.value - v1) / (v2 - v1);
				if (r2_orig <= 0) {continue;}
				if (r1_orig >= 1) {continue;}

				auto r1 = r1_orig;
				auto r2 = r2_orig;

				if (r1 < 0) {
					color1 = interpolateColor(0, r1_orig, r2_orig, c1.color, c2.color);
					r1 = 0;
				}
				if (r2 > 1) {
					color2 = interpolateColor(1, r1_orig, r2_orig, c1.color, c2.color);
					r2 = 1;
				}

				double xx1 = (1 - r1) * x1 + r1 * x2;
				double xx2 = (1 - r2) * x1 + r2 * x2;
				QLinearGradient gradient(QPointF(xx1, 0), QPointF(xx2, 0));
				gradient.setColorAt(0, color1);
				gradient.setColorAt(1, color2);

				QBrush brush(gradient);
				double xmin = std::min(xx1, xx2);
				double xmax = std::max(xx1, xx2);
				QRectF rect(QPointF(xmin, ymin), QPointF(xmax, ymax));
				painter->fillRect(rect, brush);
			}
		}
	} else if (transitionMode == TransitionMode::Discrete) {
		double min = getColorTableMinValue();

		if (v1 <= min && fillLower) {
			if (v2 <= min) {
				fillRect(x1, x2, ymin, ymax, colors.begin()->color, painter);
			} else {
				double r = (min - v1) / (v2 - v1);
				double x = (1 - r) * x1 + r * x2;
				fillRect(x1, x, ymin, ymax, colors.begin()->color, painter);
			}
		}
		if (v2 >= colors.rbegin()->value && fillUpper) {
			if (v1 >= colors.rbegin()->value) {
				fillRect(x1, x2, ymin, ymax, colors.rbegin()->color, painter);
			} else {
				double r = (colors.rbegin()->value - v1) / (v2 - v1);
				double x = (1 - r) * x1 + r * x2;
				fillRect(x, x2, ymin, ymax, colors.rbegin()->color, painter);
			}
		}

		for (int i = 0; i < colors.size(); ++i) {
			if (colors.at(i).transparent) {continue;}

			double min2, max2;
			if (i == 0) {
				min2 = min;
				max2 = colors.at(i).value;
			} else {
				min2 = colors.at(i - 1).value;
				max2 = colors.at(i).value;
			}
			auto color = colors.at(i).color;

			if (std::abs(v2 - v1) < VERYSMALL) {
				auto r = (v1 - min2) / (max2 - min2);
				if (r < 0) {continue;}
				if (r > 1) {continue;}

				fillRect(x1, x2, ymin, ymax, color, painter);
			} else {
				auto r1 = (min2 - v1) / (v2 - v1);
				auto r2 = (max2 - v1) / (v2 - v1);
				if (r2 <= 0) {continue;}
				if (r1 >= 1) {continue;}

				if (r1 < 0) {r1 = 0;}
				if (r2 > 1) {r2 = 1;}

				double xx1 = (1 - r1) * x1 + r1 * x2;
				double xx2 = (1 - r2) * x1 + r2 * x2;
				fillRect(xx1, xx2, ymin, ymax, color, painter);
			}
		}
	}
	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->restore();
}

void ColorMapSettingContainer::paintCellData(double x1, double x2, double v, double ymin, double ymax, QPainter* painter)
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, false);
	auto colors = getColors();
	QColor color;
	if (transitionMode == TransitionMode::Continuous) {
		if (v < colors.begin()->value) {
			if (! fillLower) {return;}
			color = colors.begin()->color;
		} else if (v > colors.rbegin()->value) {
			if (! fillUpper) {return;}
			color = colors.rbegin()->color;
		} else {
			for (int i = 0; i < colors.size() - 1; ++i) {
				const auto& c1 = colors.at(i);
				const auto& c2 = colors.at(i + 1);
				if (v >= c1.value && v <= c2.value) {
					color = interpolateColor(v, c1.value, c2.value, c1.color, c2.color);
				}
			}
		}
		fillRect(x1, x2, ymin, ymax, color, painter);
	} else if (transitionMode == TransitionMode::Discrete) {
		double min = getColorTableMinValue();
		if (v < min) {
			if (! fillLower) {return;}
			if (colors.begin()->transparent) {return;}
			color = colors.begin()->color;
		} else if (v > colors.rbegin()->value) {
			if (! fillUpper) {return;}
			if (colors.rbegin()->transparent) {return;}
			color = colors.rbegin()->color;
		} else {
			for (int i = 0; i < colors.size(); ++i) {
				double min2, max2;
				if (i == 0) {
					min2 = min;
					max2 = colors.at(i).value;
				} else {
					min2 = colors.at(i - 1).value;
					max2 = colors.at(i).value;
				}
				if (v >= min2 && v < max2) {
					color = colors.at(i).color;
				}
			}
		}
		fillRect(x1, x2, ymin, ymax, color, painter);
	}
	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->restore();
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
		auto scalar = DiscreteValuesUtil::buildValues(activeScalar, fillLower, fillUpper, getColorTableMinValue(), actualColors, ignoreTransparent);
		polyData2->GetCellData()->AddArray(scalar);
		polyData2->GetCellData()->SetActiveScalars(scalar->GetName());
		scalar->Delete();
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

void ColorMapSettingContainer::importDefault()
{
	QDir dir(iRICRootPath::get());
	dir.cdUp();
	dir.cdUp();
	QStringList paths;
	paths.append(dir.absoluteFilePath("private/colormaps/default.cmsetting"));
	paths.append(dir.absoluteFilePath("colormaps/default.cmsetting"));
	for (auto path : paths) {
		bool ok = importData(path);

		if (ok) {return;}
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
		pair.transparent = origPair.transparent;
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
