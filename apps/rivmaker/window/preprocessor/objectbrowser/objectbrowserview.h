#ifndef OBJECTBROWSERVIEW_H
#define OBJECTBROWSERVIEW_H

#include <QTreeView>

class ObjectBrowserView : public QTreeView
{
	Q_OBJECT

public:
	ObjectBrowserView(QWidget* w);
	~ObjectBrowserView();

	void select(const QModelIndex& index);

signals:
	void itemSelected(const QModelIndex& selected);

private:
	void currentChanged(const QModelIndex& current, const QModelIndex& previous) override;
};

#endif // OBJECTBROWSERVIEW_H
