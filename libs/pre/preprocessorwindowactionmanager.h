#ifndef PREPROCESSORWINDOWACTIONMANAGER_H
#define PREPROCESSORWINDOWACTIONMANAGER_H

#include <guicore/project/cgnsfilelist.h>

#include <QList>
#include <QObject>
class QAction;
class QMenu;
class QToolBar;
class PreProcessorWindow;

/// This class manages the actions on iRIC pre-processor window,
/// that is an instance of PreProcessorWindow.
class PreProcessorWindowActionManager : public QObject
{
	Q_OBJECT
public:
	/// Constructor
	PreProcessorWindowActionManager(PreProcessorWindow* parent);

	QAction* calcCondEditAction;
	QAction* calcCondImportAction;
	QAction* calcCondExportAction;
	QMenu* calcCondMenu(){return m_calcCondMenu;}
	void connectWithDataModel();
private slots:
	void informCgnsListUpdate(const QList<CgnsFileList::CgnsFileEntry*>& list);
private:
	/// Initialization
	void init();
	void setupCalcCondMenu();
	QMenu* m_calcCondMenu;
	PreProcessorWindow* m_preWindow;
};

#endif // PREPROCESSORWINDOWACTIONMANAGER_H
