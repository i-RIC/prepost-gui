#ifndef COORDINATESYSTEMSELECTDIALOG_H
#define COORDINATESYSTEMSELECTDIALOG_H

#include "guibase_global.h"

#include <QDialog>

class CoordinateSystem;
class CoordinateSystemBuilder;

namespace Ui
{
	class CoordinateSystemSelectDialog;
}

/// Dialog to select coordinate system from well-known systems
class GUIBASEDLL_EXPORT CoordinateSystemSelectDialog : public QDialog
{
	Q_OBJECT

public:
	explicit CoordinateSystemSelectDialog(QWidget* parent = nullptr);
	~CoordinateSystemSelectDialog();

	void setBuilder(CoordinateSystemBuilder* builder);

	CoordinateSystem* coordinateSystem() const;
	void setCoordinateSystem(CoordinateSystem* cs);

private slots:
	void updateList();
	void updateCurrent(int index);

private:
	CoordinateSystemBuilder* m_builder;

	QList<CoordinateSystem*> m_listSystems;
	CoordinateSystem* m_currentSystem;

	Ui::CoordinateSystemSelectDialog* ui;
};

#endif // COORDINATESYSTEMSELECTDIALOG_H
