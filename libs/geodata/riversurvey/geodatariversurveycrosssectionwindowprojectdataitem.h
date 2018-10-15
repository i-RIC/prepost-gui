#ifndef GEODATARIVERSURVEYCROSSSECTIONWINDOWPROJECTDATAITEM_H
#define GEODATARIVERSURVEYCROSSSECTIONWINDOWPROJECTDATAITEM_H

#include "gd_riversurvey_global.h"

#include <guicore/project/projectdataitem.h>

class GeoDataRiverSurveyCrosssectionWindow;
class PreProcessorGeoDataGroupDataItemInterface;

class GD_RIVERSURVEY_EXPORT GeoDataRiverSurveyCrosssectionWindowProjectDataItem : public ProjectDataItem
{

public:
	GeoDataRiverSurveyCrosssectionWindowProjectDataItem(PreProcessorGeoDataGroupDataItemInterface* gitem, QWidget* parentWindow);
	~GeoDataRiverSurveyCrosssectionWindowProjectDataItem();
	void informWindowClose();
	void requestWindowClose();
	GeoDataRiverSurveyCrosssectionWindow* window() {
		return m_window;
	}

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

protected:
	GeoDataRiverSurveyCrosssectionWindow* m_window;

public:
	friend class PostProcessorWindow;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONWINDOWPROJECTDATAITEM_H
