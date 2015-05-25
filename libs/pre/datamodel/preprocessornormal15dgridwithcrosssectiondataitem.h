#ifndef PREPROCESSORNORMAL15DGRIDWITHCROSSSECTIONDATAITEM_H
#define PREPROCESSORNORMAL15DGRIDWITHCROSSSECTIONDATAITEM_H

#include "preprocessorgriddataitem.h"

class QMenu;

class PreProcessorNormal15DGridWithCrossSectionDataItem : public PreProcessorGridDataItem
{
	Q_OBJECT
public:
	PreProcessorNormal15DGridWithCrossSectionDataItem(PreProcessorDataItem* parent);
	~PreProcessorNormal15DGridWithCrossSectionDataItem();
protected:
	void setupMenu() override;
private:
	QMenu* m_editMenu;
	QMenu* m_displayMenu;
};

#endif // PREPROCESSORNORMAL15DGRIDWITHCROSSSECTIONDATAITEM_H
