#ifndef HCODER_H
#define HCODER_H

#include <iostream>


class HCoder final
{
private:
	using uint16 = short unsigned int;

private:
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
	std::string m_raw_data;
	std::string m_encoded_data;

	uint16* m_blocks = nullptr;
	unsigned int m_blocks_amount = 0;

private:
	Block block(unsigned int _index);
	bool get_raw_data_bit(const char* _raw_data, unsigned int _bit_index) const;
	unsigned int block_index_from_raw(unsigned int _index) const;

	bool try_fix_block(uint16& _block);
	void setup_block(unsigned int _index);


public:
	HCoder();
	HCoder(const HCoder& _other);
	HCoder(HCoder&& _other);
	~HCoder();

	void clear();

	void encode(const std::string& _data);
	void decode(const std::string& _data);

public:
	const std::string& encoded_data() const;
	const std::string& decoded_data() const;
	std::string& encoded_data();
	std::string& decoded_data();

	bool is_ok() const;

};


#endif // HCODER_H
