#ifndef __SENDFILETO_H__
#define __SENDFILETO_H__

#include <mapi.h>
#include <string>

class CSendFileTo
{
public:
    static bool SendMailAttach(char *szToMailAddress, char *szToMailName, const std::string& strAttachmentFileName, const std::string& strSubject, char * szNoteText)
 {
  if (strAttachmentFileName.empty())
   return false;

  HINSTANCE hMAPI = ::LoadLibraryA("MAPI32.DLL");
  if (!hMAPI)
   return false;

  ULONG (PASCAL *SendMail)(ULONG, ULONG_PTR, MapiMessage*, FLAGS, ULONG);
  (FARPROC&)SendMail = GetProcAddress(hMAPI, "MAPISendMail");

  if (!SendMail)
        {
            ::FreeLibrary(hMAPI);
            return false;
        }
   

  char szFileName[_MAX_PATH];
  char szPath[_MAX_PATH];
  char szSubject[_MAX_PATH];
        
        strcpy(szFileName,strAttachmentFileName.c_str());
        strcpy(szPath, strAttachmentFileName.c_str());
        strcpy(szSubject, strSubject.c_str());

  /*::StrCpy(szFileName, strAttachmentFileName.GetString());
  ::StrCpy(szPath, strAttachmentFileName.GetString());
  ::StrCpy(szSubject, strSubject.GetString());*/

  MapiFileDesc fileDesc;
  ::ZeroMemory(&fileDesc, sizeof(fileDesc));
  fileDesc.nPosition = (ULONG)-1;
  fileDesc.lpszPathName = szPath;
  fileDesc.lpszFileName = szFileName;

  MapiMessage message;
  ::ZeroMemory(&message, sizeof(message));
  message.lpszSubject = szSubject;
  message.nFileCount = 1;
  message.lpFiles = &fileDesc;
  message.lpszNoteText = szNoteText;

  //写入收件人地址

  MapiRecipDesc recv = {0}; 
  recv.ulRecipClass = MAPI_TO; 
  recv.lpszAddress = szToMailAddress; 
  recv.lpszName = szToMailName; 

  message.lpRecips = &recv; 
  message.nRecipCount = 1; 



  (FARPROC&)SendMail = GetProcAddress(hMAPI, "MAPISendMail");

  int nError = SendMail(0, (ULONG)::GetDesktopWindow(), &message, MAPI_LOGON_UI|MAPI_DIALOG, 0);

  if (nError != SUCCESS_SUCCESS && nError != MAPI_USER_ABORT && nError != MAPI_E_LOGIN_FAILURE)
        {
            ::FreeLibrary(hMAPI);
            return false;
        }
   
        ::FreeLibrary(hMAPI);
  return true;
 }

    static bool SendMail(char* szToEmail, char* szToName, char * szNoteText)
    {
        /*
        MapiFileDesc fileDesc; 
        memset(&fileDesc, 0, sizeof(fileDesc)); 
        fileDesc.nPosition = (ULONG)-1; 

        //USES_CONVERSION; 

        CString strFullPath("c://test.txt"); 
        CString strFileName; 
        int nPos = strFullPath.ReverseFind(_T('//')); 
        if (nPos != -1) 
        { 
            strFileName = strFullPath.Right(strFullPath.GetLength() - nPos - 1); 
        } 
        else 
        { 
            strFileName = strFullPath; 
        } 

        fileDesc.lpszPathName = strFullPath.GetBuffer(); 
        fileDesc.lpszFileName = strFileName.GetBuffer(); 
        */

        MapiMessage message; 
        memset(&message, 0, sizeof(message)); 

        message.lpszSubject = ""; 
        message.nFileCount = 0; 
        message.lpFiles = NULL; 
        message.flFlags = MAPI_SENT; 
        message.ulReserved = 0; 
		message.lpszNoteText = szNoteText;

        /*
        MapiRecipDesc sender = {0}; 
        sender.ulRecipClass = MAPI_ORIG; 
        sender.lpszName = T2A("ken"); 
        char szAdderss[100] = "SMTP:"; 
        strncat(szAdderss, sender.lpszName, 100); 
        sender.lpszAddress = szAdderss; 
        sender.ulReserved = 0; 


        message.lpOriginator = &sender; 
        */

        MapiRecipDesc recv = {0}; 
        recv.ulRecipClass = MAPI_TO; 
        recv.lpszAddress = szToEmail; 
        recv.lpszName = szToName; 
		

        message.lpRecips = &recv; 
        message.nRecipCount = 1; 

        HINSTANCE hMAPI = ::LoadLibraryA("MAPI32.DLL");
        if ( hMAPI == NULL )
            return false;

        ULONG (PASCAL *SendMail)(ULONG, ULONG_PTR, MapiMessage*, FLAGS, ULONG);
        (FARPROC&)SendMail = GetProcAddress(hMAPI, "MAPISendMail");

        if (!SendMail)
        {
           
            ::FreeLibrary(hMAPI);
            return false;
        }


        int nError = SendMail(0, (ULONG)::GetDesktopWindow(), 
            &message, MAPI_DIALOG | MAPI_LOGON_UI , 0); 

        if (nError != SUCCESS_SUCCESS && 
            nError != MAPI_USER_ABORT && nError != MAPI_E_LOGIN_FAILURE) 
        { 
            ::FreeLibrary(hMAPI);
            return false;
        } 

        ::FreeLibrary(hMAPI);
        return true;

    }
};

#endif

