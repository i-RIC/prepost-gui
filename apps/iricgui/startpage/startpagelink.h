#ifndef STARTPAGELINK_H
#define STARTPAGELINK_H

#include "startpagelabel.h"

class StartPageLink : public StartPageLabel
{
public:
	static const int URLMAXLEN = 40;
	static const int URLLASTLEN = 15;
	StartPageLink(const QString& title, const QString& url, QWidget* parent);
	const QString& url() {return m_url;}
private:
	QString m_url;
};

#endif // STARTPAGELINK_H
