#ifndef POSTSTRINGRESULTARGUMENTEDITDIALOG_H
#define POSTSTRINGRESULTARGUMENTEDITDIALOG_H

#include "poststringresultargument.h"

#include <QDialog>

#include <vector>

namespace Ui {
class PostStringResultArgumentEditDialog;
}

class v4PostZoneDataContainer;

class PostStringResultArgumentEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PostStringResultArgumentEditDialog(QWidget *parent = 0);
	~PostStringResultArgumentEditDialog();

	void setZoneDataContainer(v4PostZoneDataContainer* container);
	PostStringResultArgument* argument();
	void setArgument(PostStringResultArgument* arg);

public slots:
	void accept() override;
	void reject() override;

private slots:
	void handlePositionChange();
	void handleResultChange();

private:
	void updateWidgets();
	void applySetting();

	PostStringResultArgument* m_argument;
	bool m_rejected;
	v4PostZoneDataContainer* m_zoneDataContainer;

	std::vector<PostStringResultArgument::Type> m_types;

	Ui::PostStringResultArgumentEditDialog *ui;
};

#endif // POSTSTRINGRESULTARGUMENTEDITDIALOG_H
