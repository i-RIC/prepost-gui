#ifndef POST2DBIRDEYEWINDOWACTIONMANAGER_H
#define POST2DBIRDEYEWINDOWACTIONMANAGER_H

#include <QObject>
class QAction;
class QMenu;
class QToolBar;
class Post2dBirdEyeWindow;

class Post2dBirdEyeWindowActionManager : public QObject
{
	Q_OBJECT

public:
	Post2dBirdEyeWindowActionManager(Post2dBirdEyeWindow* parent);

	QAction* drawGridAction;
	QAction* drawContourAction;
	QAction* drawCellScalarAction;

	QAction* drawTitleAction;
	QAction* drawTimeAction;

	QAction* backgroundColorAction;
	QAction* editZScaleAction;

	QMenu* drawMenu() const;
	void connectWithDataModel();

private:
	void setupDrawMenu();
	void setupViewToolBar();

	QMenu* m_drawMenu;
	Post2dBirdEyeWindow* m_post2dWindow;
};

#endif // POST2DBIRDEYEWINDOWACTIONMANAGER_H
