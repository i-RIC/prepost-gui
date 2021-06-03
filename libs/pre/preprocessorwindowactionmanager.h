#ifndef PREPROCESSORWINDOWACTIONMANAGER_H
#define PREPROCESSORWINDOWACTIONMANAGER_H

#include <QList>
#include <QObject>

class CgnsFileEntry;
class PreProcessorWindow;

class QAction;
class QMenu;
class QToolBar;

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
	QMenu* calcCondMenu() const;
	void connectWithDataModel();

private:
	/// Initialization
	void init();
	void setupCalcCondMenu();

	QMenu* m_calcCondMenu;
	PreProcessorWindow* m_preWindow;
};

#endif // PREPROCESSORWINDOWACTIONMANAGER_H
