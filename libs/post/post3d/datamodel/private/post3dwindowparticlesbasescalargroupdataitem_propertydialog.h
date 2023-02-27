#ifndef POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_PROPERTYDIALOG_H
#define POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_PROPERTYDIALOG_H

#include "../post3dwindowparticlesbasescalargroupdataitem.h"

#include <QDialog>

#include <postbase/particle/particledatasetting.h>

namespace Ui {
class Post3dWindowParticlesBaseScalarGroupDataItem_PropertyDialog;
}

class Post3dWindowParticlesBaseScalarGroupDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PropertyDialog(Post3dWindowParticlesBaseScalarGroupDataItem* item, QWidget *parent = nullptr);
	~PropertyDialog();

	QUndoCommand* createModifyCommand(bool apply = false);

	void accept() override;
	void reject() override;

private slots:
	void colorTargetChanged(int index);
	void handleButtonClick(QAbstractButton* button);

private:
	ParticleDataSetting setting() const;
	void setSetting(const ParticleDataSetting& setting);
	void apply();

	void setupColorTargets();
	std::string colorTarget() const;

	bool m_applied;

	std::vector<std::string> m_colorTargets;

	Post3dWindowParticlesBaseScalarGroupDataItem* m_item;

	Ui::Post3dWindowParticlesBaseScalarGroupDataItem_PropertyDialog *ui;
};

#endif // POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_PROPERTYDIALOG_H
