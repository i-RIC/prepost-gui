#ifndef GEODATAPOINTMAPRITER3DLOADER_H
#define GEODATAPOINTMAPRITER3DLOADER_H

#include <QObject>

class FolderRemover;
class GeoDataPointmap;
class iRICMainWindowI;

class QWidget;

class GeoDataPointmapRiter3dLoader : public QObject
{
	Q_OBJECT

public:
	GeoDataPointmapRiter3dLoader(const QString& fileName);
	~GeoDataPointmapRiter3dLoader();

	bool open(const QString& tmpFileName, iRICMainWindowI* mainWindow);
	void close();

	bool check(QWidget* w);
	bool load(GeoDataPointmap* pointmap, QWidget* w);

private:
	QString m_fileName;
	QString m_tmpFileName;
	FolderRemover* m_folderRemover;

	class TarHeaderBlock;
};

#endif // GEODATAPOINTMAPRITER3DLOADER_H
