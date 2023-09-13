#ifndef GRAPH2DHYBRIDWINDOW_H
#define GRAPH2DHYBRIDWINDOW_H

#include <guicore/post/postprocessorwindow.h>
#include <guicore/base/additionalmenuwindowi.h>

class ObjectBrowser;
class Graph2dHybridWindowObjectBrowser;
class Graph2dHybridWindowDataModel;
class Graph2dHybridWindowActionManager;
class Graph2dHybridWindowProjectDataItem;
class Graph2dHybridWindowControlWidget;

class Graph2dHybridWindow :
	public PostProcessorWindow,
	public AdditionalMenuWindowI
{
	Q_OBJECT

public:
	explicit Graph2dHybridWindow(QWidget* parent, int index, Graph2dHybridWindowProjectDataItem* pdi);
	~Graph2dHybridWindow();

	QPixmap snapshot() const override;
	QWidget* snapshotArea() const override;
	QList<QMenu*> getAdditionalMenus() const override;
	const std::shared_ptr<QToolBar>& getAdditionalToolBar() const override;

	Graph2dHybridWindowControlWidget* controlWidget() const;

signals:
	void closeButtonClicked();

private:
	void init();
	void setupDefaultGeometry(int index) override;

	Graph2dHybridWindowObjectBrowser* m_objectBrowser;
	Graph2dHybridWindowControlWidget* m_controlWidget;
	Graph2dHybridWindowDataModel* m_dataModel;
	Graph2dHybridWindowActionManager* m_actionManager;
	QByteArray m_initialState;
	std::shared_ptr<QToolBar> m_toolBar;

public:
	friend class Graph2dHybridWindowProjectDataItem;
	friend class Graph2dHybridWindowActionManager;
};

#endif // GRAPH2DHYBRIDWINDOW_H
