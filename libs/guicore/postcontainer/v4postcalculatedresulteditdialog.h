#ifndef V4POSTCALCULATEDRESULTEDITDIALOG_H
#define V4POSTCALCULATEDRESULTEDITDIALOG_H

#include "v4postcalculatedresult.h"

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class v4PostCalculatedResultEditDialog;
}

class v4PostZoneDataContainer;

class v4PostCalculatedResultEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit v4PostCalculatedResultEditDialog(QWidget *parent = nullptr);
	~v4PostCalculatedResultEditDialog();

	void setZoneDataContainer(v4PostZoneDataContainer* container);

	v4PostCalculatedResult* calculatedResult() const;
	void setCalculatedResult(v4PostCalculatedResult* result);
	void setCalculatedResultIndex(int index);

public slots:
	void accept() override;
	void reject() override;

private slots:
	void addArgument();
	void deleteSelectedArgument();
	void handlePositionChange();

	void test();

private:
	bool check();
	void applySetting();
	void updateArgumentsTable();

	v4PostCalculatedResult* m_calculatedResult;
	int m_calculatedResultIndex;
	bool m_rejected;
	v4PostZoneDataContainer* m_zoneDataContainer;
	std::vector<v4PostCalculatedResult::DataType> m_dataTypes;

	QStandardItemModel m_model;

	Ui::v4PostCalculatedResultEditDialog *ui;

	class TableDelegate;
	TableDelegate* m_tableDelegate;
};

#endif // V4POSTCALCULATEDRESULTEDITDIALOG_H
