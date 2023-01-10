#ifndef QTIMEZONESELECTCOMBOBOX_H
#define QTIMEZONESELECTCOMBOBOX_H

#include <QComboBox>
#include <QTimeZone>

#include "misc_global.h"

class MISCDLL_EXPORT QTimeZoneSelectComboBox : public QComboBox
{
public:
	QTimeZoneSelectComboBox(QWidget* parent);

	QTimeZone timeZone() const;
	void setTimeZone(const QTimeZone& timeZone);

private:
	void setupTimeZones();

	std::vector<QTimeZone> m_timeZones;
};

#endif // QTIMEZONESELECTCOMBOBOX_H
