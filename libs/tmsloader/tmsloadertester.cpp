#include "tmsloader.h"
#include "tmsloadertester.h"

#include <QImage>

using namespace tmsloader;

TmsLoaderTester::TmsLoaderTester(TmsLoader* loader,  QObject* parent) :
	QObject (parent),
	m_loader (loader)
{}

void TmsLoaderTester::handleImageUpdate(int requestId)
{
	QImage img = m_loader->getImage(requestId);
	img.save("E:/web.png");
}
