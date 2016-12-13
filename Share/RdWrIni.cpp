/********************************************************************  
created:    2009/10/10  
filename:   RdWrIni.cpp,RdWrIni.h
author:     YRC  
purpose:    Read and Write Ini file  
*********************************************************************/  

#include "RdWrIni.h"

LPSTR g_pData = NULL;    // �洢����INI�ļ����ݵĻ�����    

/************************************************************************  
**������CopyFile  
**���ܣ�The CopyFile function copies an existing file to a new file. 

**������  
		lpExistingFileName[in] -Pointer to a null-terminated string
							    that specifies the name of an existing file. 
		lpNewFileName[in]      - Pointer to a null-terminated string that specifies the name of the new file. 
		bFailIfExists[in]      - Specifies how this operation is to proceed if a file of the same name as that 
							      specified by lpNewFileName already exists.
								   If this parameter is TRUE and the new file already exists, the function fails. 
									If this parameter is FALSE and the new file already exists, 
									the function overwrites the existing file and succeeds. 
**���أ�  
		TRUE - success  
		FALSE - fail 
**���ߣ�YRC
**���ڣ�09.10.20  
**��ע��
************************************************************************/  

BOOL CopyFile(LPCSTR lpExistingFileName, // name of an existing file
			  LPCSTR lpNewFileName,      // name of new file
			  BOOL bFailIfExists         // operation if file exists
			  )
{
	INT fdIn = 0, fdOut = 0;
	struct stat statinfo;
	DWORD dwSize =0;
	void *pSrc =NULL, *pDst = NULL;
	
	memset(&statinfo,0, sizeof(statinfo));
	if (!lpExistingFileName || !lpExistingFileName)
	{
		return FALSE;
	}
	
	fdIn = open(lpExistingFileName, O_RDONLY);
	if (fdIn<0)
	{
		perror("CopyFile open");
		return FALSE;
	}
	// get file info
	if (fstat(fdIn, &statinfo) < 0) 
	{
		perror("CopyFile fstat ");
	}
	dwSize = statinfo.st_size;
	//printf("statinfo.st_size = %d\n ", statinfo.st_size);
	if (bFailIfExists)
	{
		fdOut = open(lpNewFileName, O_RDWR | O_CREAT | O_EXCL,S_IRWXU);
		if (fdOut== -1)
		{
			perror("CopyFile new file fail");
			return FALSE;
		}
	}
	else
	{
		fdOut = open(lpNewFileName, O_RDWR | O_CREAT |O_TRUNC,S_IRWXU);
		if (fdOut== -1)
		{
			perror("CopyFile new file fail ");
			return FALSE;
		}
	}
	//printf("CopyFile lseek\n");
	if (lseek(fdOut, dwSize - 1, SEEK_SET) == -1) 
	{
		perror("CopyFile lseek ");
		return FALSE;
	}
	
	if (write(fdOut, "", 1) != 1) 
	{
		perror("the file isn't writable ");
		return FALSE;
	}
	//printf("CopyFile write\n");
	if ((pSrc = mmap(NULL, dwSize, PROT_READ, MAP_SHARED, fdIn, 0)) == MAP_FAILED)
	{
		perror("mmap ");
		return FALSE;	
	}
	//printf("CopyFile mmap 1\n");
	if ((pDst = mmap(NULL, dwSize, PROT_READ|PROT_WRITE, MAP_SHARED, fdOut, 0)) == MAP_FAILED) 
	{
		perror("mmap ");
		return FALSE;
	}
	//printf("CopyFile mmap 2\n");
	//dose copy the file
	memcpy(pDst,pSrc, dwSize);
	close(fdOut);
	close(fdIn);

//	if (pSrc)
//	{
//		free(pSrc);
//	}
//	if (pDst)
//	{
//		free(pDst);
//	}
//	printf("CopyFile exit\n");
	return TRUE;
}

