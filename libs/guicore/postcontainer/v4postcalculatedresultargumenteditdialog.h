#ifndef V4POSTCALCULATEDRESULTARGUMENTEDITDIALOG_H
#define V4POSTCALCULATEDRESULTARGUMENTEDITDIALOG_H

#include <QDialog>

namespace Ui {
class v4PostCalculatedResultArgumentEditDialog;
}

class v4PostCalculatedResult;
class v4PostCalculatedResultArgument;
class v4PostZoneDataContainer;

class v4PostCalculatedResultArgumentEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit v4PostCalculatedResultArgumentEditDialog(QWidget *parent = nullptr);
	~v4PostCalculatedResultArgumentEditDialog();

	v4PostCalculatedResultArgument* argument() const;
	void setArgument(v4PostCalculatedResultArgument* argument);

	void setCalculatedResult(v4PostCalculatedResult* result);
	void setCalculatedResultIndex(int index);
	void setZoneDataContainer(v4PostZoneDataContainer* container);

public slots:
	void accept() override;
	void reject() override;

private:
	void setupComboBox();

	v4PostCalculatedResultArgument* m_argument;

	v4PostCalculatedResult* m_calculatedResult;
	int m_calculatedResultIndex;
	v4PostZoneDataContainer* m_zoneDataContainer;
	bool m_rejected;

	Ui::v4PostCalculatedResultArgumentEditDialog *ui;
};

#endif // V4POSTCALCULATEDRESULTARGUMENTEDITDIALOG_H
