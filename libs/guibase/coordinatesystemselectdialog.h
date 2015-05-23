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
	/// Set the builder to build the list of well-known coordinate systems
	void setBuilder(CoordinateSystemBuilder* builder) {m_builder = builder;}
	/// Set the coordinate system
	void setCoordinateSystem(CoordinateSystem* cs);
	/// The coordinate system
	CoordinateSystem* coordinateSystem() {return m_currentSystem;}

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
