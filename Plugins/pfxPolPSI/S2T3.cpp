#include "stdafx.h"
#include "S2T3.h"
#include "DlgS2T3.h"
#include <complex>

CS2T3::CS2T3(void)
{
}


CS2T3::~CS2T3(void)
{
}

void CS2T3::Process()
{
	CDlgS2T3 dlg;
	if(dlg.DoModal()!=IDOK)
		return;

	S2toT3(dlg.m_strHH,dlg.m_strHV,dlg.m_strVH,dlg.m_strVV
		, dlg.m_strT11,dlg.m_strT12,dlg.m_strT13
		, dlg.m_strT22,dlg.m_strT23,dlg.m_strT33);

}

void CS2T3::Batch()
{
	S2toT3( m_aryStrs.GetAt(0)
		,  m_aryStrs.GetAt(1)
		,  m_aryStrs.GetAt(2)
		,  m_aryStrs.GetAt(3)
		,  m_aryStrs.GetAt(4)
		,  m_aryStrs.GetAt(5)
		,  m_aryStrs.GetAt(6)
		,  m_aryStrs.GetAt(7)
		,  m_aryStrs.GetAt(8)
		,  m_aryStrs.GetAt(9));
}

void CS2T3::S2toT3(CString strInHH,CString strInHV,CString strInVH,CString strInVV, CString strT11,CString strT12,CString strT13,CString strT22,CString strT23,CString strT33)
{
	CRMGImage imag(strInHH);
	CFile hhFile,hvFile,vhFile,vvFile,T11File,T12File,T13File,T22File,T23File,T33File;
	int iWidth = imag.m_oHeader.Sample;
	int iRow = imag.m_oHeader.Line;

	CFileException ex;
	if(!hhFile.Open(strInHH,CFile::modeRead | CFile::shareDenyWrite, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		return;
	}
	if(!hvFile.Open(strInHV,CFile::modeRead | CFile::shareDenyWrite, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		hhFile.Close();
		return;
	}
	if(!vhFile.Open(strInVH,CFile::modeRead | CFile::shareDenyWrite, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		hhFile.Close();
		hvFile.Close();
		return;
	}
	if(!vvFile.Open(strInVV,CFile::modeRead | CFile::shareDenyWrite, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		hhFile.Close();
		hvFile.Close();
		vhFile.Close();
		return;
	}

	if (!T11File.Open(strT11, CFile::modeWrite |
		CFile::shareExclusive | CFile::modeCreate, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		hhFile.Close();
		hvFile.Close();
		vhFile.Close();
		vvFile.Close();
		return;
	}
	if (!T12File.Open(strT12, CFile::modeWrite |
		CFile::shareExclusive | CFile::modeCreate, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		hhFile.Close();
		hvFile.Close();
		vhFile.Close();
		vvFile.Close();
		T11File.Close();
		return;
	}
	if (!T13File.Open(strT13, CFile::modeWrite |
		CFile::shareExclusive | CFile::modeCreate, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		hhFile.Close();
		hvFile.Close();
		vhFile.Close();
		vvFile.Close();
		T11File.Close();
		T12File.Close();
		return;
	}
	if (!T22File.Open(strT22, CFile::modeWrite |
		CFile::shareExclusive | CFile::modeCreate, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		hhFile.Close();
		hvFile.Close();
		vhFile.Close();
		vvFile.Close();
		T11File.Close();
		T12File.Close();
		T13File.Close();
		return;
	}
	if (!T23File.Open(strT23, CFile::modeWrite |
		CFile::shareExclusive | CFile::modeCreate, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		hhFile.Close();
		hvFile.Close();
		vhFile.Close();
		vvFile.Close();
		T11File.Close();		T12File.Close();		T13File.Close();
		T22File.Close();
		return;
	}
	if (!T33File.Open(strT33, CFile::modeWrite |
		CFile::shareExclusive | CFile::modeCreate, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
		hhFile.Close();
		hvFile.Close();
		vhFile.Close();
		vvFile.Close();
		T11File.Close();		T12File.Close();		T13File.Close();
		T22File.Close();		T23File.Close();
		return;
	}
	if(imag.m_oHeader.DataType == eCINT16)
	{
		;
	}
	else if(imag.m_oHeader.DataType == eCFLOAT32)
	{
		complex<float> *hhData,*hvData,*vhData,*vvData;
		complex<float> *T12Data,*T13Data,*T23Data;
		float *T11Data, *T22Data, *T33Data;

		hhData = new complex<float>[iWidth];
		hvData = new complex<float>[iWidth];
		vhData = new complex<float>[iWidth];
		vvData = new complex<float>[iWidth];

		T11Data = new float[iWidth];
		T22Data = new float[iWidth];
		T33Data = new float[iWidth];

		T12Data = new complex<float>[iWidth];
		T13Data = new complex<float>[iWidth];
		T23Data = new complex<float>[iWidth];

		int i,j;
		const float coeff = 1./sqrt(2.);
		for(i=0;i<iRow;i++)
		{
			hhFile.Read(hhData,sizeof(complex<float>)*iWidth);
			hvFile.Read(hvData,sizeof(complex<float>)*iWidth);
			vhFile.Read(vhData,sizeof(complex<float>)*iWidth);
			vvFile.Read(vvData,sizeof(complex<float>)*iWidth);

			for(j=0;j<iWidth;j++)
			{
				T11Data[j] = norm(coeff*(hhData[j]+vvData[j]));
				T12Data[j] = coeff*(hhData[j]+vvData[j])*conj(coeff*(hhData[j]-vvData[j]));
				T13Data[j] = coeff*(hhData[j]+vvData[j])*conj(coeff*(hvData[j]+vhData[j]));
				T22Data[j] = norm(coeff*(hhData[j]-vvData[j]));
				T23Data[j] = coeff*(hhData[j]-vvData[j])*conj(coeff*(hvData[j]+vhData[j]));
				T33Data[j] = norm((coeff*(hvData[j]+vhData[j])));
			}
			T11File.Write(T11Data,sizeof(float)*iWidth);
			T22File.Write(T22Data,sizeof(float)*iWidth);
			T33File.Write(T33Data,sizeof(float)*iWidth);
			T12File.Write(T12Data,sizeof(complex<float>)*iWidth);
			T13File.Write(T13Data,sizeof(complex<float>)*iWidth);
			T23File.Write(T23Data,sizeof(complex<float>)*iWidth);
		}
		delete[] hhData;	delete[] hvData;	delete[] vhData;	delete[] vvData;
		delete[] T11Data;	delete[] T12Data;	delete[] T13Data;
		delete[] T22Data;	delete[] T23Data;	delete[] T33Data;
	}


	hhFile.Close();
	hvFile.Close();
	vhFile.Close();
	vvFile.Close();

	T11File.Close();		T12File.Close();		T13File.Close();
	T22File.Close();		T23File.Close();		T33File.Close();

	CRMGHeader header(imag.m_oHeader);
	header.Band=1;
	header.DataType = eFLOAT32;
	strT11.Replace(".rmg",".ldr");
	header.Save(strT11);

	strT22.Replace(".rmg",".ldr");
	header.Save(strT22);
	strT33.Replace(".rmg",".ldr");
	header.Save(strT33);

	header.DataType = eCFLOAT32;
	strT12.Replace(".rmg",".ldr");
	header.Save(strT12);
	strT13.Replace(".rmg",".ldr");
	header.Save(strT13);
	strT23.Replace(".rmg",".ldr");
	header.Save(strT23);

	return;
}