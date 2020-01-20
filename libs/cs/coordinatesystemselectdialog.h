#ifndef COORDINATESYSTEMSELECTDIALOG_H
#define COORDINATESYSTEMSELECTDIALOG_H

#include "cs_api.h"

#include <QDialog>

class CoordinateSystem;
class CoordinateSystemBuilder;

namespace Ui
{
	class CoordinateSystemSelectDialog;
}

/// Dialog to select coordinate system from well-known systems
class CS_API CoordinateSystemSelectDialog : public QDialog
{
	Q_OBJECT

public:
	explicit CoordinateSystemSelectDialog(QWidget* parent = nullptr);
	~CoordinateSystemSelectDialog();

	void setBuilder(CoordinateSystemBuilder* builder);
	void setForceSelect(bool force);

	CoordinateSystem* coordinateSystem() const;
	void setCoordinateSystem(CoordinateSystem* cs);

private slots:
	void updateList();
	void updateCurrent(int index);

private:
	void updateOkButtonStatus();

	CoordinateSystemBuilder* m_builder;

	QList<CoordinateSystem*> m_listSystems;
	CoordinateSystem* m_currentSystem;
	bool m_forceSelect;

	Ui::CoordinateSystemSelectDialog* ui;
};

#endif // COORDINATESYSTEMSELECTDIALOG_H
