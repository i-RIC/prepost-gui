#include "hydraulicdatariversurveywaterelevation.h"
#include "private/hydraulicdatariversurveywaterelevation_editcaptioncommand.h"
#include "private/hydraulicdatariversurveywaterelevation_editcolorcommand.h"
#include "private/hydraulicdatariversurveywaterelevation_edititemcommand.h"
#include "private/hydraulicdatariversurveywaterelevation_impl.h"

#include <guicore/pre/base/preprocessorhydraulicdatagroupdataiteminterface.h>
#include <guicore/project/colorsource.h>

#include <QDataStream>
#include <QFile>

HydraulicDataRiverSurveyWaterElevation::HydraulicDataRiverSurveyWaterElevation(ProjectDataItem* parent) :
	HydraulicData {parent},
	impl {new Impl {}}
{
	auto groupItem = dynamic_cast<PreProcessorHydraulicDataGroupDataItemInterface*> (parent->parent());
	int colorIndex = groupItem->childItems().size();

	ColorSource cs(this);
	cs.load(":/libs/hydraulicdata/riversurveywaterelevation/data/colorsource_wse.xml");
	setColor(cs.getColor(colorIndex));
}

HydraulicDataRiverSurveyWaterElevation::~HydraulicDataRiverSurveyWaterElevation()
{
	delete impl;
}

bool HydraulicDataRiverSurveyWaterElevation::dataExists() const
{
	return impl->m_items.size() > 0;
}

HydraulicDataRiverSurveyWaterElevationItem* HydraulicDataRiverSurveyWaterElevation::getItem(const QString& name)
{
	auto it = impl->m_items.find(name);
	if (it == impl->m_items.end()) {return nullptr;}

	return &(it->second);
}

void HydraulicDataRiverSurveyWaterElevation::addItem(const QString& name, bool specified, double value)
{
	HydraulicDataRiverSurveyWaterElevationItem* item = getItem(name);
	if (item != nullptr) {
		item->setSpecified(specified);
		item->setValue(value);
		return;
	}
	HydraulicDataRiverSurveyWaterElevationItem newItem(name, specified, value);
	impl->m_items.insert({name, newItem});
}

void HydraulicDataRiverSurveyWaterElevation::clear()
{
	impl->m_items.clear();
}

QColor HydraulicDataRiverSurveyWaterElevation::color() const
{
	return impl->m_color;
}

void HydraulicDataRiverSurveyWaterElevation::setColor(const QColor& color)
{
	impl->m_color = color;
}

void HydraulicDataRiverSurveyWaterElevation::pushEditCaptionCommand(const QString& caption)
{
	pushCommand(new EditCaptionCommand(caption, this));
}

void HydraulicDataRiverSurveyWaterElevation::pushEditColorCommand(const QColor& color)
{
	pushCommand(new EditColorCommand(color, this));
}

void HydraulicDataRiverSurveyWaterElevation::pushEditItemCommand(const QString& name, bool specified, double value)
{
	pushCommand(new EditItemCommand(name, specified, value, this));
}

void HydraulicDataRiverSurveyWaterElevation::loadExternalData(const QString& filename)
{
	QFile f(filename);
	f.open(QIODevice::ReadOnly);
	QDataStream s(&f);
	int numData;
	s >> impl->m_color;
	s >> numData;
	clear();
	for (int i = 0; i < numData; ++i){
		QString name;
		bool specified;
		double val;
		s >> name >> specified >> val;
		addItem(name, specified, val);
	}
	f.close();
}

void HydraulicDataRiverSurveyWaterElevation::saveExternalData(const QString& filename)
{
	QFile f(filename);
	f.open(QIODevice::WriteOnly);
	QDataStream s(&f);
	s << impl->m_color;
	s << static_cast<int>(impl->m_items.size());
	for (const auto& pair : impl->m_items) {
		const auto& item = pair.second;
		s << item.name() << item.isSpecified() << item.value();
	}
	f.close();
}
