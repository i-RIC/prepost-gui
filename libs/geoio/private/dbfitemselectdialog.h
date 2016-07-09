#ifndef DBFITEMSELECTDIALOG_H
#define DBFITEMSELECTDIALOG_H

#include <QDialog>
#include <QVariant>

#include <string>
#include <vector>

namespace Ui {
class DBFItemSelectDialog;
}

class DBFItemSelectDialog : public QDialog
{
	Q_OBJECT

public:
	explicit DBFItemSelectDialog(const QString& DBFFileName, QWidget *parent = 0);
	~DBFItemSelectDialog();

	int selectedItem() const;

private slots:
	void updateTable();

private:
	void setLabels(const QStringList& labels);
	void setData(const std::vector<std::vector<QVariant> >& data);

	std::string m_dbfFileName;

	Ui::DBFItemSelectDialog *ui;
};

#endif // DBFITEMSELECTDIALOG_H
