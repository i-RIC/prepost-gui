#ifndef IRICMIMAINWINDOW_H
#define IRICMIMAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class iRICMIMainWindow; }
QT_END_NAMESPACE

class iRICMIMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	iRICMIMainWindow(QWidget *parent = nullptr);
	~iRICMIMainWindow();

private:
	void addConnection();
	void addModel();

private:
	void setupModelsModel();
	void setupConnectionsModel();

	QStandardItemModel m_modelsModel;
	QStandardItemModel m_connectionsModel;

	Ui::iRICMIMainWindow *ui;

	class ModelsDelegate;
};
#endif // IRICMIMAINWINDOW_H
