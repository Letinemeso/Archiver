#include "HCoder.h"


HCoder::Block::Column::Bit::operator bool() const
{
	uint16 mask = 1 << m_requested_bit_index;
	return (*m_data & mask) > 0;
}

void HCoder::Block::Column::Bit::operator=(bool _value)
{
	bool current_value = *this;

	if(current_value && !_value)
	{
		uint16 mask = 1 << m_requested_bit_index;
		*m_data -= mask;
	}
	else if(!current_value && _value)
	{
		uint16 mask = 1 << m_requested_bit_index;
		*m_data += mask;
	}
}





HCoder::Block::Column::Bit HCoder::Block::Column::operator[](unsigned int _index)
{
	if(_index > 3)
		return Bit(nullptr, 0);

	return Bit(m_data, (m_requested_column_index * 4) + _index);
}





HCoder::Block::Column HCoder::Block::operator[](unsigned int _index)
{
	if(_index > 3)
		return Column(nullptr, 0);

	return Column(m_data, _index);
}





HCoder::Block HCoder::block(unsigned int _index)
{
	if(_index >= m_blocks_amount)
		return Block(nullptr);

	return Block(&m_blocks[_index]);
}

bool HCoder::get_raw_data_bit(const char *_raw_data, unsigned int _bit_index) const
{
	return (_raw_data[_bit_index / 8] & (1 << (_bit_index % 8))) > 0;
}

unsigned int HCoder::block_index_from_raw(unsigned int _index) const
{
	switch (_index) {
	case 0:
		return 3;
	case 1:
		return 5;
	case 2:
		return 6;
	case 3:
		return 7;
	case 4:
		return 9;
	case 5:
		return 10;
	case 6:
		return 11;
	case 7:
		return 12;
	case 8:
		return 13;
	case 9:
		return 14;
	case 10:
		return 15;
	default:
		return 0;
	}
}


bool HCoder::try_fix_block(uint16& _block)
{
	uint16 bl = _block;

	unsigned int ones_count = 0;
	unsigned int broken_bit_index = 0;

	for(unsigned int i=0; i<16; ++i)
	{
		if((bl & (1 << i)) > 0)
		{
			broken_bit_index ^= i;
			++ones_count;
		}
	}

	if(broken_bit_index == 0 && ones_count % 2 == 0)
		return true;

	if(ones_count % 2 == 0)
		return false;

	uint16 mask = 1 << broken_bit_index;

	bool broken_bit = (bl & mask) > 0;

	if(broken_bit == true)
	{
		bl -= mask;
		--ones_count;
	}
	else
	{
		bl += mask;
		++ones_count;
	}

	broken_bit_index = 0;
	for(unsigned int i=0; i<16; ++i)
		if((bl & (1 << i)) > 0)
			broken_bit_index ^= i;

	if(broken_bit_index != 0)
		return false;

	_block = bl;
	return true;
}

void HCoder::setup_block(unsigned int _index)
{
	Block bl = block(_index);

	unsigned int counter = 0;
	for(unsigned int i=0; i<4; ++i)
	{
		counter += bl[1][i];
		counter += bl[3][i];
	}
	bl[1][0] = counter % 2;

	counter = 0;
	for(unsigned int i=0; i<4; ++i)
	{
		counter += bl[2][i];
		counter += bl[3][i];
	}
	bl[2][0] = counter % 2;

	counter = 0;
	for(unsigned int i=0; i<4; ++i)
	{
		counter += bl[i][1];
		counter += bl[i][3];
	}
	bl[0][1] = counter % 2;

	counter = 0;
	for(unsigned int i=0; i<4; ++i)
	{
		counter += bl[i][2];
		counter += bl[i][3];
	}
	bl[0][2] = counter % 2;



	counter = 0;
	for(unsigned int i=0; i<4; ++i)
		for(unsigned int j=0; j<4; ++j)
			counter += bl[i][j];

	bl[0][0] = counter % 2;
}





HCoder::HCoder()
{

}

HCoder::HCoder(const HCoder& _other)
{
	m_raw_data = _other.m_raw_data;
	m_encoded_data = _other.m_encoded_data;
	m_blocks_amount = _other.m_blocks_amount;
	m_blocks = new uint16[m_blocks_amount];
	for(unsigned int i=0; i<m_blocks_amount; ++i)
		m_blocks[i] = _other.m_blocks[i];
}

HCoder::HCoder(HCoder&& _other)
{
	m_raw_data = (std::string&&)(_other.m_raw_data);
	m_encoded_data = (std::string&&)(_other.m_encoded_data);
	m_blocks_amount = _other.m_blocks_amount;
	_other.m_blocks_amount = 0;
	m_blocks = _other.m_blocks;
	_other.m_blocks = nullptr;
}

HCoder::~HCoder()
{
	delete[] m_blocks;
}


void HCoder::clear()
{
	m_raw_data.clear();
	m_encoded_data.clear();
	delete[] m_blocks;
	m_blocks = nullptr;
	m_blocks_amount = 0;
}


void HCoder::encode(const std::string &_data)
{
	clear();

	m_raw_data = _data;

	m_blocks_amount = ((_data.size() * 8) / 11) + ((_data.size() * 8) % 11 > 0);
	m_blocks = new uint16[m_blocks_amount];
	for(unsigned int i=0; i<m_blocks_amount; ++i)
		m_blocks[i] = 0;

	for(unsigned int i=0; i<_data.size() * 8; ++i)
	{
		if(get_raw_data_bit(_data.c_str(), i) == false)
			continue;

		unsigned int block_index = i / 11;

		uint16 mask = 1 << block_index_from_raw(i % 11);
		m_blocks[block_index] += mask;
	}

	for(unsigned int i=0; i<m_blocks_amount; ++i)
		setup_block(i);


	unsigned int packed_chars_amount = m_blocks_amount * 2;
	m_encoded_data.resize(packed_chars_amount);

	for(unsigned int i=0; i<m_blocks_amount; ++i)
	{
		m_encoded_data[i * 2] = ((char*)(&m_blocks[i]))[0];
		m_encoded_data[i * 2 + 1] = ((char*)(&m_blocks[i]))[1];
	}
}

void HCoder::decode(const std::string& _data)
{
	clear();

	m_encoded_data = _data;

	m_blocks_amount = _data.size() / 2;
	m_blocks = new uint16[m_blocks_amount];
	for(unsigned int i=0; i<m_blocks_amount; ++i)
		m_blocks[i] = *((uint16*)(&_data.c_str()[i * 2]));

	for(unsigned int i=0; i<m_blocks_amount; ++i)
	{
		if(!try_fix_block(m_blocks[i]))
		{
			clear();
			return;
		}
	}

	unsigned int chars_amount = (m_blocks_amount * 11) / 8;

	m_raw_data.resize(chars_amount);

	for(unsigned int i=0; i<chars_amount * 8; ++i)
	{
		bool value_in_block = m_blocks[i  / 11] & (1 << block_index_from_raw(i % 11));
		if(!value_in_block)
			continue;
		char mask = 1 << (i % 8);
		m_raw_data[i / 8] += mask;
	}
}



const std::string& HCoder::encoded_data() const
{
	return m_encoded_data;
}

const std::string& HCoder::decoded_data() const
{
	return m_raw_data;
}

std::string& HCoder::encoded_data()
{
	return m_encoded_data;
}

std::string& HCoder::decoded_data()
{
	return m_raw_data;
}


bool HCoder::is_ok() const
{
	return m_blocks != nullptr;
}

































































