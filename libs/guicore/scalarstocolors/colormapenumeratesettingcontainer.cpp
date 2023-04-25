#include "colormapenumeratesettingcontainer.h"
#include "discretevaluesutil.h"
#include "../project/colorsource.h"
#include "../solverdef/enumloader.h"

#include <guibase/iricactivecellfilter.h>

#include <QLinearGradient>
#include <QPainter>

#include <algorithm>
#include <unordered_map>

namespace {

vtkDataArray* getActiveScalarOrFirst(vtkDataSetAttributes* atts)
{
	if (atts->GetNumberOfArrays() == 0) {return nullptr;}

	auto active = atts->GetScalars();
	if (active != nullptr) {return active;}

	return atts->GetArray(0);
}

} // namespace

ColorMapEnumerateSettingContainer::ColorMapEnumerateSettingContainer() :
	CompositeContainer({&legend}),
	legend {}
{
	legend.setColorMapSetting(this);
	QObject::connect(&legend.imageSetting, &ImageSettingContainer::updated, this, &ColorMapSettingContainerI::updated);
}

ColorMapEnumerateSettingContainer::ColorMapEnumerateSettingContainer(const ColorMapEnumerateSettingContainer& c) :
	ColorMapEnumerateSettingContainer {}
{
	copyValue(c);
}

ColorMapEnumerateSettingContainer& ColorMapEnumerateSettingContainer::operator=(const ColorMapEnumerateSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& ColorMapEnumerateSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ColorMapEnumerateSettingContainer&>(c));
}

void ColorMapEnumerateSettingContainer::load(const QDomNode& node)
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

void ColorMapEnumerateSettingContainer::save(QXmlStreamWriter& writer) const
{
	CompositeContainer::save(writer);

	for (const auto& c : colors) {
		writer.writeStartElement("Item");
		c.save(writer);
		writer.writeEndElement();
	}
}

void ColorMapEnumerateSettingContainer::copy(const ColorMapSettingContainerI& c)
{
	copyValue(dynamic_cast<const ColorMapEnumerateSettingContainer&> (c));
}

ColorMapSettingContainerI* ColorMapEnumerateSettingContainer::copy()
{
	auto s = new ColorMapEnumerateSettingContainer();
	s->copy(*this);

	return s;
}

void ColorMapEnumerateSettingContainer::copyValue(const XmlAttributeContainer& c)
{
	const auto& c2 = dynamic_cast<const ColorMapEnumerateSettingContainer&>(c);
	CompositeContainer::copyValue(c2);

	colors = c2.colors;
	valueCaption = c2.valueCaption;
	valueCaptions = c2.valueCaptions;

	emit ColorMapSettingContainerI::updated();
}

bool ColorMapEnumerateSettingContainer::importData(const QString& fileName)
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
	if (elem.nodeName() != "EnumerateColorMapSetting") {
		return false;
	}
	load(elem);
	return true;
}

bool ColorMapEnumerateSettingContainer::exportData(const QString& fileName)
{
	QFile f(fileName);
	bool ok = f.open(QFile::WriteOnly);
	if (! ok) {return false;}

	QXmlStreamWriter writer(&f);
	writer.writeStartDocument();
	writer.writeStartElement("EnumerateColorMapSetting");
	save(writer);
	writer.writeEndElement();
	writer.writeEndDocument();
	f.close();

	return true;
}

void ColorMapEnumerateSettingContainer::setup(const EnumLoader* enumLoader)
{
	ColorSource cs(nullptr);
	cs.load(":/libs/guicore/data/colorsource_cell.xml");

	auto enums = enumLoader->variantEnumerations();
	int index = 0;
	for (auto it = enums.begin(); it != enums.end(); ++it) {
		double value = it.key().toDouble();
		auto color = cs.getColor(index);
		colors.push_back(ColorMapSettingValueColorPairContainer(value, color));

		valueCaptions.insert({value, it.value()});

		++ index;
	}
}

vtkMapper* ColorMapEnumerateSettingContainer::buildCellDataMapper(vtkDataSet* data, bool ignoreTransparent)
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
		auto scalar = DiscreteValuesUtil::buildValuesEnumerate(activeScalar, colors, ignoreTransparent);
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

	auto stc = DiscreteValuesUtil::buildScalarsToColors(colors, ignoreTransparent);
	mapper->SetLookupTable(stc);
	stc->Delete();

	return mapper;
}

vtkMapper* ColorMapEnumerateSettingContainer::buildPointDataMapper(vtkDataSet* data)
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
		auto scalar = DiscreteValuesUtil::buildValuesEnumerate(activeScalar, colors, true);
		polyData2->GetPointData()->AddArray(scalar);
		polyData2->GetPointData()->SetActiveScalars(scalar->GetName());
		scalar->Delete();
	}

	mapper->SetInputData(polyData2);
	mapper->ScalarVisibilityOn();

	auto stc = DiscreteValuesUtil::buildScalarsToColors(colors, true);
	mapper->SetLookupTable(stc);
	stc->Delete();

	return mapper;
}

void ColorMapEnumerateSettingContainer::paintNodeData(double x1, double x2, double v1, double v2, double ymin, double ymax, QPainter* painter)
{
	painter->save();

	QColor c1, c2;
	for (const auto& pair : colors) {
		if (pair.value == v1) {
			c1 = pair.color;
		}
		if (pair.value == v2) {
			c2 = pair.color;
		}
	}
	if (! c1.isValid() || c2.isValid()) {return;}

	QLinearGradient gradient(QPointF(x1, 0), QPointF(x2, 0));
	gradient.setColorAt(0, c1);
	gradient.setColorAt(1, c2);

	QBrush brush(gradient);
	double xmin = std::min(x1, x2);
	double xmax = std::max(x1, x2);
	QRectF rect(QPointF(xmin, ymin), QPointF(xmax, ymax));
	painter->fillRect(rect, brush);

	painter->restore();
}

void ColorMapEnumerateSettingContainer::paintCellData(double x1, double x2, double v, double ymin, double ymax, QPainter* painter)
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, false);
	for (const auto& pair : colors) {
		if (pair.value == v && (! pair.transparent)) {
			QBrush brush(pair.color);

			double xmin = std::min(x1, x2);
			double xmax = std::max(x1, x2);
			QRectF rect(QPointF(xmin, ymin), QPointF(xmax, ymax));
			painter->fillRect(rect, brush);
		}
	}
	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->restore();
}

ColorMapLegendSettingContainerI* ColorMapEnumerateSettingContainer::legendSetting()
{
	return &legend;
}

ColorMapLegendSettingContainerI* ColorMapEnumerateSettingContainer::copyLegendSetting()
{
	auto l = new ColorMapEnumerateLegendSettingContainer();
	*l = legend;
	l->setColorMapSetting(this);

	return l;
}