/************************************************************************  
**������GetLine  
**���ܣ���ȡ��g_pData�д�dwOffsetλ�ÿ�ʼ��һ�����ݲ����浽pLine��ͬʱ��ƫ����dwOffset  
		�Ƶ���һ������  
**������  
		pLine[out]   - ����һ������(������\r\n)  
		dwOffset[in] - Ҫ��ȡ����һ�еĿ�ʼλ��  
		dwSize[in]   - INI�ļ���С  
**���أ�  
		��ȷ - ��һ�����׵�λ��  
		���� - 0  
**���ߣ�YRC
**���ڣ�09.10.19  
**��ע��
************************************************************************/   
INT	GetLine(LPSTR pLine, DWORD dwOffset, DWORD dwSize)
{
	int len = 0;   
    int len2 = 0;   
    // Look for the end of the line.    
    while ( dwOffset + len < dwSize   
        && '\r' != g_pData[dwOffset+len] && '\n' != g_pData[dwOffset+len])   
    {   
        if( g_pData[dwOffset+len]==0 )   
            break;   
        pLine[len] = g_pData[dwOffset+len] ;   
        ++len;   
    }   
	
    pLine[len] = 0 ;   
    // Now push the internal offset past the newline.    
    // (We assume \r\n pairs are always in this order)    
    if (dwOffset + len + len2 < dwSize && '\r' == g_pData[dwOffset+len+len2])   
        ++len2;   
    if (dwOffset + len + len2+1 < dwSize && '\n' == g_pData[dwOffset+len+len2])   
        ++len2;   
    if (2 >= len + len2 && (dwOffset +2 >=dwSize) )   
        return 0;   
	
    dwOffset += len + len2;   
    return dwOffset;   
}

/************************************************************************  
**������IsComment  
**���ܣ��ж��ǲ���ע����  
**������  
		pLine[in] - INI��һ������  
**���أ�  
		1 - ע����  
		0 - ����ע����  
**��ע��  
		1). ����Ҳ��Ϊע����  
************************************************************************/ 

BOOL IsComment(LPCSTR pLine)
{
	if (!pLine || 0 == strlen(pLine) || ';' == *pLine)   
        return TRUE;   
    return FALSE;  
}

/************************************************************************  
**������IsSection  
**���ܣ��ж��ǲ��Ƕ���  
**������  
		pLine[in] - INI��һ������  
**���أ�  
		1 - �Ƕ���  
		0 - ����  
************************************************************************/   
BOOL IsSection(LPCSTR pLine)   
{   
    if (pLine && '[' == *pLine)   
        return TRUE;   
    return FALSE;   
}   

/************************************************************************  
**������IsSectionName  
**���ܣ��ж���INI�ļ���һ��(pLine)�ǲ���Ҫ�ҵĶ���(pSection)  
**������  
pLine[in]    - INI�ļ���һ������  
pSection[in] - Ҫ�ҵĶ���  
**���أ�  
1 - ��  
0 - ����  
**��ע��  
************************************************************************/   
BOOL IsSectionName(LPCSTR pLine, LPCSTR pSection)   
{   
    if (IsSection(pLine))   
    {   
        DWORD len = strlen(pSection);   
        if (strlen(pLine) - 2 == len && 0 == strncasecmp(pLine+1, pSection, len))   
            return TRUE;   
    }   
    return FALSE;   
}  


