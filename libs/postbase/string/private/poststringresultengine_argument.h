#ifndef POSTSTRINGRESULTENGINE_ARGUMENT_H
#define POSTSTRINGRESULTENGINE_ARGUMENT_H

#include "../poststringresultargumentcontainer.h"
#include "../poststringresultengine.h"

class PostStringResultEngine::Argument
{
public:
	Argument(const PostStringResultArgumentContainer& c);

	QString valueForTest() const;
	QJSValue value(v4PostZoneDataContainer* c) const;

private:
	std::string name() const;

	QJSValue baseIterativeValue(v4PostZoneDataContainer* c) const;

	QJSValue nodeValue(v4PostZoneDataContainer* c) const;
	int nodeIndex(v4PostZoneDataContainer* c) const;

	QJSValue cellValue(v4PostZoneDataContainer* c) const;
	int cellIndex(v4PostZoneDataContainer* c) const;

	QJSValue iEdgeValue(v4PostZoneDataContainer* c) const;
	int iEdgeIndex(v4PostZoneDataContainer* c) const;

	QJSValue jEdgeValue(v4PostZoneDataContainer* c) const;
	int jEdgeIndex(v4PostZoneDataContainer* c) const;

	QJSValue kEdgeValue(v4PostZoneDataContainer* c) const;
	int kEdgeIndex(v4PostZoneDataContainer* c) const;

	PostStringResultArgumentContainer m_setting;
};

#endif // POSTSTRINGRESULTENGINE_ARGUMENT_H
