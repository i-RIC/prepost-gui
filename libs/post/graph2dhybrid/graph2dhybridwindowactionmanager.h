#ifndef GRAPH2DHYBRIDWINDOWACTIONMANAGER_H
#define GRAPH2DHYBRIDWINDOWACTIONMANAGER_H

#include <QObject>
class QAction;
class QMenu;
class QToolBar;
class Graph2dHybridWindow;

class Graph2dHybridWindowActionManager : public QObject
{
	Q_OBJECT
public:
	/// Constructor
	Graph2dHybridWindowActionManager(Graph2dHybridWindow* parent);
	QAction* drawDatasourceSettingAction;
	QAction* drawDrawSettingAction;
	QAction* drawMarkerSettingAction;
	QAction* drawKPMarkerSettingAction;
	QAction* drawAxisSettingAction;
	QAction* drawCopyAction;
    QAction* drawLegendAction;

	QMenu* drawMenu(){return m_drawMenu;}
	void connectWithDataModel();
private:
	void init();
	void setupDrawMenu();
	QMenu* m_drawMenu;
	Graph2dHybridWindow* m_graphWindow;
};

#endif // GRAPH2DHYBRIDWINDOWACTIONMANAGER_H
