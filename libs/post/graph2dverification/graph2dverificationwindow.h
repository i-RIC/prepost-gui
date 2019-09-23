#ifndef GRAPH2DVERIFICATIONWINDOW_H
#define GRAPH2DVERIFICATIONWINDOW_H

#include <guicore/post/postprocessorwindow.h>
#include <guicore/base/additionalmenuwindowi.h>

class ObjectBrowser;
class Graph2dVerificationWindowObjectBrowser;
class Graph2dVerificationWindowDataModel;
class Graph2dVerificationWindowActionManager;
class Graph2dVerificationWindowProjectDataItem;
//{{
class Graph2dVerificationWindowTopWidget;
//}}
class Graph2dVerificationWindowControlWidget;

class Graph2dVerificationWindow :
	public PostProcessorWindow,
	public AdditionalMenuWindowI
{
	Q_OBJECT

public:
	explicit Graph2dVerificationWindow(QWidget* parent, int index, Graph2dVerificationWindowProjectDataItem* pdi);
	~Graph2dVerificationWindow();
	QPixmap snapshot() override;
	QList<QMenu*> getAdditionalMenus() const override;
	QToolBar* getAdditionalToolBar() const override;

	Graph2dVerificationWindowControlWidget* controlWidget() const;
	Graph2dVerificationWindowTopWidget* topWidget() const;

	void setComment(const QString& comment);

signals:
	void closeButtonClicked();

private:
	void init();
	void setupDefaultGeometry(int index) override;

	Graph2dVerificationWindowObjectBrowser* m_objectBrowser;
	Graph2dVerificationWindowControlWidget* m_controlWidget;
	//{{
	Graph2dVerificationWindowTopWidget* m_topWidget;
	//}}
	Graph2dVerificationWindowDataModel* m_dataModel;
	Graph2dVerificationWindowActionManager* m_actionManager;
	QByteArray m_initialState;

public:
	friend class Graph2dVerificationWindowProjectDataItem;
	friend class Graph2dVerificationWindowActionManager;
};

#endif // GRAPH2DVERIFICATIONWINDOW_H
