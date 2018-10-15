#ifndef STARTPAGELINKS_H
#define STARTPAGELINKS_H

#include <QWidget>

class QVBoxLayout;

class StartPageLinks : public QWidget
{
	Q_OBJECT

public:
	explicit StartPageLinks(QWidget* parent = nullptr);
	void setupLinks(const QString& locale);

private slots:
	void handleLinkClick();

private:
	void addGroup(const QString& title);
	void addLink(const QString& title, const QString& url);
	QVBoxLayout* m_layout;
};

#endif // STARTPAGELINKS_H
