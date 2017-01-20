#ifndef GEODATAPOINTMAPWEBIMPORTER_H
#define GEODATAPOINTMAPWEBIMPORTER_H

#include "gd_pointmap_global.h"
#include <guicore/pre/geodata/geodatawebimporter.h>

class WebMeratorUtil;
class CoordinateSystem;
class QNetworkAccessManager;
class QNetworkReply;

class GD_POINTMAP_EXPORT GeoDataPointmapWebImporter : public GeoDataWebImporter
{
	Q_OBJECT

public:
	GeoDataPointmapWebImporter(GeoDataCreator* creator);
	~GeoDataPointmapWebImporter();

	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const override;

	bool importData(GeoData* data, int index, QWidget* w) override;

private slots:
	void abortRequest();
	void handleDone();

private:
	bool doInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w) override;

	double m_lonMin;
	double m_lonMax;
	double m_latMin;
	double m_latMax;

	int m_zoomLevel;
	QString m_url;

	QNetworkAccessManager* m_webAccessManager;
	QNetworkReply* m_webReply;

	bool m_isWaitingHttpResponse;

	CoordinateSystem* m_coordinateSystem;
	WebMeratorUtil* m_wmUtil;
};

#endif // GEODATAPOINTMAPWEBIMPORTER_H
