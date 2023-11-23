#ifndef GRIDCOMPLEXCONDITIONGROUP_H
#define GRIDCOMPLEXCONDITIONGROUP_H

#include "../../guicore_global.h"

#include <QColor>
#include <QObject>
#include <QString>

class SolverDefinition;
class InputConditionContainerSet;

class QDomElement;
class QLabel;
class QWidget;

namespace iRICLib {
	class H5CgnsConditionGroup;
} // namespace iRICLib

class GUICOREDLL_EXPORT GridComplexConditionGroup : public QObject
{
	Q_OBJECT

public:
	class GUICOREDLL_EXPORT Setting {
	public:
		Setting();
		Setting(const Setting& s);
		Setting(Setting&& s);

		~Setting();

		Setting copy() const;
		Setting& operator=(const Setting& setting);

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
	const std::vector<QWidget*>& tableLabels() const;
	const std::vector<QWidget*>& tableWidgets() const;
	InputConditionContainerSet* containerSet() const;
	static std::vector<std::string> widgetNames(const QDomElement& elem);

	QString caption() const;
	QColor color() const;
	bool isDefault() const;
	Setting setting() const;

	void backup();
	void restore();

signals:
	void captionChanged(const QString& caption);
	void colorChanged(const QColor& color);
	void isDefaultChanged(bool isDefault);

public slots:
	void setCaption(const QString& caption);
	void setColor(const QColor& color);
	void setIsDefault(bool isDefault);
	void setSetting(const Setting& setting);

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

#ifdef _DEBUG
	#include "private/gridcomplexconditiongroup_impl.h"
#endif // _DEBUG

#endif // GRIDCOMPLEXCONDITIONGROUP_H