/************************************************************************  
**������IsKey  
**���ܣ��ж�INI�ļ���һ�е������ǲ���Ҫ�ҵļ���,����ǲ���ȡ��ֵ  
**������  
		pLine[in] - INI�ļ�ĳ������  
		pKeyName[in] - ҪѰ�ҵļ���  
		pValue[out] - ��ֵ  
		dwValLen[out] - ��ֵpValue��С(in bytes)  
**���أ�  
		1 - �ǣ�ͬʱpValue���ؼ�ֵ  
		0 - ���ǣ�pValueΪNULL  
**���ߣ�YRC  
**���ڣ�09.10.19  
**��ע��  
************************************************************************/   
BOOL IsKey(LPSTR pLine , LPCSTR pKeyName, LPSTR* pValue, DWORD* dwValLen )   
{   
    LPSTR pEqual = NULL;   
    DWORD length = 0, len = 0;   
	
    if(!pLine || !pKeyName)   
        return FALSE;   
	
    // pLine�ǲ���ע����    
    if (IsComment( pLine ))   
        return FALSE;   
	
    // Ѱ��"="��    
    pEqual = strchr(pLine, '=' );   
    if (!pEqual)   
        return FALSE;   
	
    // Ѱ�Ҽ������һ�ַ���λ��    
    //while (pEqual - 1 >= pLine && iswspace(*(pEqual-1)))   
	//while(pEqual - 1 >= pLine && ((*(pEqual-1)) == "")) 
	while(pEqual - 1 >= pLine && ((*(pEqual-1)) == ' ')) 
        --pEqual;   
    // Badly formed file.    
    if (pEqual - 1 < pLine)   
        return FALSE;   
	
    // ��������    
    length = pEqual - pLine;   
	
    len = strlen(pKeyName);   
    //if (len == length && 0 == _wcsnicmp(pLine, pKeyName, len))    
    if ( 0 == strncasecmp(pLine, pKeyName, len))   
    {   
        *pValue = strchr(pLine, '=' );   
        ++(*pValue);   
        *dwValLen = strlen(pLine) - ((*pValue) - pLine);   
		
        // ȥ��������"="�ź�����пո�    
        //while (0 < *dwValLen && iswspace(**pValue))   
		//while (0 < *dwValLen && ((**pValue) == ""))   
		while (0 < *dwValLen && ((**pValue) == ' '))   
        {   
            ++(*pValue);   
            --(*dwValLen);   
        }   
        //while (0 < *dwValLen && ((*pValue)[*dwValLen-1] == ""))   
        while (0 < *dwValLen && ((*pValue)[*dwValLen-1] == ' '))   
        {   
            --(*dwValLen);   
        }   
        // If the string is surrounded by quotes, remove them.    
        if ('"' == (**pValue))   
        {   
            ++(*pValue);   
            --(*dwValLen);   
            if ('"' == (*pValue)[*dwValLen-1])   
            {   
                --(*dwValLen);   
            }   
        }   
        return TRUE;   
    }   
    else   
    {   
        *pValue = NULL;   
        return FALSE;   
    }   
}   

/************************************************************************  
**������ReadIniFile  
**���ܣ����ļ��������ļ����ݿ�����һ������g_pData��   
**������  
		lpFileName[in] - INI�ļ��������û��·����Ĭ��·����ǰӦ�ó���·�� 
**���أ�  
		��0 - ��INI�ļ��Ĵ�С(in bytes)  
		0   - ʧ��  
**���ߣ�YRC  
**���ڣ�09.10.20  
**��ע��  

************************************************************************/   
INT ReadIniFile(LPCSTR lpFileName)   
{   
    int nReturn = 0;   
	struct stat stat;
    int fdFile =0;   
    DWORD dwSize =0 ;   
    BOOL  bRet = FALSE; // bUnicode - ��־INI�ļ��ǲ���Unicode�ļ�    
    BYTE* pData = NULL; 
	char buf[255] = {0};
	
    if (!lpFileName)   
    {   
        nReturn = 0;   
        goto exit;   
    }   
	
    // ReadIniFile the file.    
	fdFile = open(lpFileName, O_RDONLY);
	if(fdFile < 0)
	{
		perror("Open file fail");
		nReturn = 0;   
        goto exit;   
	}
	//printf("+++++++fdFile = %d\n", fdFile);
	if(read(fdFile, buf,sizeof(buf))>0)
	{
		//printf("++++++buf :%s \n",buf);
	}

    // Get its size.    
	
	if((fstat(fdFile,&stat) < 0) )
	{
		nReturn = 0;   
        goto exit;   
	}
	
	dwSize = stat.st_size;
	
    // Next, load the data.    
    //printf("ReadIniFile, the size of ini file [%s] is [%d]\n", lpFileName, dwSize);     
    if (0 < dwSize)   
    {   
        pData = (BYTE *)malloc((dwSize + sizeof(char)));   
        if (!pData)   
        {   
			printf("ReadIniFile, no enough memory\n");   
            nReturn = 0;   
            goto exit;   
        }   
        memset(pData,0,dwSize + sizeof(char));
		lseek(fdFile, 0, SEEK_SET);
		bRet = read(fdFile, pData, dwSize);
        close(fdFile);   
        if (bRet != dwSize)   
        {   
			
            printf("ReadIniFile, ReadFile fail, err = %s\n",strerror(errno));    
            nReturn = 0;   
            goto exit;   
        }   
		
        // Create the buffer.    
        if( g_pData )   
        {   
            free(g_pData);   
            g_pData = NULL;   
        }   
		
        g_pData = (char *)malloc(dwSize*sizeof(char));   
        if(!g_pData)   
        {   
            printf("ReadIniFile, no enough momery\n");   
            nReturn = 0;   
            goto exit;   
        }   
		
		memcpy(g_pData, pData, dwSize);     
        dwSize--; // includes the NULL termination character    
        nReturn = dwSize;   
    } 
	else
	{
		//printf("read file dwSize<0\n");
		goto exit;
	}
	
exit:   
    if(pData)   
    {   //printf("ReadIniFile free\n");
        free(pData);   
        pData = NULL;   
    }   
    if(fdFile)   
    {  
		//printf("ReadIniFile close\n");
        close(fdFile);   
        fdFile = 0;   
    }  
	//printf("exit ReadIniFile\n");
    return nReturn;   
}   


