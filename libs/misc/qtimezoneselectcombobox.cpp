#include "qtimezoneselectcombobox.h"

#include <QRegExp>

QTimeZoneSelectComboBox::QTimeZoneSelectComboBox(QWidget* parent) :
	QComboBox {parent}
{
	setupTimeZones();
}

QTimeZone QTimeZoneSelectComboBox::timeZone() const
{
	return m_timeZones.at(currentIndex());
}

void QTimeZoneSelectComboBox::setTimeZone(const QTimeZone& timeZone)
{
	for (int i = 0; i < m_timeZones.size(); ++i) {
		if (m_timeZones.at(i) == timeZone) {
			setCurrentIndex(i);
		}
	}
}

void QTimeZoneSelectComboBox::setupTimeZones()
{
	QRegExp re("UTC(\\+|\\-)[0-9]{2}\\:[0-9{2}]");
	for (const auto& tzId : QTimeZone::availableTimeZoneIds()) {
		QTimeZone tz(tzId);
		QString name = tz.displayName(QTimeZone::StandardTime);
		if (re.indexIn(name) != -1) {
			m_timeZones.push_back(tz);
			addItem(name);
		}
	}
}
