#ifndef TMSLOADER_TMSLOADER_H
#define TMSLOADER_TMSLOADER_H

#include "tmsloader_api.h"

#include <QObject>

class QImage;
class QNetworkProxy;
class QPointF;
class QSize;
class QWidget;

namespace tmsloader {

class TmsRequest;

class TMSLOADER_API TmsLoader : public QObject
{
	Q_OBJECT

public:
	TmsLoader(QWidget* parent);
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
