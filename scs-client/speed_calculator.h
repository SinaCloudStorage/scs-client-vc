// speed_calculator.h: interface for the speed_calculator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPEED_CALCULATOR_H__1620FC17_7B0F_4DA4_8591_DDF4F4A1C141__INCLUDED_)
#define AFX_SPEED_CALCULATOR_H__1620FC17_7B0F_4DA4_8591_DDF4F4A1C141__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
该数据计算类目前有一个问题，GetTickCount()在系统运行到49.7天后会归零，该情况未处理
*/

class speed_calculator  
{
public:
    //只要有指针成员，就必须要定义拷贝构造函数和重载=
	speed_calculator(const speed_calculator &calculator);
	speed_calculator &operator = (const speed_calculator &calculator);

	// cycle 表示计算速度时，从当前时间往前推，参与计算的样本数
	// unit 表示每个样本收集数据的时间跨度，单位毫秒
	speed_calculator(unsigned int cycle, unsigned int unit = 1);
	virtual ~speed_calculator();

	void add_bytes( unsigned int bytes );
    unsigned __int64 speed_value( void ); // 获得当前的速度，单位是字节/秒

    void clear(); // 清空所有速度，从零开始

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
