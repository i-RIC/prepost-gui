#ifndef OBJECTBROWSERVIEW_H
#define OBJECTBROWSERVIEW_H

#include "guibase_global.h"

#include <QTreeView>
#include <QPoint>

class QAction;

class GUIBASEDLL_EXPORT ObjectBrowserView : public QTreeView
{
	Q_OBJECT

public:
	ObjectBrowserView(QWidget* parent);
	QAction* deleteAction() const {return m_deleteAction;}
	QAction* moveUpAction() const {return m_moveUpAction;}
	QAction* moveDownAction() const {return m_moveDownAction;}
	QAction* propertyAction() const {return m_propertyAction;}
	QSize sizeHint() const {return QSize(250, 200);}
	void setCommandExecution(bool exec){m_commandExecution = exec;}
	void select(const QModelIndex& index);

protected slots:
	void handlePress(const QModelIndex& index);
	void deleteCurrentItem();
	void moveUpCurrentItem();
	void moveDownCurrentItem();
	void showPropertyForCurrentItem();

signals:
	void selectionChanged();
	void itemSelected(const QModelIndex& selected);
	void itemDeselected(const QModelIndex& deselected);
	void pressed(const QModelIndex &index, const QPoint& globalPos);
	void requestDeleteItem(const QModelIndex &index);
	void requestMoveUpItem(const QModelIndex &index);
	void requestMoveDownItem(const QModelIndex &index);
	void requestShowPropertyDialog(const QModelIndex &index);
	void requestDeleteImage(const QModelIndex &index);
	void requestMoveUpImage(const QModelIndex &index);
	void requestMoveDownImage(const QModelIndex &index);
	void requestDeleteMeasuredData(const QModelIndex &index);
	void requestMoveUpMeasuredData(const QModelIndex &index);
	void requestMoveDownMeasuredData(const QModelIndex &index);

protected:
	void mousePressEvent(QMouseEvent* event);
	void currentChanged(const QModelIndex &current, const QModelIndex &previous);
	QPoint m_pressGlobalPos;
	QAction* m_moveUpAction;
	QAction* m_moveDownAction;
	QAction* m_deleteAction;
	QAction* m_propertyAction;

private:
	bool m_commandExecution;
	bool m_isPushing;
};

#endif // ObjectBrowserView_H
