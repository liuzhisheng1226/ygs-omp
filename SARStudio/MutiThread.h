#pragma once
class CMutiThread
{
public:
	CMutiThread(void);
	~CMutiThread(void);


	static void Invoke(int m_nId)
	{
		DWORD thId;
		

		CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)Test,
			(LPVOID)m_nId,
			0,
			&thId);
	}

	static void Test(LPVOID lpParam)
	{
		int nId=(int)lpParam;
		CEnviStyleManager::Instance()->EnviWindowFinder(nId)->ResetView();		
	}
};

