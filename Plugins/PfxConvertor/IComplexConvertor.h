#pragma once
#include <complex>
#include <cstdlib>


enum ENUM_COMPLEX_CONVERT
{
    AMPT,
    NORM,
    ARGU
};

template<class T> class CComplex2Amplitude;
template<class T> class CComplex2Argument;
template<class T> class CComplex2Norm;

template<class T>
class CIComplexConvertor
{
public:
    CIComplexConvertor(void);
    ~CIComplexConvertor(void);

    virtual float Convert(T)=0;
public:
    static CIComplexConvertor<T>* ComplexConvertorFactory(ENUM_COMPLEX_CONVERT eConvert)
    {
        CIComplexConvertor<T> *convertor;
        switch(eConvert)
        {       
        case AMPT:convertor=new  CComplex2Amplitude<T>();break;
        case ARGU:convertor=new CComplex2Argument<T>();break;
        case NORM:convertor=new  CComplex2Norm<T>();break;

        }
        return convertor;
    }

};

/*
 *������ת����Ϊ����ͼ
 */
template<class T>
class CComplex2Amplitude:public CIComplexConvertor<T>
{   
public: 
    
    float Convert(T t)
    {   
        return abs(t);
    }
};


/*
 *������ת����Ϊ����
 */
template<class T>
class CComplex2Norm:public CIComplexConvertor<T>
{
public:     
    float Convert(T t)
    {       
        return norm(t);
    }
};

/*
 *������ת����Ϊ����
 */
template<class T>
class CComplex2Argument:public CIComplexConvertor<T>
{
public:     
    float Convert(T t)
    {       
        return arg(t);
    }
};

