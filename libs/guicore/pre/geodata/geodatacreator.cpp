#include "../../datamodel/graphicswindowdataitem.h"
#include "../base/preprocessorgeodatadataiteminterface.h"
#include "geodata.h"
#include "geodatacreator.h"
//#include "pre/datamodel/preprocessorgeodatadataitem.h"
//#include "pre/datamodel/preprocessorgeodatagroupdataitem.h"
//#include "geodatafactory.h"

#include <QDomElement>
#include <QDomNode>
#include <QSet>

GeoDataCreator::GeoDataCreator()
	: QObject(nullptr)
{
	m_emptyData = nullptr;
}

GeoDataCreator::~GeoDataCreator()
{
	delete m_emptyData;
}

GeoData* GeoDataCreator::restore(const QDomNode& node, ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	QDomElement elem = node.toElement();
	if (elem.attribute("type") == m_typeName) {
		return create(parent, condition);
	}
	return 0;
}

void GeoDataCreator::setNameAndDefaultCaption(const QList<GraphicsWindowDataItem*>& list, GeoData* data)
{
	QSet<QString> nameSet;
	QSet<QString> captionSet;

	// first, setup nameSet and captionSet.
	for (auto it = list.begin(); it != list.end(); ++it) {
		GeoData* geodata = dynamic_cast<PreProcessorRawdataDataItemInterface*>(*it)->geoData();
		if (geodata->name() != "") {nameSet.insert(geodata->name());}
		if (geodata->caption() != "") {captionSet.insert(geodata->caption());}
	}
	unsigned int idx = 1;
	bool ok = false;
	while (! ok) {
		QString tmpname = data->creator()->name(idx);
		QString tmpcap = data->creator()->defaultCaption(idx);
		if (! nameSet.contains(tmpname) && ! captionSet.contains(tmpcap)) {
			data->setName(tmpname);
			data->setCaption(tmpcap);
			ok = true;
		}
		++idx;
	}
}
