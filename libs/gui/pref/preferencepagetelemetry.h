#ifndef PREFERENCEPAGETELEMETRY_H
#define PREFERENCEPAGETELEMETRY_H

#include "preferencepage.h"

#include <QWidget>

class TelemetryWidget;
class iRICAuthClient;

namespace Ui
{
	class PreferencePageTelemetry;
}

/// Preferences tab that lets the user revisit the telemetry consent choice.
///
/// It embeds the shared TelemetryWidget, the same widget shown by the
/// first-run consent dialog. The auth client is borrowed and may be null.
class PreferencePageTelemetry : public PreferencePage
{
	Q_OBJECT

public:
	explicit PreferencePageTelemetry(iRICAuthClient* client, QWidget* parent = nullptr);
	~PreferencePageTelemetry();

	void update() override;

private:
	TelemetryWidget* m_widget;
	Ui::PreferencePageTelemetry* ui;
};

#endif // PREFERENCEPAGETELEMETRY_H
