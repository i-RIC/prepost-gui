#include "hydraulicdatariversurveywaterelevation_edititemcommand.h"
#include "hydraulicdatariversurveywaterelevation_impl.h"

#include <guicore/base/iricmainwindowinterface.h>

HydraulicDataRiverSurveyWaterElevation::EditItemCommand::EditItemCommand(const QString& name, bool specified, double value, HydraulicDataRiverSurveyWaterElevation* we) :
	m_newSpecified {specified},
	m_newValue {value},
	m_isNew {false},
	m_name {name},
	m_we {we}
{
	auto it = we->impl->m_items.find(name);
	if (it == we->impl->m_items.end()) {
		m_isNew = true;
	} else {
		auto& item = it->second;
		m_oldSpecified = item.isSpecified();
		m_oldValue = item.value();
	}
}

void HydraulicDataRiverSurveyWaterElevation::EditItemCommand::redo()
{
	m_we->addItem(m_name, m_newSpecified, m_newValue);
	m_we->iricMainWindow()->updateCrosssectionWindows();
}

void HydraulicDataRiverSurveyWaterElevation::EditItemCommand::undo()
{
	if (m_isNew) {
		m_we->impl->m_items.erase(m_name);
	} else {
		m_we->addItem(m_name, m_oldSpecified, m_oldValue);
	}
	m_we->iricMainWindow()->updateCrosssectionWindows();
}
