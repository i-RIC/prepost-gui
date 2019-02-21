#ifndef POSTCALCULATEDRESULTARGUMENTEDITDIALOG_H
#define POSTCALCULATEDRESULTARGUMENTEDITDIALOG_H

#include <QDialog>

namespace Ui {
class PostCalculatedResultArgumentEditDialog;
}

class PostCalculatedResult;
class PostCalculatedResultArgument;
class PostZoneDataContainer;

class PostCalculatedResultArgumentEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PostCalculatedResultArgumentEditDialog(QWidget *parent = nullptr);
	~PostCalculatedResultArgumentEditDialog();

	PostCalculatedResultArgument* argument() const;
	void setArgument(PostCalculatedResultArgument* argument);

	void setCalculatedResult(PostCalculatedResult* result);
	void setCalculatedResultIndex(int index);
	void setZoneDataContainer(PostZoneDataContainer* container);

public slots:
	void accept() override;
	void reject() override;

private:
	void setupComboBox();

	PostCalculatedResultArgument* m_argument;

	PostCalculatedResult* m_calculatedResult;
	int m_calculatedResultIndex;
	PostZoneDataContainer* m_zoneDataContainer;
	bool m_rejected;

	Ui::PostCalculatedResultArgumentEditDialog *ui;
};

#endif // POSTCALCULATEDRESULTARGUMENTEDITDIALOG_H