/************************************************************************  
**������GetString  
**���ܣ���INI�ļ�  
**������  
		lpAppName[in]         - �ֶ���  
		lpKeyName[in]         - ����  
		lpReturnedString[out] - ��ֵ  
		nSize[in]             - ��ֵ��������С(in characters )  
		lpFileName[in]        - ������INI�ļ���  
**���أ�Returns the number of bytes read.  
**���ߣ�YRC  
**���ڣ�09.10.20  
**��ע��  
************************************************************************/   
DWORD GetString( LPCSTR lpAppName,LPCSTR lpKeyName,   
                LPSTR lpReturnedString, DWORD nSize,LPCSTR lpFileName)   
{     
    DWORD dwSize , cchCopied;   
    DWORD dwOffset = 0;   
    char pLine[MAX_PATH] = {0} ;   
    DWORD dwValLen = 0;   
	
    if (!lpAppName || !lpFileName)   
        return 0;   
	
    //���ļ������ļ����ݿ�����������g_pData�У�����INI�ļ���С    
    if (0 == (dwSize = ReadIniFile(lpFileName)))   
    {   
        printf("GetString, Could not ReadIniFile INI file: %s\n", lpFileName);   
        return 0;   
    }   
	
    cchCopied = 0;   
    while ( 0 != (dwOffset = GetLine( pLine , dwOffset , dwSize )))   
    {   
        //RETAILMSG(1,(_T("%s\n"),szLine));    
        // �ǲ���ע����    
        if (IsComment(pLine))   
            continue;   
		
        // �ǲ��Ƕ���    
        if (IsSection(pLine))   
        {   
            // �ǲ�������Ҫ�ҵĶ���    
            if (IsSectionName(pLine,lpAppName))   
            {   
                // Ѱ������Ҫ�ļ���    
                while ( 0 != (dwOffset = GetLine(pLine , dwOffset , dwSize)))   
                {   
                    LPSTR pValue=NULL;   
					
                    if (IsSection(pLine))   
                        break;   
					
                    if (IsKey(pLine , lpKeyName, &pValue, &dwValLen))   
                    {   
                        cchCopied = Min(dwValLen, nSize-1);   
                        memcpy(lpReturnedString, pValue, cchCopied);   
                        lpReturnedString[cchCopied] = 0;   
                        // We're done.    
                        break;                         
                    }   
                }   
                break;   
            }   
        }   
    }   
	
    return cchCopied;   
}  

