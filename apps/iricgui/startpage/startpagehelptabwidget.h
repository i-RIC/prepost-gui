#ifndef STARTPAGEHELPTABWIDGET_H
#define STARTPAGEHELPTABWIDGET_H

#include <QWidget>

class StartPageLinks;
class StartPageHelpTabWidget : public QWidget
{
	Q_OBJECT
public:
	explicit StartPageHelpTabWidget(QWidget *parent = 0);
	void setLocale(const QString &locale);
private:
	StartPageLinks* m_links;
};

#endif // STARTPAGEHELPTABWIDGET_H
