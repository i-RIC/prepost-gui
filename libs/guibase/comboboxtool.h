#ifndef COMBOBOXTOOL_H
#define COMBOBOXTOOL_H

#include "guibase_global.h"

#include <string>
#include <vector>

class QComboBox;
class QStringList;

class GUIBASEDLL_EXPORT ComboBoxTool
{
public:
	static void setupItems(const std::vector<std::string>& items, QComboBox* comboBox);
	static void setupItems(const QStringList& items, QComboBox* comboBox);

private:
	ComboBoxTool();
	~ComboBoxTool();
};

#endif // COMBOBOXTOOL_H
