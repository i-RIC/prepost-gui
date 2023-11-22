#ifndef V4POSTZONESELECTINGDIALOG_H
#define V4POSTZONESELECTINGDIALOG_H

#include "../guicore_global.h"

#include <QDialog>

#include <string>
#include <unordered_map>
#include <vector>

namespace Ui
{
	class v4PostZoneSelectingDialog;
}

class SolverDefinitionGridType;
class v4PostZoneDataContainer;

class QString;

class GUICOREDLL_EXPORT v4PostZoneSelectingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit v4PostZoneSelectingDialog(QWidget* parent = nullptr);
	~v4PostZoneSelectingDialog();

	void setContainers(const std::vector<v4PostZoneDataContainer*>& containers);
	std::string gridTypeName() const;
	std::string zoneName() const;

private slots:
	void setupZoneList(int index);

private:
	void setupGridTypeComboBox();
	SolverDefinitionGridType* currentGridType() const;

	Ui::v4PostZoneSelectingDialog* ui;
	std::vector<SolverDefinitionGridType*> m_gridTypes;
	std::unordered_map<SolverDefinitionGridType*, std::vector<v4PostZoneDataContainer*> > m_zoneLists;
};

#endif // V4POSTZONESELECTINGDIALOG_H