/************************************************************************  
**������GetPrivateProfileString  
**���ܣ�LINUX�¶�ȡINI�ļ���ĳ����/�����ļ�ֵ���ַ���  
**������  
		lpAppName[in]         - points to section name  
		lpKeyName[in]         - points to key name  
		lpDefault[in]         - points to default string  
		lpReturnedString[out] - points to destination buffer  
		nSize[in]             - size of destination buffer "lpReturnedString"(in characters)  
		lpFileName[in]        - points to initialization filename     
**���أ�The return value is the number of characters copied to the buffer,   
		not including the terminating null character.          
**���ߣ�YRC 
**���ڣ�09.10.20  
**��ע��  
1). ���INI�ļ�û������ĵ����ݣ�����Ĭ��ֵlpDefault  
************************************************************************/   
DWORD GetPrivateProfileString(   
							  LPCSTR lpAppName,   
							  LPCSTR lpKeyName,   
							  LPCSTR lpDefault,    
							  LPSTR  lpReturnedString,   
							  DWORD   Size,   
							  LPCSTR lpFileName )   
{   
    DWORD dwRc = 0, dwReturn = 0;   
    if(!lpAppName || !lpKeyName || !lpReturnedString || !lpFileName || Size<=0 )   
        return 0;   
	
    dwRc = GetString(lpAppName,lpKeyName,lpReturnedString,Size,lpFileName);   
    if(dwRc != 0)   
    {   
        dwReturn = dwRc;   
    }   
    else   
    {   
        if(lpDefault)   
        {   
            memcpy(lpReturnedString, lpDefault, Size);   
            lpReturnedString[Size-1] = NULL;   
        }   
        else   
            *lpReturnedString = 0;   
        dwReturn = strlen(lpReturnedString);   
    }   
	
    // �ͷ��ڴ�    
    if (NULL != g_pData)   
    {   
        free( g_pData );   
        g_pData = NULL;   
    }   
	
    return dwReturn;   
}   

/************************************************************************  
**������GetPrivateProfileInt  
**���ܣ� retrieves an integer associated with a key in the  
		specified section of the given initialization file  
**������  
		LPCTSTR lpAppName,  // address of section name  
		LPCTSTR lpKeyName,  // address of key name  
		INT nDefault,       // return value if key name is not found  
		LPCTSTR lpFileName  // address of initialization filename  
**���أ�  
		The return value is the integer equivalent of the string following   
		the specified key name in the specified initialization file. If the   
		key is not found, the return value is the specified default value.   
		If the value of the key is less than zero, the return value is zero.   
**���ߣ�YRC 
**���ڣ�09.10.20 
**��ע��  
************************************************************************/   
UINT GetPrivateProfileInt(   
						  LPCSTR lpAppName,   
						  LPCSTR lpKeyName,    
						  INT nDefault,   
						  LPCSTR lpFileName )   
{   
    char szRet[80] ={0}; 
	DWORD cch =0;
	
    if(!lpAppName || !lpKeyName || !lpFileName )   
        return 0;   
	
    cch = GetString(lpAppName, lpKeyName, szRet, sizeof(szRet)/sizeof(char), lpFileName);   
	
    // �ͷ��ڴ�    
    if (NULL != g_pData)   
    {   
        free( g_pData );   
        g_pData = NULL;   
    }   
	
    if (cch)   
        return atoi(szRet);   
    else   
        return nDefault;       
}  

/************************************************************************  
**������WriteLine  
**���ܣ����ļ�д��һ������(�����س����з�)  
**������  
		hOutput[in] - �Ѵ򿪵��ļ����  
		pLine[in]   - Ҫд���һ������  
**���أ�NONE  
**���ߣ�YRC  
**���ڣ�09.10.20
**��ע��  
		1). д��һ�����ݣ�Ҳ����д����ĩ��"\r\n"�����ַ�  
		3). ע�ⲻҪ��������Ҳд���ļ�  
************************************************************************/   
void WriteLine(INT hOutput , LPCSTR pLine)   
{   
	if (pLine)   
	{   
		write(hOutput, pLine, strlen(pLine)*sizeof(char));   
		write(hOutput, "\r\n", 2*sizeof(char));   
	}   
}   

