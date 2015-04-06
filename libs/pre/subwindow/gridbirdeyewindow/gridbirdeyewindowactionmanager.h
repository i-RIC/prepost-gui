#ifndef GRIDBIRDEYEWINDOWACTIONMANAGER_H
#define GRIDBIRDEYEWINDOWACTIONMANAGER_H

#include <QObject>
class QAction;
class QMenu;
class GridBirdEyeWindow;

class GridBirdEyeWindowActionManager : public QObject
{
	Q_OBJECT
public:
	/// Constructor
	GridBirdEyeWindowActionManager(GridBirdEyeWindow* parent);
	QAction* drawDisplaySettingAction;
	QAction* backgroundColorAction;
	QAction* editZScaleAction;

	QMenu* drawMenu(){return m_drawMenu;}
//	void connectWithDataModel();
private:
	void init();
	void setupDrawMenu();
	QMenu* m_drawMenu;
	GridBirdEyeWindow* m_gridBirdEyeWindow;
};

#endif // GRIDBIRDEYEWINDOWACTIONMANAGER_H
