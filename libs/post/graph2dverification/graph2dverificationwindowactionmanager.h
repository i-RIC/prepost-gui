#ifndef GRAPH2DVERIFICATIONWINDOWACTIONMANAGER_H
#define GRAPH2DVERIFICATIONWINDOWACTIONMANAGER_H

#include <QObject>
class QAction;
class QMenu;
class QToolBar;
class Graph2dVerificationWindow;

class Graph2dVerificationWindowActionManager : public QObject
{
	Q_OBJECT

public:
	/// Constructor
	Graph2dVerificationWindowActionManager(Graph2dVerificationWindow* parent);
	QAction* drawDatasourceSettingAction;
	QAction* drawDrawSettingAction;
	QAction* drawMarkerSettingAction;
	QAction* drawKPMarkerSettingAction;
	QAction* drawAxisSettingAction;
	QAction* drawCopyAction;
	QAction* drawLegendAction;

	QMenu* drawMenu() const {return m_drawMenu;}
	void connectWithDataModel();

private:
	void init();
	void setupDrawMenu();
	QMenu* m_drawMenu;
	Graph2dVerificationWindow* m_graphWindow;
};

#endif // GRAPH2DVERIFICATIONWINDOWACTIONMANAGER_H
