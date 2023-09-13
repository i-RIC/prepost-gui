#ifndef GRAPH2DSCATTEREDWINDOW_H
#define GRAPH2DSCATTEREDWINDOW_H

#include <guicore/post/postprocessorwindow.h>
#include <guicore/base/additionalmenuwindowi.h>

class ObjectBrowser;
class Graph2dScatteredWindowObjectBrowser;
class Graph2dScatteredWindowDataModel;
class Graph2dScatteredWindowActionManager;
class Graph2dScatteredWindowProjectDataItem;
class Graph2dScatteredWindowControlWidget;

class Graph2dScatteredWindow :
	public PostProcessorWindow,
	public AdditionalMenuWindowI
{
	Q_OBJECT

public:
	explicit Graph2dScatteredWindow(QWidget* parent, int index, Graph2dScatteredWindowProjectDataItem* pdi);
	~Graph2dScatteredWindow();

	QPixmap snapshot() const override;
	QWidget* snapshotArea() const override;

	QList<QMenu*> getAdditionalMenus() const override;
	const std::shared_ptr<QToolBar>& getAdditionalToolBar() const override;

	Graph2dScatteredWindowControlWidget* controlWidget() const;

signals:
	void closeButtonClicked();

private:
	void init();
	void setupDefaultGeometry(int index) override;

	Graph2dScatteredWindowObjectBrowser* m_objectBrowser;
	Graph2dScatteredWindowControlWidget* m_controlWidget;
	Graph2dScatteredWindowDataModel* m_dataModel;
	Graph2dScatteredWindowActionManager* m_actionManager;
	QByteArray m_initialState;
	std::shared_ptr<QToolBar> m_toolBar;

public:
	friend class Graph2dScatteredWindowProjectDataItem;
	friend class Graph2dScatteredWindowActionManager;
};

#endif // GRAPH2DSCATTEREDWINDOW_H
