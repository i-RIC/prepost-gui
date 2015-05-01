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
	void update();
	void setColors(QMap<double, QColor> map) {
		m_colors = map;
		update();
	}
	void setEnumerations(QMap<double, QString> map) {m_enumerations = map;}
	void setEnglishEnumerations(QMap<double, QString> map) {m_englishEnumerations = map;}
	const QMap<double, QString>& enumerations() {return m_enumerations;}
	const QMap<double, QColor> colors() {return m_colors;}
	const QMap<double, QString>& englishEnumerations() {return m_englishEnumerations;}

protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);

	QMap<double, QString> m_enumerations;
	QMap<double, QString> m_englishEnumerations;
	QMap<double, QColor> m_colors;
};

#endif // COLORTRANSFERFUNCTIONCONTAINER_H
