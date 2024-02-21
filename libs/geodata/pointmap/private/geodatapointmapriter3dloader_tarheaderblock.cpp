#include "geodatapointmapriter3dloader_tarheaderblock.h"

#define NOMINMAX
#include <Poco/Types.h>
#include <Poco/BinaryReader.h>
#include <Poco/BinaryWriter.h>

#include <iomanip>
#include <sstream>

namespace {
const static int BLOCK_SIZE = 512;
const static int END_OF_ARCHIVE_SIZE = 1024;

const static int NAME_SIZE     = 100;
const static int MODE_SIZE     =   8;
const static int UID_SIZE      =   8;
const static int GID_SIZE      =   8;
const static int SIZE_SIZE     =  12;
const static int MTIME_SIZE    =  12;
const static int CHKSUM_SIZE   =   8;
const static int TYPEFLAG_SIZE =   1;
const static int LINKNAME_SIZE = 100;
const static int MAGIC_SIZE    =   6;
const static int VERSION_SIZE  =   2;
const static int UNAME_SIZE    =  32;
const static int GNAME_SIZE    =  32;
const static int DEVMAJOR_SIZE =   8;
const static int DEVMINOR_SIZE =   8;
const static int PREFIX_SIZE   = 155;
const static int OFFSET_SIZE   =  12;

unsigned int convertString2UInt(const std::string& str)
{
	return std::stoi(str, nullptr, 8);
}

std::string convertUInt2String(int size, unsigned int num, bool trim)
{
	if (trim) {--size;}

	std::ostringstream sout;
	sout << std::setfill('0') << std::setw(size) << std::oct << num;

	std::string str = sout.str();

	return str;
}

void readRawWithSize(Poco::BinaryReader* reader, int size, std::string* str)
{
	reader->readRaw(size, *str);
}

void readRawWithSize(Poco::BinaryReader* reader, int size, unsigned int* num)
{
	std::string str;
	reader->readRaw(size, str);
	*num = convertString2UInt(str);
}

void writeRawWithSize(Poco::BinaryWriter* writer, int size, const std::string& str)
{
	writer->writeRaw(str);

	if (str.length() < size) {
		std::string offset;
		offset.resize(size - str.length(), '\0');
		writer->writeRaw(offset);
	}
}

void writeRawWithSize(Poco::BinaryWriter* writer, int size, unsigned int num)
{
	writeRawWithSize(writer, size, convertUInt2String(size, num, true));
}

unsigned int calculateChksum(const std::string& str)
{
	unsigned int ret = 0;
	for (size_t i = 0; i < str.length(); ++i) {
		ret += str.at(i);
	}

	return ret;
}
}

GeoDataPointmapRiter3dLoader::TarHeaderBlock::TarHeaderBlock() :
	m_name ("NONAME"),
	m_mode {493}, // 755
	m_uid ("0601751"),
	m_gid ("0601001"),
	m_size {0},
	// m_mtime {static_cast<unsigned int> (time(NULL))},
	m_mtime {0},
	m_chksum {0},
	m_typeflag ("0"),
	m_linkname {},
	m_magic ("ustar "),
	m_version (" "),
	m_uname ("tineditor"),
	m_gname ("tineditor"),
	m_devmajor {},
	m_devminor {},
	m_prefix {}
{}

