#ifndef COLORTRANSFERFUNCTIONCONTAINER_H
#define COLORTRANSFERFUNCTIONCONTAINER_H

#include "../guicore_global.h"
#include "scalarstocolorscontainer.h"

#include <QColor>
#include <QMap>

class GUICOREDLL_EXPORT ColorTransferFunctionContainer : public ScalarsToColorsContainer
{
public:
	ColorTransferFunctionContainer(ProjectDataItem* d);

	void update() override;

	const QMap<double, QColor> colors() const;
	void setColors(const QMap<double, QColor>& map);

	const QMap<double, QString>& enumerations() const;
	void setEnumerations(const QMap<double, QString>& map);

	const QMap<double, QString>& englishEnumerations() const;
	void setEnglishEnumerations(const QMap<double, QString>& map);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QMap<double, QString> m_enumerations;
	QMap<double, QString> m_englishEnumerations;
	QMap<double, QColor> m_colors;
};

#endif // COLORTRANSFERFUNCTIONCONTAINER_H
