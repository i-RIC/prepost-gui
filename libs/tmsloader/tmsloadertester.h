#ifndef TMSLOADERTESTER_H
#define TMSLOADERTESTER_H

#include <QObject>

namespace tmsloader {

class TmsLoader;

class TmsLoaderTester : public QObject
{
	Q_OBJECT

public:
	TmsLoaderTester(TmsLoader* loader,  QObject* parent);

public slots:
	void handleImageUpdate(int requestId);

private:
	TmsLoader* m_loader;
};

} // tmsloader

#endif // TMSLOADERTESTER_H
