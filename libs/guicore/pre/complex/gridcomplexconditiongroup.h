#ifndef GRIDCOMPLEXCONDITIONGROUP_H
#define GRIDCOMPLEXCONDITIONGROUP_H

#include "../../guicore_global.h"

#include <QColor>
#include <QString>

class SolverDefinition;
class InputConditionContainerSet;

class QDomElement;
class QWidget;

namespace iRICLib {
	class H5CgnsConditionGroup;
} // namespace iRICLib

class GUICOREDLL_EXPORT GridComplexConditionGroup
{
public:
	class GUICOREDLL_EXPORT Setting {
	public:
		Setting();
		Setting(const Setting& s);
		Setting(Setting&& s);

		~Setting();

		QString caption;
		QColor color;
		bool isDefault;
		InputConditionContainerSet* containerSet;
	};

	GridComplexConditionGroup(SolverDefinition* def, const QDomElement& elem);
	~GridComplexConditionGroup();

	int load(const iRICLib::H5CgnsConditionGroup& group);
	int save(iRICLib::H5CgnsConditionGroup* group);

	QWidget* widget() const;
	InputConditionContainerSet* containerSet() const;

	QString caption() const;
	void setCaption(const QString& caption);

	QColor color() const;
	void setColor(const QColor& color);

	bool isDefault() const;
	void setIsDefault(bool isDefault);

	Setting setting() const;
	void setSetting(const Setting& setting);

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

#ifdef _DEBUG
	#include "private/gridcomplexconditiongroup_impl.h"
#endif // _DEBUG

#endif // GRIDCOMPLEXCONDITIONGROUP_H
