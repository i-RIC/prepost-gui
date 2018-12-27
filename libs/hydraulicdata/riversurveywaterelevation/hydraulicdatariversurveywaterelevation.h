#ifndef HYDRAULICDATARIVERSURVEYWATERELEVATION_H
#define HYDRAULICDATARIVERSURVEYWATERELEVATION_H

#include "hd_riversurveywaterelevation_global.h"

#include <guicore/pre/hydraulicdata/hydraulicdata.h>

class HydraulicDataRiverSurveyWaterElevationItem;

class HD_RIVERSURVEYWATERELEVATION_EXPORT HydraulicDataRiverSurveyWaterElevation : public HydraulicData
{
	Q_OBJECT

public:
	HydraulicDataRiverSurveyWaterElevation(ProjectDataItem* parent);
	~HydraulicDataRiverSurveyWaterElevation();

	bool dataExists() const override;

	HydraulicDataRiverSurveyWaterElevationItem* getItem(const QString& name);
	void addItem(const QString& name, bool specified, double value);
	void clear();

	QColor color() const;
	void setColor(const QColor& color);

	void pushEditCaptionCommand(const QString& caption);
	void pushEditColorCommand(const QColor& color);
	void pushEditItemCommand(const QString& name, bool specified, double value);

private:
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;

	class Impl;
	Impl* impl;

	class EditCaptionCommand;
	class EditColorCommand;
	class EditItemCommand;

public:
	friend class GeoDataRiverSurveyCrosssectionWindow;
};

#include "private/hydraulicdatariversurveywaterelevation_impl.h"

#endif // HYDRAULICDATARIVERSURVEYWATERELEVATION_H
