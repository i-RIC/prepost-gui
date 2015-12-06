#ifndef NAMEDGRAPHICWINDOWDATAITEM_H
#define NAMEDGRAPHICWINDOWDATAITEM_H

#include "../guicore_global.h"
#include "../datamodel/graphicswindowdataitem.h"

#include <string>

class GUICOREDLL_EXPORT NamedGraphicWindowDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT
public:
	NamedGraphicWindowDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);
	~NamedGraphicWindowDataItem();

	const std::string& name() const;
	void handleStandardItemChange() override;

signals:
	void changed(NamedGraphicWindowDataItem*);

private:
	std::string m_name;
};

#endif // NAMEDGRAPHICWINDOWDATAITEM_H
