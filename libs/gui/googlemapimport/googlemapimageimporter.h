#ifndef GOOGLEMAPIMAGEIMPORTER_H
#define GOOGLEMAPIMAGEIMPORTER_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class QFile;
class iRICMainWindowInterface;
class ProjectData;

class GoogleMapImageImporter : public QObject
{
	Q_OBJECT

public:
	const static double METERPERPIXEL_AT_EQUATOR_1;
	const static int MAX_GOOGLEAPI_COUNT;

	GoogleMapImageImporter(ProjectData* projectdata, iRICMainWindowInterface* parent);

	void importImages();

private slots:
	void abortGoogleMapRequest();
	void handleGoogleMapDone();

private:
	iRICMainWindowInterface* mainWindow();
	QString getNextGoogleMapFileName();

	QNetworkAccessManager* m_googleMapAccessManager;
	QNetworkReply* m_googleMapReply;
	QFile* m_googleMapFile;
	ProjectData* m_projectData;

	bool m_isWaitingHttpResponse;
};

#endif // GOOGLEMAPIMAGEIMPORTER_H