/************************************************************************  
**������WritePrivateProfileString  
**���ܣ�LINUX�����£���ָ��INI�ļ�ָ������д���ַ�������  
**������  
		lpAppName[in]  
		Pointer to a null-terminated string containing section name. If  
		the section does not exit, it is created.  
		lpKeyName[in]  
		Pointer to a null-terminated string containing key name. If the  
		key does not exit in the specified section pointed to by the lpAppName   
		parameter, it's created. If this parameter is NULL, the ertire section,   
		including all keys within the section, is deleted. When deleting a   
		section, leave the comments intact.  
		lpString[in]  
		pointer to a null-terminated string to be written to the file.   
		If this parameter is NULL, the key pointed to by the lpKeyName   
		parameter is deleted.  
		lpFileName[in]  
		Pointer to a null-terminated string that names the initialization file.   
**���أ�  
		FALSE - fail  
		TRUE  - success  
**���ߣ�YRC  
**���ڣ�09.10.20
**��ע��  
		1). �Ƚ�Ҫ�޸ĵ�INI�ļ���ȫ�����ݶ�ȡ��ȫ���ڴ�g_pData��  
		2). ��g_pData�ж�λ������Ҫ�޸ĵ�λ�ã����������ݺ������޸ĵ�����д��һ��ʱini�ļ�  
		3). �����ʱini�ļ�����ԭ����ini�ļ�����ɾ����ʱini�ļ�  
		4). ��Ҫ��API������  
		creat��lseek��close��remove��open��mmap 
		5). ���lpKeyName == NULL, ɾ��������, ���lpString == NULL, ɾ����  
************************************************************************/   
BOOL WritePrivateProfileString(   
							   LPCSTR lpAppName,   
							   LPCSTR lpKeyName,   
							   LPCSTR lpString,   
							   LPCSTR lpFileName)   
{   
    DWORD dwSize, dwOffset;                // dwSize - ini�ļ���С, dwOffset - ƫ����    
    BOOL  bReadLine = TRUE;   
    BOOL  bWrote = FALSE;   
    char pszLine[MAX_PATH] = {0};         // �洢һ�е�����    
    char pszIniFileTemp[MAX_PATH] = {0};  // ��ʱini�ļ�������(����·��)    
    INT  fdOutputFile ;   
    LPSTR pValue;   
    DWORD dwValLen;     
	
    dwOffset = 0;   
    if (!lpFileName)   
        return FALSE;   
	
    // ��ȡINI�ļ����ݵ�ȫ�ֱ���g_pData�ڴ���    
    dwSize = ReadIniFile(lpFileName);   
	
    //RETAILMSG(1, (TEXT("lpFileName=[%s], dwSize=[%d]"), lpFileName, dwSize));    
	
    // Create the output file.    
    //wcscpy(pszIniFileTemp, lpFileName);    
	
    sprintf(pszIniFileTemp, "%s.ini.tmp",lpAppName);   
//	printf("pszIniFileTemp = %s\n", pszIniFileTemp);
    
    fdOutputFile = creat(pszIniFileTemp, S_IRWXU| S_IRWXO |S_IRWXG);
	
    if (-1 == fdOutputFile)   
    {   
        perror("Could not open output file");   
        return FALSE;   
    }   
	
    // ����������д����ʱini�ļ�    
    for (;;)   
    {   
        // The bReadLine flag is used to not read a new line after we break    
        // out of the inner loop. We've already got a line to process.    
        if (bReadLine)   
        {   
            dwOffset = GetLine( pszLine , dwOffset , dwSize );   
            if (!dwOffset)     
			{
				//printf("dwOffset = %d \n", dwOffset);
                break;
			}   
        }   
        bReadLine = TRUE;   
        // Skip past comments.    
        if (IsComment(pszLine))   
        {   
            WriteLine(fdOutputFile , pszLine);   
            continue;   
        }   
        // Found a section name.    
        if (IsSection(pszLine))   
        {   
            // It's the section we want.    
            if (IsSectionName(pszLine , lpAppName))   
            {   
                // �������lpKeyNameΪNULL��ɾ��������    
                if (lpKeyName)   
                    WriteLine(fdOutputFile , pszLine);   
				
                // Process the whole section.    
                while (0 != (dwOffset = GetLine( pszLine , dwOffset , dwSize )))   
                {   
                    // Reached the end of the section.    
                    if (IsSection(pszLine))   
                    {   
                        bReadLine = FALSE;   
                        // This line will be written in the outer loop.    
                        break;   
                    }   
                    // When deleting a section, leave the comments intact.    
                    else if (IsComment(pszLine))   
                    {   
                        WriteLine(fdOutputFile , pszLine);   
                        continue;   
                    }   
                    // Got the value we want.    
                    if (!bWrote && IsKey(pszLine , lpKeyName, &pValue, &dwValLen))   
                    {   
                        bWrote = TRUE;   
                        // ���lpStringΪNULL��ɾ����lpKeyName    
                        if(lpString)   
                            WriteValue(fdOutputFile , NULL, lpKeyName, lpString);   
                    }   
                    else   
                    {   
                        if (lpKeyName)   
                            WriteLine(fdOutputFile , pszLine);   
                    }   
					
                    if(dwOffset >= dwSize)   
                        break ;   
                }   
				
                // ����ڶ���lpAppName�¼���lpKeyName�����ڣ����½�����lpKeyName�ͼ�ֵlpString    
                if (!bWrote)   
                {   
                    bWrote = TRUE;   
                    WriteValue(fdOutputFile, NULL, lpKeyName, lpString);   
                }   
            }   
            else   
                WriteLine(fdOutputFile , pszLine);   
        }   
        else   
            WriteLine(fdOutputFile , pszLine);   
		
        if(dwOffset ==0)    
            break;   
    }   
	
    // ���ָ���Ķ���lpAppName�����ڣ����½�����lpAppName������lpKeyName�ͼ�ֵlpString    
    if (!bWrote && lpKeyName && lpString)   
    {   
		printf("create key! lpAppName = %s, lpKeyName = %s , lpString =%s \n",lpAppName,lpKeyName,lpString);
        WriteValue(fdOutputFile , lpAppName, lpKeyName, lpString);   
    }   
	
    // ����ʱini�ļ�����ԭ����ini�ļ���ɾ����ʱini�ļ�    
    if (-1 != fdOutputFile)   
    {   
        lseek(fdOutputFile, 0, SEEK_END);   
        close(fdOutputFile);   
        CopyFile(pszIniFileTemp, lpFileName, FALSE);   
        if(remove(pszIniFileTemp) !=0)
		{
			perror("remove file fail ");
		}
		
    }   
	
    // �ͷ�ReadIniFile������ȫ���ڴ�    
    if (NULL != g_pData)   
    {   
        free( g_pData ) ;   
        g_pData = NULL ;   
    }   
	
    return TRUE;   
}   

