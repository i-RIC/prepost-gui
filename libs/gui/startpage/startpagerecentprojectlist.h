#ifndef STARTPAGERECENTPROJECTLIST_H
#define STARTPAGERECENTPROJECTLIST_H

#include <QWidget>

class QVBoxLayout;

class StartPageRecentProjectList : public QWidget
{
	Q_OBJECT

private:
	const static int MAXPROJECTS = 6;

public:
	explicit StartPageRecentProjectList(QWidget* parent = nullptr);

signals:
	void projectSelected(const QString& filename);

private slots:
	void handleProjectSelection();

private:
	void setupItems();
	void add(const QString& projectFileName);
	void setup();

	QVBoxLayout* m_layout;
	int m_numberOfProjects;
};

#endif // STARTPAGERECENTPROJECTLIST_H
