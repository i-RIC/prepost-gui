#include "inputconditionwidgetfunctionaldownloaddialog.h"
#include "ui_inputconditionwidgetfunctionaldownloaddialog.h"

#include <QDomDocument>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#include <guibase/overridecursorchanger.h>
#include <misc/qttool.h>

namespace {

bool validSiteID(const QString& site) {
	/*
	The site number may be optionally prefixed by the agency code followed by a
	colon, which ensures the site is unique. Examples: ?site=06306300 or
	?sites=USGS:06306300. There is no default if this argument is used. Site
	numbers range from 8 to 15 characters.
	*/
	QRegularExpression re("^(?:\\w+:)?(\\d{8,15})$");
	QRegularExpressionMatch match = re.match(site);
	return (match.hasMatch() && match.lastCapturedIndex() == 1);
}

} // namespace

InputConditionWidgetFunctionalDownloadDialog::InputConditionWidgetFunctionalDownloadDialog(QWidget *parent) :
	QDialog(parent),
	m_manager(QtTool::networkAccessManager()),
	m_needsUpdate(true),
	ui(new Ui::InputConditionWidgetFunctionalDownloadDialog)
{
	ui->setupUi(this);

	connect(ui->siteLineEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
	connect(ui->startDateTimeEdit, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(startDateChanged(QDateTime)));
	connect(ui->endDateTimeEdit, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(endDateChanged(QDateTime)));
	connect(ui->testButton, SIGNAL(clicked()), this, SLOT(test()));
}

InputConditionWidgetFunctionalDownloadDialog::~InputConditionWidgetFunctionalDownloadDialog()
{
	delete ui;
}


void InputConditionWidgetFunctionalDownloadDialog::startDateChanged(const QDateTime &)
{
	m_needsUpdate = true;
}

void InputConditionWidgetFunctionalDownloadDialog::endDateChanged(const QDateTime &)
{
	m_needsUpdate = true;
}

void InputConditionWidgetFunctionalDownloadDialog::textChanged(const QString &)
{
	m_needsUpdate = true;
}

QString InputConditionWidgetFunctionalDownloadDialog::generateURL()
{
	QDateTime start = ui->startDateTimeEdit->dateTime();
	QDateTime end = ui->endDateTimeEdit->dateTime();

	// build codes
	QString nwisCodes = m_container.code(0).c_str();
	for (int i = 1; i < m_container.codeCount(); ++i) {
		nwisCodes += "," + QString(m_container.code(i).c_str());
	}

	// replace
	QString url = m_container.wml2URL();
	url.replace("{siteID}", m_container.site());
	url.replace("{startDate}", m_container.startDate());
	url.replace("{endDate}", m_container.endDate());
	url.replace("{nwisCodes}", nwisCodes);

	return url;
}

bool InputConditionWidgetFunctionalDownloadDialog::validate()
{
	QString site = ui->siteLineEdit->text().trimmed();
	if (site.isEmpty()) {
		ui->siteLineEdit->setFocus();
		QMessageBox::critical(this, tr("Error"), tr("Please enter a site number."));
		return false;
	}

	if (! validSiteID(site)) {
		ui->siteLineEdit->setFocus();
		QMessageBox::critical(this, tr("Error"), tr("Invalid site number."));
		return false;
	}

	QDateTime start = ui->startDateTimeEdit->dateTime();
	QDateTime end = ui->endDateTimeEdit->dateTime();

	if (start > end) {
		QMessageBox::critical(this, tr("Error"), tr("Start date not prior to end date."));
	}

	QString startDate = start.toString(Qt::ISODate);
	QString endDate = end.toString(Qt::ISODate);

	m_container.setSite(site);
	m_container.setStartDate(startDate);
	m_container.setEndDate(endDate);

	return true;
}

void InputConditionWidgetFunctionalDownloadDialog::accept()
{
	bool ok = validate();
	if (!ok) { return; }

	OverrideCursorChanger cursorChanger(Qt::WaitCursor);
	if (m_needsUpdate) {
		if (get(generateURL())) {
			ok = parse(m_wml);
			if (!ok) { return; }
		}
	}

	QDialog::accept();
}

void InputConditionWidgetFunctionalDownloadDialog::setupData()
{
	ui->siteLineEdit->setText(m_container.site());
	if (m_container.startDate().isEmpty() || m_container.startDate().isEmpty()) {
		QDateTime now = QDateTime::currentDateTime();
		ui->startDateTimeEdit->setDateTime(now);
		ui->endDateTimeEdit->setDateTime(now);
		return;
	}
	ui->startDateTimeEdit->setDateTime(QDateTime::fromString(m_container.startDate(), Qt::ISODate));
	ui->endDateTimeEdit->setDateTime(QDateTime::fromString(m_container.endDate(), Qt::ISODate));
}

const InputConditionContainerFunctional& InputConditionWidgetFunctionalDownloadDialog::container() const
{
	return m_container;
}

void InputConditionWidgetFunctionalDownloadDialog::setData(const InputConditionContainerFunctional& c)
{
	m_container = c;
	setupData();
}

void InputConditionWidgetFunctionalDownloadDialog::test()
{
	OverrideCursorChanger cursorChanger(Qt::WaitCursor);

	if (! validate()) return;

	QString urlstr = generateURL();

	ui->urlEdit->setText(urlstr);
	ui->urlEdit->selectAll();
	qApp->processEvents();

	if (get(urlstr)) {
		parse(m_wml);
	}

	m_needsUpdate = false;
}

bool InputConditionWidgetFunctionalDownloadDialog::get(const QString& urlstr)
{
	QUrl url(urlstr);
	QNetworkRequest request;
	QNetworkReply *reply;

	QString errors;

	while (url.toString().size() != 0) {
		// make request
		request.setUrl(url);
		reply = m_manager->get(request);

		// wait until finished
		QEventLoop loop;
		connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
		loop.exec();

		// HTTP status code
		// see https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
		// see https://waterservices.usgs.gov/rest/IV-Service.html#Error
		int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		QString reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
		qDebug() << status << reason.toStdString().c_str();
		errors.append(QString::number(status) + " " + reason);

		// check for errors
		if (reply->error() != 0) {
			Q_ASSERT(!reason.isEmpty());
			break;
		}

		// get redirect url (if any)
		url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	}

	m_wml.clear();
	m_get_errors.clear();

	if (reply->error() == 0) {
		m_wml = reply->readAll();
	}
	else {
		m_get_errors = errors;
	}

	return (reply->error() == 0);
}

bool InputConditionWidgetFunctionalDownloadDialog::parse(const QString& xml)
{
	QDomDocument doc;
	doc.setContent(xml);
	Q_ASSERT(!doc.isNull());

	std::vector<QString> titles;
	std::vector<QString> units;

	QMap<QString, QMap<QDateTime, QVariant> > parm2map;
	QMap<QDateTime, QString> dateTimes;

	QRegularExpression parmRE("^.*parmCd=(.*)$");

	QDomNodeList list = doc.elementsByTagName("wml2:observationMember");
	if (list.count() == 0) {
		QMessageBox::warning(this, tr("Import failed"), tr("No data to import."));
		return false;
	}

	for (int i = 0; i < list.count(); ++i) {
		QDomElement oMemb = list.at(i).toElement();

		QDomNodeList oProp = oMemb.elementsByTagName("om:observedProperty");
		Q_ASSERT(oProp.size() == 1);

		// property
		QString title;
		if (oProp.at(0).attributes().contains("xlink:title")) {
			title = oProp.at(0).attributes().namedItem("xlink:title").nodeValue();
			titles.push_back(title);
		}

		// parmCd (code)
		QString parmCd;
		if (oProp.at(0).attributes().contains("xlink:href")) {
			QString href = oProp.at(0).attributes().namedItem("xlink:href").nodeValue();
			QRegularExpressionMatch match = parmRE.match(href);
			Q_ASSERT(match.hasMatch() && match.lastCapturedIndex() == 1);
			parmCd = match.captured(1);
		}

		// units
		QDomNodeList uom = oMemb.elementsByTagName("wml2:uom");
		if (uom.at(0).attributes().contains("xlink:title")) {
			QString unit = uom.at(0).attributes().namedItem("xlink:title").nodeValue();
			units.push_back(unit);
		}

		// result
		QDomElement result = oMemb.elementsByTagName("om:result").at(0).toElement();

		// points
		QDomNodeList pts = result.elementsByTagName("wml2:point");
		QMap<QDateTime, QVariant> &time2var = parm2map[parmCd];
		for (int p = 0; p < pts.count(); ++p) {
			// time
			QString timestr = pts.at(p).toElement().elementsByTagName("wml2:time").at(0).childNodes().at(0).toText().data();
			QDateTime dateTime = QDateTime::fromString(timestr, Qt::ISODate);

			// value
			QString valuestr = pts.at(p).toElement().elementsByTagName("wml2:value").at(0).childNodes().at(0).toText().data();
			double value = valuestr.toDouble();

			dateTimes[dateTime] = timestr;
			time2var[dateTime] = QVariant(value);
		}
	}

	// reset
	m_container.removeAllValues();

	// times
	QMap<QDateTime, QString>::const_iterator ci = dateTimes.constBegin();
	const QDateTime& first = ci.key();
	for (; ci != dateTimes.constEnd(); ++ci) {
		qint64 seconds = first.msecsTo(ci.key()) / 1000;
		m_container.param().push_back(seconds);
	}

	// values
	for (int p = 0; p < m_container.valueCount(); ++p) {
		QString code = m_container.code(p).c_str();
		QMap< QDateTime, QVariant > &time2var = parm2map[code];
		for (ci = dateTimes.constBegin(); ci != dateTimes.constEnd(); ++ci) {
			m_container.value(p).push_back(time2var[ci.key()].toDouble());
		}
	}

	// convert units
	for (auto &p : m_container.param()) {
		p *= m_container.paramFactor();
	}
	for (int i = 0; i < m_container.valueCount(); ++i) {
		for (auto &p : m_container.value(i)) {
			p *= m_container.valueFactor(i);
		}
	}

	// write results
	QString lines;
	std::vector<double>::iterator it = m_container.param().begin();
	for (; it != m_container.param().end(); ++it) {
		if (m_container.paramFactor() < 1.) {
			lines += QString::number(*it, 'g', 3);

		} else {
			lines += QString::number((int)(*it + .5));
		}
		for (int c = 0; c < m_container.valueCount(); ++c) {
			lines += "\t" + QString::number(m_container.value(c).at(std::distance(m_container.param().begin(), it)), 'g', 3);
		}
		lines += "\n";
	}
	ui->plainTextEdit->setPlainText(lines);

	return true;
}
