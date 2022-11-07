#include "Data_Block.h"


Archive::Block::Column::Bit::operator bool() const
{
	uint16 mask = 1 << m_requested_bit_index;
	return (*m_data & mask) > 0;
}

void Archive::Block::Column::Bit::operator=(bool _value)
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





Archive::Block::Column::Bit Archive::Block::Column::operator[](unsigned int _index)
{
	if(_index > 3)
		return Bit(nullptr, 0);

	return Bit(m_data, (m_requested_column_index * 4) + _index);
}





Archive::Block::Column Archive::Block::operator[](unsigned int _index)
{
	if(_index > 3)
		return Column(nullptr, 0);

	return Column(m_data, _index);
}





Archive::Block Archive::operator[](unsigned int _index)
{
	if(_index >= m_blocks_amount)
		return Block(nullptr);

	return Block(&m_blocks[_index]);
}

bool Archive::get_raw_data_bit(const unsigned char *_raw_data, unsigned int _bit_index) const
{
	return (_raw_data[_bit_index / 8] & (1 << (_bit_index % 8))) > 0;
}

unsigned int Archive::block_index_from_raw(unsigned int _index) const
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

void Archive::setup_block(unsigned int _index)
{
	Block block = (*this)[_index];

	unsigned int counter = 0;
	for(unsigned int i=0; i<4; ++i)
	{
		counter += block[1][i];
		counter += block[3][i];
	}
	block[1][0] = counter % 2;

	counter = 0;
	for(unsigned int i=0; i<4; ++i)
	{
		counter += block[2][i];
		counter += block[3][i];
	}
	block[2][0] = counter % 2;

	counter = 0;
	for(unsigned int i=0; i<4; ++i)
	{
		counter += block[i][1];
		counter += block[i][3];
	}
	block[0][1] = counter % 2;

	counter = 0;
	for(unsigned int i=0; i<4; ++i)
	{
		counter += block[i][2];
		counter += block[i][3];
	}
	block[0][2] = counter % 2;



	counter = 0;
	for(unsigned int i=0; i<4; ++i)
		for(unsigned int j=0; j<4; ++j)
			counter += block[i][j];

	block[0][0] = counter % 2;
}





Archive::Archive(const unsigned char* _raw_data, unsigned int _size)
{
	m_blocks_amount = ((_size * 8) / 11) + ((_size * 8) % 11 > 0);
	m_blocks = new uint16[m_blocks_amount];
	for(unsigned int i=0; i<m_blocks_amount; ++i)
		m_blocks[i] = 0;

	for(unsigned int i=0; i<_size * 8; ++i)
	{
		if(get_raw_data_bit(_raw_data, i) == false)
			continue;

		unsigned int block_index = i / 11;

		uint16 mask = 1 << block_index_from_raw(i % 11);
		m_blocks[block_index] += mask;
	}

	for(unsigned int i=0; i<m_blocks_amount; ++i)
		setup_block(i);
}































































