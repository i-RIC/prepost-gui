#ifndef GRAPH2DSCATTEREDWINDOWACTIONMANAGER_H
#define GRAPH2DSCATTEREDWINDOWACTIONMANAGER_H

#include <QObject>
class QAction;
class QMenu;
class QToolBar;
class Graph2dScatteredWindow;

class Graph2dScatteredWindowActionManager : public QObject
{
	Q_OBJECT

public:
	/// Constructor
	Graph2dScatteredWindowActionManager(Graph2dScatteredWindow* parent);
	QAction* drawDatasourceSettingAction;
	QAction* drawDrawSettingAction;
	QAction* drawAxisSettingAction;
	QAction* drawLegendAction;

	QMenu* drawMenu() const {return m_drawMenu;}
	void connectWithDataModel();

private:
	void init();
	void setupDrawMenu();
	QMenu* m_drawMenu;
	Graph2dScatteredWindow* m_graphWindow;
};

#endif // GRAPH2DSCATTEREDWINDOWACTIONMANAGER_H
