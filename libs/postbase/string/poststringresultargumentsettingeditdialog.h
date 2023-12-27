#ifndef POSTSTRINGRESULTARGUMENTSETTINGEDITDIALOG_H
#define POSTSTRINGRESULTARGUMENTSETTINGEDITDIALOG_H

#include "poststringresultargumentcontainer.h"

#include <QDialog>

#include <vector>

namespace Ui {
class PostStringResultArgumentSettingEditDialog;
}

class v4PostZoneDataContainer;

class PostStringResultArgumentSettingEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PostStringResultArgumentSettingEditDialog(QWidget *parent = nullptr);
	~PostStringResultArgumentSettingEditDialog();

	void setZoneDataContainer(v4PostZoneDataContainer* container);
	PostStringResultArgumentContainer setting() const;
	void setSetting(const PostStringResultArgumentContainer& setting);

public slots:
	void accept() override;

private slots:
	void handlePositionChange();
	void handleResultChange();

private:
	v4PostZoneDataContainer* m_zoneDataContainer;
	std::vector<PostStringResultArgumentContainer::Type> m_types;
	std::vector<std::string> m_valueNames;

	Ui::PostStringResultArgumentSettingEditDialog *ui;
};

#endif // POSTSTRINGRESULTARGUMENTSETTINGEDITDIALOG_H
