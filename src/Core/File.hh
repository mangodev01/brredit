#pragma once

#include <cstdint>
#include <string>
#include <fstream>
#include <vector>

namespace BrrEdit {
	class FileWriter {
		public:
			explicit FileWriter(const std::string& path);
			~FileWriter();

			void F32(float value);
			void F64(double value);
			void U32(uint32_t value);
			void U64(uint64_t value);
			void I32(int32_t value);
			void I64(int64_t value);
			void U8(uint8_t value);
			void Str(const std::string& value);
			void Bytes(const uint8_t* data, size_t size);
			void Flush();

		private:
			std::string m_path;
			std::vector<uint8_t> m_buffer;
			std::ofstream m_file;
	};

	class FileReader {
		public:
			explicit FileReader(const std::string& path);
			~FileReader();

			float F32();
			double F64();
			uint32_t U32();
			uint64_t U64();
			int32_t I32();
			int64_t I64();
			uint8_t U8();
			std::string Str();
			void Bytes(uint8_t* out, size_t size);

		private:
			std::ifstream m_file;
	};
}