/************************************************************************  
**������WritePrivateProfileInt  
**���ܣ�LINUX�����£���ָ��INI�ļ�ָ������д����������  
**�������ο�WritePrivateProfileString����  
**���أ�  
		FALSE - fail  
		TRUE  - success  
**���ߣ�YRC  
**���ڣ�09.10.20
**��ע��  
************************************************************************/   
BOOL WritePrivateProfileInt(   
							LPCSTR lpAppName,   
							LPCSTR lpKeyName,   
							INT     Value,   
							LPCSTR lpFileName)   
{      
    char ValBuf[16]={0};       
	
    sprintf( ValBuf, "%d", Value);       
    return( WritePrivateProfileString(lpAppName, lpKeyName, ValBuf, lpFileName) );    
}   

/************************************************************************  
**������WriteValue  
**���ܣ���ָ��INI�ļ���д������������ͼ�ֵ  
**������  
		m_hOutput[in]  
		pointer to the handle of ini file.  
		pAppName[in]  
		Pointer to a null-terminated string containing the name of the section  
		in which data is written. If this parameter is NULL, the WriteValue  
		function just wirte the pKeyName and pString.  
		pKeyName[in]  
		Pointer to a null-terminated string containing the name of the key in  
		which data is writtern.   
		pString[in]  
		Pointer to a null-terminated string to be written to the file.   
**���أ�NONE  
**���ߣ�YRC  
**���ڣ�09.10.20 
**��ע��  
		1). Ҫ�ɹ�д��INI�ļ�������pKeyName�ͼ�ֵpString������ΪNULL��  
		2). �������pAppNameΪNULL����ֻд�����pKeyName�ͼ�ֵpString��  
		3). ע����INI�ļ�д���ַ���ʱ����Ҫд���������  
************************************************************************/   
void WriteValue(INT fdOutput, LPCSTR pAppName, LPCSTR pKeyName, LPCSTR pString)   
{   
 
    if (pKeyName && pString)   
    {   
        if (pAppName)       // д�����    
        {   
			write(fdOutput, "[", sizeof(char));   
			write(fdOutput, pAppName, strlen(pAppName)*sizeof(char));   
			write(fdOutput, "]\r\n", 3*sizeof(char));   									
        }   
		// д�뽡���ͼ�ֵ   
		write(fdOutput, pKeyName, strlen(pKeyName)*sizeof(char));   
		write(fdOutput, "=", sizeof(char));   
		write(fdOutput, pString, strlen(pString)*sizeof(char));   
		write(fdOutput, "\r\n", 2*sizeof(char));  
    }   
}   

