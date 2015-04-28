// speed_calculator.cpp: implementation of the speed_calculator class.
//
//////////////////////////////////////////////////////////////////////

#include "speed_calculator.h"
#include <assert.h>
#include <windows.h>
#include <algorithm>





//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

speed_calculator::speed_calculator(const speed_calculator &calculator)
{
    _samples_ptr    = 0;
    _cycle          = 0;
    _unit           = 0;
    _start_index    = 0;
    _end_index      = 0;

    *this = calculator;
}
speed_calculator &speed_calculator::operator = (const speed_calculator &calculator)
{
	if( &calculator != this )
	{
		if ( _samples_ptr )
        {
            delete [] _samples_ptr;
            _samples_ptr = 0;
        }
		_cycle = calculator._cycle;
		_unit  = calculator._unit;
		_samples_ptr = new unsigned int[calculator._cycle];
        _start_index = calculator._start_index;
		_end_index   = calculator._end_index;
		memcpy(_samples_ptr, calculator._samples_ptr, calculator._cycle*sizeof(unsigned int));
	}

	return *(this);
}

speed_calculator::speed_calculator( unsigned int cycle, unsigned int unit )
{
    _cycle  = cycle;
    _unit   = unit;

    _start_index = GetTickCount()/_unit;
	_end_index = _start_index + 1;

	_samples_ptr = new unsigned int[_cycle];
    memset( _samples_ptr, 0, sizeof(unsigned int)*_cycle );
}

speed_calculator::~speed_calculator()
{
	if ( 0 != _samples_ptr )
	{
		delete [] _samples_ptr;
	}
	_samples_ptr = 0;	
}

void speed_calculator::clean( unsigned int start_index, unsigned int end_index )
{
    if ( start_index >= end_index )
    {
        return;
    }

    unsigned int unit_count = end_index-start_index;
    unit_count = std::min<unsigned int>( unit_count, _cycle );
    unsigned int i = 0;
    for ( i=0; i<unit_count; i++ )
    {
        _samples_ptr[(start_index+i)%_cycle] = 0;
    }
}

void speed_calculator::update( unsigned int tickcount )
{
    unsigned int current_index = tickcount/_unit;
    
    clean( _end_index, current_index + 1 );
    
    _end_index = current_index + 1;
    if ( _start_index < _end_index - _cycle )
    {
        _start_index = _end_index - _cycle;
    }
    
    assert( _end_index-_start_index <= _cycle );
}

void speed_calculator::add_bytes( unsigned int bytes )
{
    unsigned int tickcount = GetTickCount();
    unsigned int current_index = tickcount/_unit;

    update( tickcount );
    
    _samples_ptr[current_index%_cycle] += bytes;

    assert( _end_index-_start_index <= _cycle );
}

unsigned __int64 speed_calculator::speed_value(void)
{
    unsigned int tickcount = GetTickCount();
    
    update( tickcount );

    unsigned __int64 total = 0;

    unsigned int unit_count = _end_index-_start_index;
    assert( unit_count <= _cycle );
    unsigned int i = 0;
    for ( i=0; i<unit_count; i++ )
    {
        total += _samples_ptr[(_start_index+i)%_cycle];
    }

    return (total*1000)/(unit_count*_unit);
}

void speed_calculator::clear()
{
    assert( _samples_ptr && _cycle && _unit );
    _start_index = GetTickCount()/_unit;
    _end_index = _start_index + 1;    
    memset( _samples_ptr, 0, sizeof(unsigned int)*_cycle );
}

