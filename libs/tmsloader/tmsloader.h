#ifndef TMSLOADER_TMSLOADER_H
#define TMSLOADER_TMSLOADER_H

#include "tmsloader_api.h"

#include <QObject>

class QNetworkProxy;
class QImage;
class QPointF;
class QSize;

namespace tmsloader {

class TmsRequest;

class TMSLOADER_API TmsLoader : public QObject
{
	Q_OBJECT

public:
	TmsLoader(QObject* parent = nullptr);
	~TmsLoader();

	void registerRequest(const TmsRequest& request, int* requestId);
	void cancelRequest(int requestId);

	QImage getImage(int requestId) const;

private slots:
	void handleUpdate();

signals:
	void imageUpdated(int requestId);

private:
	class Impl;
	Impl* impl;
};

} // namespace tmsloader

#endif // TMSLOADER_TMSLOADER_H
