#ifndef POST3DWINDOWACTIONMANAGER_H
#define POST3DWINDOWACTIONMANAGER_H

#include <QObject>
class QAction;
class QMenu;
class QToolBar;
class Post3dWindow;

class Post3dWindowActionManager : public QObject
{
	Q_OBJECT
public:
	/// Constructor
	Post3dWindowActionManager(Post3dWindow* parent);
	QAction* drawGridAction;
	QAction* drawSurfaceAction;
	QAction* drawContourAction;
	QAction* drawVectorAction;
	QAction* drawStreamlineAction;
	QAction* drawParticleTraceAction;
	QAction* drawIsoSurfaceAction;

	QAction* drawTitleAction;
	QAction* drawTimeAction;

	QAction* drawColorBarAction;

	QMenu* drawMenu() const {return m_drawMenu;}
	void connectWithDataModel();

private:
	void init();
	void setupDrawMenu();
	QMenu* m_drawMenu;
	Post3dWindow* m_post3dWindow;
};

#endif // POST3DWINDOWACTIONMANAGER_H
