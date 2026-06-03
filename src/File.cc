#include "File.hh"

namespace BrrEdit {
	FileWriter::FileWriter(const std::string& path) : m_path(path) {}

	FileWriter::~FileWriter() {
		Flush();
	}

	void FileWriter::F32(float value) {
		uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
		m_buffer.insert(m_buffer.end(), ptr, ptr + sizeof(float));
	}

	void FileWriter::F64(double value) {
		uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
		m_buffer.insert(m_buffer.end(), ptr, ptr + sizeof(double));
	}

	void FileWriter::U32(uint32_t value) {
		uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
		m_buffer.insert(m_buffer.end(), ptr, ptr + sizeof(uint32_t));
	}

	void FileWriter::U64(uint64_t value) {
		uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
		m_buffer.insert(m_buffer.end(), ptr, ptr + sizeof(uint64_t));
	}

	void FileWriter::I32(int32_t value) {
		uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
		m_buffer.insert(m_buffer.end(), ptr, ptr + sizeof(int32_t));
	}

	void FileWriter::I64(int64_t value) {
		uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
		m_buffer.insert(m_buffer.end(), ptr, ptr + sizeof(int64_t));
	}

	void FileWriter::U8(uint8_t value) {
		m_buffer.push_back(value);
	}

	void FileWriter::Str(const std::string& value) {
		uint32_t len = static_cast<uint32_t>(value.size());
		U32(len);
		m_buffer.insert(m_buffer.end(), value.begin(), value.end());
	}

	void FileWriter::Bytes(const uint8_t* data, size_t size) {
		m_buffer.insert(m_buffer.end(), data, data + size);
	}

	void FileWriter::Flush() {
		std::ofstream file(m_path, std::ios::binary);
		file.write(reinterpret_cast<const char*>(m_buffer.data()), m_buffer.size());
		file.close();
	}

	FileReader::FileReader(const std::string& path) {
		m_file.open(path, std::ios::binary);
	}

	FileReader::~FileReader() {
		if (m_file.is_open()) {
			m_file.close();
		}
	}

	float FileReader::F32() {
		float value;
		m_file.read(reinterpret_cast<char*>(&value), sizeof(float));
		return value;
	}

	double FileReader::F64() {
		double value;
		m_file.read(reinterpret_cast<char*>(&value), sizeof(double));
		return value;
	}

	uint32_t FileReader::U32() {
		uint32_t value;
		m_file.read(reinterpret_cast<char*>(&value), sizeof(uint32_t));
		return value;
	}

	uint64_t FileReader::U64() {
		uint64_t value;
		m_file.read(reinterpret_cast<char*>(&value), sizeof(uint64_t));
		return value;
	}

	int32_t FileReader::I32() {
		int32_t value;
		m_file.read(reinterpret_cast<char*>(&value), sizeof(int32_t));
		return value;
	}

	int64_t FileReader::I64() {
		int64_t value;
		m_file.read(reinterpret_cast<char*>(&value), sizeof(int64_t));
		return value;
	}

	uint8_t FileReader::U8() {
		uint8_t value;
		m_file.read(reinterpret_cast<char*>(&value), sizeof(uint8_t));
		return value;
	}

	std::string FileReader::Str() {
		uint32_t len = U32();
		std::string value(len, '\0');
		m_file.read(value.data(), len);
		return value;
	}

	void FileReader::Bytes(uint8_t* out, size_t size) {
		m_file.read(reinterpret_cast<char*>(out), size);
	}
}
