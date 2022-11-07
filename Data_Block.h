#ifndef DATA_BLOCK_H
#define DATA_BLOCK_H

#include <iostream>


class Archive final
{
private:
	using uint16 = short unsigned int;

public:
	class Block final
	{
	private:
		uint16* m_data = nullptr;

	private:
		class Column final
		{
		private:
			uint16* m_data = nullptr;
			unsigned int m_requested_column_index = 0;

		private:
			class Bit final
			{
			private:
				uint16* m_data = nullptr;
				unsigned int m_requested_bit_index = 0;

			public:
				Bit(uint16* _data, unsigned int _requested_bit_index) : m_data(_data), m_requested_bit_index(_requested_bit_index) { }

			public:
				operator bool() const;
				void operator=(bool _value);

			};

		public:
			Column(uint16* _data, unsigned int _requested_line_index) : m_data(_data), m_requested_column_index(_requested_line_index) { }

		public:
			Bit operator[](unsigned int _index);

		};

	public:
		Block(uint16* _data) : m_data(_data) { }

	public:
		Column operator[](unsigned int _index);

	};

private:
	uint16* m_blocks = nullptr;
	unsigned int m_blocks_amount = 0;

private:
	Block operator[](unsigned int _index);
	bool get_raw_data_bit(const unsigned char* _raw_data, unsigned int _bit_index) const;
	unsigned int block_index_from_raw(unsigned int _index) const;
	void setup_block(unsigned int _index);

public:
	Archive(const unsigned char* _raw_data, unsigned int _size);

	void print_blocks()
	{
		for(unsigned int i=0; i<m_blocks_amount; ++i)
		{
			for(unsigned int x = 0; x < 4; ++x)
			{
				for(unsigned int y = 0; y < 4; ++y)
					std::cout << (*this)[i][x][y] << " ";
				std::cout << "\n";
			}
			std::cout << "\n";
		}
	}

};


#endif // DATA_BLOCK_H
