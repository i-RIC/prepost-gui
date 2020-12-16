#include "networksetting.h"
#include "qttool.h"

#include <QNetworkAccessManager>
#include <QStandardItem>

namespace {

QNetworkAccessManager* m_networkAccessManager = nullptr;

}

void QtTool::checkItem(QStandardItem* item, bool checked)
{
	if (checked) {
		item->setCheckState(Qt::Checked);
	} else {
		item->setCheckState(Qt::Unchecked);
	}
}

void QtTool::initNetworkAccessManager()
{
	m_networkAccessManager = new QNetworkAccessManager();
	NetworkSetting setting;
	auto proxies = setting.queryProxy();

	QNetworkProxy proxy;
	proxy.setType(QNetworkProxy::NoProxy);

	if (proxies.size() != 0) {
		proxy = proxies.first();
	}

	m_networkAccessManager->setProxy(proxy);
}

void QtTool::destroyNetworkAccessManager()
{
	delete m_networkAccessManager;
	m_networkAccessManager = nullptr;
}

QNetworkAccessManager* QtTool::networkAccessManager()
{
	return m_networkAccessManager;
}

