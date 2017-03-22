#ifndef DATAITEMCONTROLLER_H
#define DATAITEMCONTROLLER_H

#include <QObject>

class DataItem;
class Model;
class View;

class QMenu;
class QMouseEvent;
class QKeyEvent;
class QWheelEvent;

class DataItemController : public QObject
{
	Q_OBJECT

public:
	DataItemController(Model* model, DataItem* item);
	virtual ~DataItemController();

	virtual void keyPressEvent(QKeyEvent*, View*);
	virtual void keyReleaseEvent(QKeyEvent*, View*);
	virtual void mouseDoubleClickEvent(QMouseEvent*, View*);
	virtual void mouseMoveEvent(QMouseEvent*, View*);
	virtual void mousePressEvent(QMouseEvent*, View*);
	virtual void mouseReleaseEvent(QMouseEvent*, View*);
	virtual void wheelEvent(QWheelEvent*, View*);

	virtual void restoreMouseCursorOnView(View*);

	QMenu& objectBrowserRightClickMenu();
	QMenu& viewRightClickMenu();

protected:
	DataItem* item();
	Model* model();

	void updateView();

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/dataitemcontroller_impl.h"
#endif // _DEBUG

#endif // DATAITEMCONTROLLER_H