GeoDataPointmapRiter3dLoader::TarHeaderBlock::~TarHeaderBlock()
{}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::read(Poco::BinaryReader* reader)
{
	std::string offset;

	readRawWithSize(reader, NAME_SIZE,     &m_name);
	readRawWithSize(reader, MODE_SIZE,     &m_mode);
	readRawWithSize(reader, UID_SIZE,      &m_uid);
	readRawWithSize(reader, GID_SIZE,      &m_gid);
	readRawWithSize(reader, SIZE_SIZE,     &m_size);
	readRawWithSize(reader, MTIME_SIZE,    &m_mtime);
	readRawWithSize(reader, CHKSUM_SIZE,   &m_chksum);
	readRawWithSize(reader, TYPEFLAG_SIZE, &m_typeflag);
	readRawWithSize(reader, LINKNAME_SIZE, &m_linkname);
	readRawWithSize(reader, MAGIC_SIZE,    &m_magic);
	readRawWithSize(reader, VERSION_SIZE,  &m_version);
	readRawWithSize(reader, UNAME_SIZE,    &m_uname);
	readRawWithSize(reader, GNAME_SIZE,    &m_gname);
	readRawWithSize(reader, DEVMAJOR_SIZE, &m_devmajor);
	readRawWithSize(reader, DEVMINOR_SIZE, &m_devminor);
	readRawWithSize(reader, PREFIX_SIZE,   &m_prefix);
	readRawWithSize(reader, OFFSET_SIZE,   &offset);
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::write(Poco::BinaryWriter* writer)
{
	updateChksum();

	writeRawWithSize(writer, NAME_SIZE,     m_name);
	writeRawWithSize(writer, MODE_SIZE,     m_mode);
	writeRawWithSize(writer, UID_SIZE,      m_uid);
	writeRawWithSize(writer, GID_SIZE,      m_gid);
	writeRawWithSize(writer, SIZE_SIZE,     m_size);
	writeRawWithSize(writer, MTIME_SIZE,    m_mtime);
	writeRawWithSize(writer, CHKSUM_SIZE,   m_chksum);
	writeRawWithSize(writer, TYPEFLAG_SIZE, m_typeflag);
	writeRawWithSize(writer, LINKNAME_SIZE, m_linkname);
	writeRawWithSize(writer, MAGIC_SIZE,    m_magic);
	writeRawWithSize(writer, VERSION_SIZE,  m_version);
	writeRawWithSize(writer, UNAME_SIZE,    m_uname);
	writeRawWithSize(writer, GNAME_SIZE,    m_gname);
	writeRawWithSize(writer, DEVMAJOR_SIZE, m_devmajor);
	writeRawWithSize(writer, DEVMINOR_SIZE, m_devminor);
	writeRawWithSize(writer, PREFIX_SIZE,   m_prefix);

	writeOffset(writer, 12);
}

std::string GeoDataPointmapRiter3dLoader::TarHeaderBlock::name()
{
	return m_name;
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::setName(std::string name)
{
	m_name = name;
}

unsigned int GeoDataPointmapRiter3dLoader::TarHeaderBlock::mode()
{
	return m_mode;
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::setMode(unsigned int mode)
{
	m_mode = mode;
}

std::string GeoDataPointmapRiter3dLoader::TarHeaderBlock::uid()
{
	return m_uid;
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::setUid(std::string uid)
{
	m_uid = uid;
}

std::string GeoDataPointmapRiter3dLoader::TarHeaderBlock::gid()
{
	return m_gid;
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::setGid(std::string gid)
{
	m_gid = gid;
}

unsigned int GeoDataPointmapRiter3dLoader::TarHeaderBlock::size()
{
	return m_size;
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::setSize(unsigned int size)
{
	m_size = size;
}

unsigned int GeoDataPointmapRiter3dLoader::TarHeaderBlock::mtime()
{
	return m_mtime;
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::setMtime(unsigned int mtime)
{
	m_mtime = mtime;
}

unsigned int GeoDataPointmapRiter3dLoader::TarHeaderBlock::chksum()
{
	return m_chksum;
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::setChksum(unsigned int chksum)
{
	m_chksum = chksum;
}

std::string GeoDataPointmapRiter3dLoader::TarHeaderBlock::typeflag()
{
	return m_typeflag;
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::setTypeflag(std::string typeflag)
{
	m_typeflag = typeflag;
}

std::string GeoDataPointmapRiter3dLoader::TarHeaderBlock::linkname()
{
	return m_linkname;
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::setLinkname(std::string linkname)
{
	m_linkname = linkname;
}

std::string GeoDataPointmapRiter3dLoader::TarHeaderBlock::magic()
{
	return m_magic;
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::setMagic(std::string magic)
{
	m_magic = magic;
}

std::string GeoDataPointmapRiter3dLoader::TarHeaderBlock::version()
{
	return m_version;
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::setVersion(std::string version)
{
	m_version = version;
}

std::string GeoDataPointmapRiter3dLoader::TarHeaderBlock::uname()
{
	return m_uname;
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::setUname(std::string uname)
{
	m_uname = uname;
}

std::string GeoDataPointmapRiter3dLoader::TarHeaderBlock::gname()
{
	return m_gname;
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::setGname(std::string gname)
{
	m_gname = gname;
}

std::string GeoDataPointmapRiter3dLoader::TarHeaderBlock::devmajor()
{
	return m_devmajor;
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::setDevmajor(std::string devmajor)
{
	m_devmajor = devmajor;
}

std::string GeoDataPointmapRiter3dLoader::TarHeaderBlock::devminor()
{
	return m_devminor;
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::setDevminor(std::string devminor)
{
	m_devminor = devminor;
}

std::string GeoDataPointmapRiter3dLoader::TarHeaderBlock::prefix()
{
	return m_prefix;
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::setPrefix(std::string prefix)
{
	m_prefix = prefix;
}

int GeoDataPointmapRiter3dLoader::TarHeaderBlock::offset()
{
	return (512 - m_size % 512) % 512; // 0 <= return value < 512.
}

int GeoDataPointmapRiter3dLoader::TarHeaderBlock::contentSize()
{
	return offset() + m_size;
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::writeOffset(Poco::BinaryWriter* writer, int offset)
{
	std::string str {};
	writeRawWithSize(writer, offset, str);
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::writeEndOfArchive(Poco::BinaryWriter* writer)
{
	writeOffset(writer, END_OF_ARCHIVE_SIZE);
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::skipEndOfArchive(Poco::BinaryReader* reader)
{
	std::string str;
	reader->readRaw(END_OF_ARCHIVE_SIZE, str);
}

int GeoDataPointmapRiter3dLoader::TarHeaderBlock::endOfArchiveSize()
{
	return END_OF_ARCHIVE_SIZE;
}

int GeoDataPointmapRiter3dLoader::TarHeaderBlock::headerSize()
{
	return BLOCK_SIZE;
}

void GeoDataPointmapRiter3dLoader::TarHeaderBlock::updateChksum()
{
	m_chksum = 0;
	m_chksum += calculateChksum(m_name);
	m_chksum += calculateChksum(convertUInt2String(MODE_SIZE, m_mode, true));
	m_chksum += calculateChksum(m_uid);
	m_chksum += calculateChksum(m_gid);
	m_chksum += calculateChksum(convertUInt2String(SIZE_SIZE, m_size, true));
	m_chksum += calculateChksum(convertUInt2String(MTIME_SIZE, m_mtime, true));
	m_chksum += calculateChksum(std::string(CHKSUM_SIZE, ' ')); // m_chksum
	m_chksum += calculateChksum(m_typeflag);
	m_chksum += calculateChksum(m_linkname);
	m_chksum += calculateChksum(m_magic);
	m_chksum += calculateChksum(m_version);
	m_chksum += calculateChksum(m_uname);
	m_chksum += calculateChksum(m_gname);
	m_chksum += calculateChksum(m_devmajor);
	m_chksum += calculateChksum(m_devminor);
	m_chksum += calculateChksum(m_prefix);
}
