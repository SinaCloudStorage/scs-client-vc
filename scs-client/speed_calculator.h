// speed_calculator.h: interface for the speed_calculator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPEED_CALCULATOR_H__1620FC17_7B0F_4DA4_8591_DDF4F4A1C141__INCLUDED_)
#define AFX_SPEED_CALCULATOR_H__1620FC17_7B0F_4DA4_8591_DDF4F4A1C141__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
�����ݼ�����Ŀǰ��һ�����⣬GetTickCount()��ϵͳ���е�49.7������㣬�����δ����
*/

class speed_calculator  
{
public:
    //ֻҪ��ָ���Ա���ͱ���Ҫ���忽�����캯��������=
	speed_calculator(const speed_calculator &calculator);
	speed_calculator &operator = (const speed_calculator &calculator);

	// cycle ��ʾ�����ٶ�ʱ���ӵ�ǰʱ����ǰ�ƣ���������������
	// unit ��ʾÿ�������ռ����ݵ�ʱ���ȣ���λ����
	speed_calculator(unsigned int cycle, unsigned int unit = 1);
	virtual ~speed_calculator();

	void add_bytes( unsigned int bytes );
    unsigned __int64 speed_value( void ); // ��õ�ǰ���ٶȣ���λ���ֽ�/��

    void clear(); // ��������ٶȣ����㿪ʼ

protected:
    void update( unsigned int tickcount );
    void clean( unsigned int start_index, unsigned int end_index );

private:
	unsigned int *_samples_ptr;
    unsigned int _start_index;
	unsigned int _end_index;
	unsigned int _cycle;
	unsigned int _unit;
};

#endif // !defined(AFX_SPEED_CALCULATOR_H__1620FC17_7B0F_4DA4_8591_DDF4F4A1C141__INCLUDED_)
