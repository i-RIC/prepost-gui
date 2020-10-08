#include "geodatapointmaprealtextimporter_lineparser.h"

namespace {

enum class Status
{
	OutsideValue,
	InsideValue,
	InsideValueWithQuote,
	InsideValueAfterEscape,
	AfterValueWithQuote
};

double readValueFromColumns(const QStringList& strs, int col, const QString& vname, bool* ok, QString* error)
{
	double ret = 0;
	if (strs.size() < col) {
		*ok = false;
		*error = GeoDataPointmapRealTextImporter::tr("Not enough values to read \"%1\". \"%1\" column is %2 but %3 values input.").arg(vname).arg(col + 1).arg(strs.size());
		return 0;
	} else {
		ret = strs.at(col).toDouble(ok);
		if (! *ok) {
			*error = GeoDataPointmapRealTextImporter::tr("Error occured while reading \"%1\" value. \"%1\" value should be real number, but \"%2\" specified.").arg(vname).arg(strs.at(col));
			return 0;
		}
	}
	*ok = true;
	return ret;
}

void pushBufferToValues(QStringList* values, QString* buffer)
{
	values->append(buffer->trimmed());
	buffer->clear();
}

} // namespace

GeoDataPointmapRealTextImporter::LineParser::LineParser()
{}

void GeoDataPointmapRealTextImporter::LineParser::setTextCodec(QTextCodec* codec)
{
	m_textCodec = codec;
}

void GeoDataPointmapRealTextImporter::LineParser::setDelimiters(std::vector<QChar>& delimiters)
{
	m_delimiters = delimiters;
}

void GeoDataPointmapRealTextImporter::LineParser::setQuoteChars(std::vector<QChar>& quotes)
{
	m_quoteChars = quotes;
}

void GeoDataPointmapRealTextImporter::LineParser::setEscapeChar(const QChar escape)
{
	m_escapeChar = escape;
}

void GeoDataPointmapRealTextImporter::LineParser::setHeaderLines(int lines)
{
	m_headerLines = lines;
}

void GeoDataPointmapRealTextImporter::LineParser::setIsFirstLineFieldNames(bool isFieldNames)
{
	m_isFirstLineFieldNames = isFieldNames;
}

void GeoDataPointmapRealTextImporter::LineParser::setXColumn(int xcol)
{
	m_xColumn = xcol;
}

void GeoDataPointmapRealTextImporter::LineParser::setYColumn(int ycol)
{
	m_yColumn = ycol;
}

void GeoDataPointmapRealTextImporter::LineParser::setValueColumn(int vcol)
{
	m_valueColumn = vcol;
}

void GeoDataPointmapRealTextImporter::LineParser::setSkipRate(int rate)
{
	m_skipRate = rate;
}

QTextCodec* GeoDataPointmapRealTextImporter::LineParser::textCodec() const
{
	return m_textCodec;
}

int GeoDataPointmapRealTextImporter::LineParser::headerLines() const
{
	return m_headerLines;
}

int GeoDataPointmapRealTextImporter::LineParser::skipRate() const
{
	return m_skipRate;
}

QStringList GeoDataPointmapRealTextImporter::LineParser::parseToStrs(const QString& line, bool* ok, QString* error)
{
	Status status = Status::OutsideValue;
	int pos = 0;

	QStringList ret;
	QString buffer;
	QChar quoteChar;

	*ok = true;
	*error = "";

	while (pos < line.length()) {
		QChar c = line.at(pos);
		switch (status) {
		case Status::OutsideValue:
			if (isDelimiter(c)) {
				pushBufferToValues(&ret, &buffer);
			} else if (isQuote(c)){
				status = Status::InsideValueWithQuote;
				quoteChar = c;
			} else {
				status = Status::InsideValue;
				buffer.append(c);
			}
			break;
		case Status::InsideValue:
			if (isDelimiter(c)) {
				pushBufferToValues(&ret, &buffer);
				status = Status::OutsideValue;
			} else {
				buffer.append(c);
			}
			break;
		case Status::InsideValueWithQuote:
			if (isQuote(c)){
				pushBufferToValues(&ret, &buffer);
				status = Status::AfterValueWithQuote;
			} else if (c == m_escapeChar){
				status = Status::InsideValueAfterEscape;
			} else {
				buffer.append(c);
			}
			break;
		case Status::InsideValueAfterEscape:
			if (c == m_escapeChar) {
				buffer.append(m_escapeChar);
			} else if (c == quoteChar) {
				buffer.append(quoteChar);
			} else {
				*ok = false;
				*error = GeoDataPointmapRealTextImporter::tr("At column %1, %2 or %3 expected, but %4 specified.").arg(pos + 1).arg(m_escapeChar).arg(quoteChar).arg(c);
				return ret;
			}
			break;
		case Status::AfterValueWithQuote:
			if (isDelimiter(c)){
				status = Status::OutsideValue;
			} else {
				*error = GeoDataPointmapRealTextImporter::tr("At column %1, delimiter expected, but %2 specified.").arg(pos + 1).arg(c);
			}
			break;
		}
		++ pos;
	}
	QString lastVal = buffer.trimmed();
	if (lastVal.size() > 0) {
		ret.push_back(lastVal);
	}

	return ret;
}

GeoDataPointmapRealTextImporter::Values GeoDataPointmapRealTextImporter::LineParser::parseToValues(const QString& line, bool* ok, QString* error)
{
	Values ret;
	QStringList strs = parseToStrs(line, ok, error);
	if (! *ok) {return ret;}

	ret.x = readValueFromColumns(strs, m_xColumn, tr("x"), ok, error);
	if (! *ok) {return ret;}

	ret.y = readValueFromColumns(strs, m_yColumn, tr("y"), ok, error);
	if (! *ok) {return ret;}

	ret.value = readValueFromColumns(strs, m_valueColumn, tr("value"), ok, error);
	if (! *ok) {return ret;}

	return ret;
}
