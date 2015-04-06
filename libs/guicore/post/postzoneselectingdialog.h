#ifndef POSTZONESELECTINGDIALOG_H
#define POSTZONESELECTINGDIALOG_H

#include "../guicore_global.h"
#include <QDialog>
#include <QString>
#include <QList>
#include <QMap>

namespace Ui {
	class PostZoneSelectingDialog;
}

class SolverDefinitionGridType;
class PostZoneDataContainer;

class GUICOREDLL_EXPORT PostZoneSelectingDialog : public QDialog
{
	Q_OBJECT
public:
	explicit PostZoneSelectingDialog(QWidget *parent = 0);
	~PostZoneSelectingDialog();
	void setContainers(const QList<PostZoneDataContainer*>& containers);
	QString gridTypeName();
	QString zoneName();

private slots:
	void setupZoneList(int index);

private:
	void setupGridTypeComboBox();
	Ui::PostZoneSelectingDialog *ui;
	QList<SolverDefinitionGridType*> m_gridTypes;
	QMap<SolverDefinitionGridType*, QList<PostZoneDataContainer*> > m_zoneLists;
};

#endif // POSTZONESELECTINGDIALOG_H
