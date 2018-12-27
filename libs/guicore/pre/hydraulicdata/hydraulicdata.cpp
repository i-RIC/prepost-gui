#include "hydraulicdata.h"
#include "private/hydraulicdata_impl.h"
#include "../base/preprocessorhydraulicdatadataiteminterface.h"

#include <QUndoCommand>

HydraulicData::Impl::Impl() :
	CompositeContainer ({&m_name, &m_caption}),
	m_name {"name"},
	m_caption {"caption"}
{}

HydraulicData::HydraulicData(ProjectDataItem* parent) :
	ProjectDataItem {parent},
	impl {new Impl {}}
{}

HydraulicData::~HydraulicData()
{
	delete impl;
}

QString HydraulicData::name() const
{
	return impl->m_name;
}

void HydraulicData::setName(const QString& name)
{
	impl->m_name = name;
	setFilename(QString("%1.dat").arg(name));
}

QString HydraulicData::caption() const
{
	return impl->m_caption;
}

void HydraulicData::setCaption(const QString& caption)
{
	impl->m_caption = caption;
}

PreProcessorHydraulicDataDataItemInterface* HydraulicData::hydraulicDataDataItem() const
{
	return dynamic_cast<PreProcessorHydraulicDataDataItemInterface*>(parent());
}

void HydraulicData::pushCommand(QUndoCommand* com)
{
	hydraulicDataDataItem()->pushCommand(com);
}

void HydraulicData::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->load(node);
	setFilename(QString("%1.dat").arg(impl->m_name));
}

void HydraulicData::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->save(writer);
}
