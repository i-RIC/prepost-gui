#ifndef RAWDATARIVERSURVEYCROSSSECTIONWINDOWPROJECTDATAITEM_H
#define RAWDATARIVERSURVEYCROSSSECTIONWINDOWPROJECTDATAITEM_H

#include "rd_riversurvey_global.h"
#include <guicore/project/projectdataitem.h>

class RawDataRiverSurvey;
class RawDataRiverSurveyCrosssectionWindow;
class PreProcessorRawDataGroupDataItemInterface;

class RD_RIVERSURVEY_EXPORT RawDataRiverSurveyCrosssectionWindowProjectDataItem : public ProjectDataItem
{

public:
	RawDataRiverSurveyCrosssectionWindowProjectDataItem(PreProcessorRawDataGroupDataItemInterface* gitem, QWidget* parentWindow);
	~RawDataRiverSurveyCrosssectionWindowProjectDataItem();
	void informWindowClose();
	void requestWindowClose();
	RawDataRiverSurveyCrosssectionWindow* window() {
		return m_window;
	}

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

protected:
	RawDataRiverSurveyCrosssectionWindow* m_window;

public:
	friend class PostProcessorWindow;
};

#endif // RAWDATARIVERSURVEYCROSSSECTIONWINDOWPROJECTDATAITEM_H
