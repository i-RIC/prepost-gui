#include "../../datamodel/graphicswindowdataitem.h"
#include "../base/preprocessorrawdatadataiteminterface.h"
#include "rawdata.h"
#include "rawdatacreator.h"
//#include "pre/datamodel/preprocessorrawdatadataitem.h"
//#include "pre/datamodel/preprocessorrawdatagroupdataitem.h"
//#include "rawdatafactory.h"

#include <QDomElement>
#include <QDomNode>
#include <QSet>

RawDataCreator::RawDataCreator()
	: QObject(nullptr)
{
	m_emptyData = nullptr;
}

RawDataCreator::~RawDataCreator()
{
	delete m_emptyData;
}

RawData* RawDataCreator::restore(const QDomNode& node, ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	QDomElement elem = node.toElement();
	if (elem.attribute("type") == m_typeName) {
		return create(parent, condition);
	}
	return 0;
}

void RawDataCreator::setNameAndDefaultCaption(const QList<GraphicsWindowDataItem*>& list, RawData* data)
{
	QSet<QString> nameSet;
	QSet<QString> captionSet;

	// first, setup nameSet and captionSet.
	for (auto it = list.begin(); it != list.end(); ++it) {
		RawData* rawdata = dynamic_cast<PreProcessorRawdataDataItemInterface*>(*it)->rawData();
		if (rawdata->name() != "") {nameSet.insert(rawdata->name());}
		if (rawdata->caption() != "") {captionSet.insert(rawdata->caption());}
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
