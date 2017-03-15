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

	QAction* addAction() const;
	QAction* deleteAction() const;
	QAction* moveUpAction() const;
	QAction* moveDownAction() const;
	QAction* propertyAction() const;
	QAction* undoableDeleteAction() const;

	QSize sizeHint() const override;
	void select(const QModelIndex& index);

	void setCommandExecution(bool exec);

protected slots:
	/// Emit signal pressed()
	void handlePress(const QModelIndex& index);

	void deleteCurrentItem();
	void moveUpCurrentItem();
	void moveDownCurrentItem();
	void showAddForCurrentItem();
	void showPropertyForCurrentItem();
	void undoableDeleteCurrentItem();

signals:
	void selectionChanged();
	void itemSelected(const QModelIndex& selected);
	void itemDeselected(const QModelIndex& deselected);
	void pressed(const QModelIndex& index, const QPoint& globalPos);
	void requestDeleteItem(const QModelIndex& index);
	void requestMoveUpItem(const QModelIndex& index);
	void requestMoveDownItem(const QModelIndex& index);
	void requestShowAddDialog(const QModelIndex& index);
	void requestShowPropertyDialog(const QModelIndex& index);
	void requestDeleteImage(const QModelIndex& index);
	void requestMoveUpImage(const QModelIndex& index);
	void requestMoveDownImage(const QModelIndex& index);
	void requestDeleteMeasuredData(const QModelIndex& index);
	void requestMoveUpMeasuredData(const QModelIndex& index);
	void requestMoveDownMeasuredData(const QModelIndex& index);
	void requestUndoableDeleteItem(const QModelIndex& index);

private:
	void mousePressEvent(QMouseEvent* event) override;
	void currentChanged(const QModelIndex& current, const QModelIndex& previous) override;

	QPoint m_pressGlobalPos;

	QAction* m_moveUpAction;
	QAction* m_moveDownAction;
	QAction* m_addAction;
	QAction* m_deleteAction;
	QAction* m_propertyAction;
	QAction* m_undoableDeleteAction;

	bool m_commandExecution;
	bool m_isPushing;
};

#endif // ObjectBrowserView_H
