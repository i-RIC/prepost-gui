#ifndef POST2DWINDOWGEODATATOPDATAITEM_H
#define POST2DWINDOWGEODATATOPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <QString>

#include <map>
#include <string>

class PreProcessorGeoDataTopDataItemI;
class Post2dWindowGeoDataGroupDataItem;
class SolverDefinitionGridAttribute;

class Post2dWindowGeoDataTopDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowGeoDataTopDataItem(PreProcessorGeoDataTopDataItemI* ditem, Post2dWindowDataItem* parent);
	PreProcessorGeoDataTopDataItemI* preGeoDataTopDataItem() const;

	Post2dWindowGeoDataGroupDataItem* groupDataItem(const std::string& name) const;

private slots:
	void updateChildren();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void buildReferenceInformationAttribute();

	std::map<std::string, Post2dWindowGeoDataGroupDataItem*> m_itemNameMap;
	PreProcessorGeoDataTopDataItemI* m_preGeoDataTopDataItem;

	SolverDefinitionGridAttribute* m_referenceInformationAttribute;
};

#endif // POST2DWINDOWGEODATATOPDATAITEM_H
