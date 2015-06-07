#include "startpagelink.h"

#include <QUrl>

StartPageLink::StartPageLink(const QString& title, const QString& url, QWidget* parent)
	: StartPageLabel(parent)
{
	m_url = url;
	setTitle(title);
	QString shortenUrl = url;
	if (shortenUrl.length() >= URLMAXLEN) {
		QString last = url.right(URLLASTLEN);
		QString head = url.left(URLMAXLEN - URLLASTLEN - 3);
		shortenUrl = head;
		shortenUrl.append("...");
		shortenUrl.append(last);
	}
	setSubtitle(shortenUrl);
	setToolTip(url);
}
