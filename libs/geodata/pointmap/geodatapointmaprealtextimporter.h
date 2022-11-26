#ifndef GEODATAPOINTMAPREALTEXTIMPORTER_H
#define GEODATAPOINTMAPREALTEXTIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

class GeoDataPointmapRealTextImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataPointmapRealTextImporter(GeoDataCreator* creator);

	bool importData(GeoData *data, int index, QWidget *w) override;
	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;

private slots:
	void cancel();

private:
	bool m_canceled;

	class Values;
	class LineParser;
	class SettingDialog;
};

#endif // GEODATAPOINTMAPREALTEXTIMPORTER_H
