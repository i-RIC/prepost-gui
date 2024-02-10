#ifndef GEODATARIVERSURVEYWEBIMPORTER_H
#define GEODATARIVERSURVEYWEBIMPORTER_H

#include <guicore/pre/geodata/geodatawebimporter.h>

class GeoDataRiverSurveyMlitImporter;

class QNetworkAccessManager;
class QNetworkReply;

class GeoDataRiverSurveyWebImporter : public GeoDataWebImporter
{
	Q_OBJECT

public:
	GeoDataRiverSurveyWebImporter(GeoDataCreator* creator);
	~GeoDataRiverSurveyWebImporter();

	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const override;

	bool importData(GeoData* data, int index, QWidget* w) override;

private slots:
	void abortRequest();
	void handleDone();

private:
	bool doInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;

	int m_zoomLevel;
	QString m_url;

	QNetworkAccessManager* m_webAccessManager;
	QNetworkReply* m_webReply;

	bool m_isWaitingHttpResponse;
        GeoDataRiverSurveyMlitImporter* m_mlitImporter;

	struct Setting {
		QString riverId;
		QString minKp;
		QString maxKp;
		QString year;
	};

	Setting m_setting;
	QString m_tmpFile;
	QString m_tmpFolder;

	class SettingDialog;
};

#endif // GEODATARIVERSURVEYWEBIMPORTER_H
