#include "geodatabackground_editvaluecommand.h"
#include "../../base/preprocessorgeodatadataiteminterface.h"

GeoDataBackground::EditValueCommand::EditValueCommand(GeoDataBackground::Type type, const QVariant& val, GeoDataBackground* b) :
	QUndoCommand(GeoDataBackground::tr("Background edit value")),
	m_newType {type},
	m_newValue {val},
	m_oldType {b->m_type},
	m_oldValue {b->variantValue()},
	m_oldMapped {b->isMapped()},
	m_background {b}
{}

void GeoDataBackground::EditValueCommand::redo()
{
	applyValues(m_newType, m_newValue, false);
}

void GeoDataBackground::EditValueCommand::undo()
{
	applyValues(m_oldType, m_oldValue, m_oldMapped);
}

void GeoDataBackground::EditValueCommand::applyValues(GeoDataBackground::Type t, const QVariant& val, bool mapped)
{
	m_background->m_type = t;
	m_background->m_customValue = val;
	m_background->setMapped(mapped);

	auto geoData = dynamic_cast<PreProcessorGeoDataDataItemInterface*> (m_background->parent());
	geoData->informValueRangeChange();
	geoData->informDataChange();
}
