#ifndef IRICMIMAINWINDOW_H
#define IRICMIMAINWINDOW_H

#include "../migui_api.h"

#include <QMainWindow>

namespace Ui {
class iRICMIMainWindow;
}

class QTableView;

class MIGUI_API iRICMIMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	static const int STATUSBAR_DISPLAYTIME = 5000;

	explicit iRICMIMainWindow(QWidget *parent = nullptr);
	~iRICMIMainWindow();

	const QLocale& locale() const;

	QTableView* modelsTable() const;
	QTableView* connectionsTable() const;

	void closeEvent(QCloseEvent* event) override;

private slots:
	void newProject();
	void openProject();
	void saveProject();
	void closeProject();

	void reload();

	void addModel();
	void addExistingModels();
	void removeModel();

	void addConnection();
	void removeConnection();

	void handleModelTableClick(const QModelIndex& index);

private:
	QStatusBar* statusBar() const;

	class Impl;
	Impl* impl;

	Ui::iRICMIMainWindow *ui;

	class ModelsTableDelegate;
};

#endif // IRICMIMAINWINDOW_H
