#ifndef GEODATAPOINTGROUP_PROPERTYDIALOG_H
#define GEODATAPOINTGROUP_PROPERTYDIALOG_H

#include "../geodatapointgroup.h"

#include <QDialog>
#include <QPixmap>

namespace Ui {
class GeoDataPointGroup_PropertyDialog;
}

class GeoDataPointGroup::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	PropertyDialog(GeoDataPointGroup* group, QWidget *parent);
	~PropertyDialog();

	QUndoCommand* createModifyCommand(bool apply);

	void accept() override;
	void reject() override;

private slots:
	void importImage();
	void updateImage();
	void clearImage();
	void handleButtonClick(QAbstractButton* button);

private:
	Setting setting() const;
	void setSetting(const Setting& setting);

	void apply();
	bool m_applied;
	QPixmap m_pixmap;

	GeoDataPointGroup* m_group;

	Ui::GeoDataPointGroup_PropertyDialog *ui;
};

#endif // GEODATAPOINTGROUP_PROPERTYDIALOG_H
