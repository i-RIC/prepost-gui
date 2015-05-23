#ifndef OBJECTBROWSERVIEW_H
#define OBJECTBROWSERVIEW_H

#include "guibase_global.h"

#include <QTreeView>
#include <QPoint>

class QAction;

/// Base class of the view inside object browsers in iRIC subwindows
class GUIBASEDLL_EXPORT ObjectBrowserView : public QTreeView
{
	Q_OBJECT

public:
	ObjectBrowserView(QWidget* parent);
	/// Action to delete the selected item
	QAction* deleteAction() const {return m_deleteAction;}
	/// Action to move up the selected item
	QAction* moveUpAction() const {return m_moveUpAction;}
	/// Action to move down the selected item
	QAction* moveDownAction() const {return m_moveDownAction;}
	/// Action to show property dialog about the selected item
	QAction* propertyAction() const {return m_propertyAction;}
	/// Size hint about the widget
	QSize sizeHint() const {return QSize(250, 200);}
	/// Set command execution mode true
	void setCommandExecution(bool exec) {m_commandExecution = exec;}
	/// Select the item with the specified index
	void select(const QModelIndex& index);

protected slots:
	/// Emit signal pressed()
	void handlePress(const QModelIndex& index);
	/// Delete the selected item
	void deleteCurrentItem();
	/// Move up the selected item
	void moveUpCurrentItem();
	/// Move down the selected item
	void moveDownCurrentItem();
	/// Open property dialog for the selected item
	void showPropertyForCurrentItem();

signals:
	void selectionChanged();
	void itemSelected(const QModelIndex& selected);
	void itemDeselected(const QModelIndex& deselected);
	void pressed(const QModelIndex& index, const QPoint& globalPos);
	void requestDeleteItem(const QModelIndex& index);
	void requestMoveUpItem(const QModelIndex& index);
	void requestMoveDownItem(const QModelIndex& index);
	void requestShowPropertyDialog(const QModelIndex& index);
	void requestDeleteImage(const QModelIndex& index);
	void requestMoveUpImage(const QModelIndex& index);
	void requestMoveDownImage(const QModelIndex& index);
	void requestDeleteMeasuredData(const QModelIndex& index);
	void requestMoveUpMeasuredData(const QModelIndex& index);
	void requestMoveDownMeasuredData(const QModelIndex& index);

protected:
	void mousePressEvent(QMouseEvent* event);
	void currentChanged(const QModelIndex& current, const QModelIndex& previous);
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
