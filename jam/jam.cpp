/* ******************************************************************** **
** @@ Jam Source File
** @  Copyrt : 
** @  Author : 
** @  Update :
** @  Update :
** @  Notes  :
** ******************************************************************** */

/* ******************************************************************** **
**                uses pre-compiled headers
** ******************************************************************** */

#include "stdafx.h"

#include "..\shared\file_find.h"

/* ******************************************************************** **
** @@                   internal #defines
** ******************************************************************** */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef NDEBUG
#pragma optimize("gsy",on)
#pragma comment(linker,"/FILEALIGN:512 /MERGE:.rdata=.text /MERGE:.data=.text /SECTION:.text,EWR /IGNORE:4078")
#endif 

/* ******************************************************************** **
** @@                   internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   external global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@                   static global variables
** ******************************************************************** */

static bool    bRemoveTabs       = true;
static bool    bRemoveEmptyLines = false;

/* ******************************************************************** **
** @@                   real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ Proceed()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static bool Proceed(const char* const pszFileName)
{
   char     pszBackName[_MAX_PATH];
   char     pszDrive   [_MAX_DRIVE];
   char     pszDir     [_MAX_DIR];
   char     pszFName   [_MAX_FNAME];
   char     pszExt     [_MAX_EXT];

   _splitpath(pszFileName,pszDrive,pszDir,pszFName,pszExt);
   _makepath( pszBackName,pszDrive,pszDir,pszFName,"BAK");

   CopyFile(pszFileName,pszBackName,FALSE);

   FILE*    pIn = fopen(pszBackName,"rt");

   if (!pIn)
   {
      perror("\a\nOpen Input File Error !\n");
      return false;
   }

   FILE*    pOut = fopen(pszFileName,"wb");

   if (!pOut)
   {
      perror("\a\nOpen Output File Error !\n");
      fclose(pIn);
      pIn = NULL;
      return false;
   }

   const DWORD    BUF_SIZE = (0x01 << 20);   // About 1 Mb !

   char*    pBuf = new char[BUF_SIZE];

   if (!pBuf)
   {
      fclose(pIn);
      pIn = NULL;
      fclose(pOut);
      pOut = NULL;
      return false;
   }

   char*    pOutBuf = new char[BUF_SIZE];

   if (!pOutBuf)
   {
      delete pBuf;
      pBuf = NULL;
      fclose(pIn);
      pIn = NULL;
      fclose(pOut);
      pOut = NULL;
      return false;
   }

   while (fgets(pBuf,BUF_SIZE,pIn))
   {
      pBuf[BUF_SIZE - 1] = 0; // ASCIIZ !

      DWORD    dwEOL = strcspn(pBuf,"\r\n");

      pBuf[dwEOL] = 0;  // Remove EOL chars

      // Remove TABs
      if (*pBuf && bRemoveTabs)
      {
         char*    pLine = pBuf;

         while (pLine && *pLine)
         {
            if (*pLine == '\t')
            {
               *pLine = ' ';
            }

            ++pLine;
         }
      }

      // Remove Leading Spaces
      char*    pBegin = pBuf;

      while (pBegin && *pBegin == ' ')
      {
         ++pBegin;
      }

      // Remove Trailing Spaces
      int      iSize = strlen(pBuf);

      if (iSize)
      {
         char*    pEnd = pBuf + (iSize - 1);

         while (pEnd && (pEnd > pBegin) && *pEnd == ' ')
         {
            *pEnd = 0;
            --pEnd;
         }
      }

      char*    pTxt = pOutBuf;

      // Remove Duplicated Spaces
      while (pBegin && *pBegin)
      {
         if ((*pBegin == ' ') || (bRemoveTabs && (*pBegin == '\t')))
         {
            *pTxt++ = *pBegin++;                // Copy the First

            while ((*pBegin == ' ') || (bRemoveTabs && (*pBegin == '\t')))
            {
               ++pBegin;                        // Skip !
            }
         }

         *pTxt++ = *pBegin++;                   // Just Copy
      }

      *pTxt = 0;     // ASCIIZ

      // Remove EMPTY lines
      if (!strlen(pOutBuf))
      {
         if (bRemoveEmptyLines)
         {
            continue;
         }
      }

      fprintf(pOut,"%s\r\n",pOutBuf);
   }

   delete[] pBuf;
   pBuf = NULL;

   delete[] pOutBuf;
   pOutBuf = NULL;

   fclose(pIn);
   pIn = NULL;

   fclose(pOut);
   pOut = NULL;

   return true;
}

/* ******************************************************************** **
** @@ ShowHelp()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void ShowHelp()
{
   const char  pszCopyright[] = "-*-   Jam   *   Copyright (c) Gazlan, 2015   -*-";
   const char  pszDescript [] = "Textfile whitespace jammer";
   const char  pszE_Mail   [] = "complains_n_suggestions direct to gazlan@yandex.ru";

   printf("%s\n\n",pszCopyright);
   printf("%s\n\n",pszDescript);
   printf("Usage: jam.com [switches] wildcards\n\n");
   printf("Switches:\n");
   printf("   +t  - Keep tabs\n");
   printf("   -e  - Remove empty lines\n\n");
   printf("%s\n",pszE_Mail);
}

/* ******************************************************************** **
** @@ main()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 06 Nov 2006
** @  Notes  :
** ******************************************************************** */

int main(int argc,char** argv)
{
   if ((argc < 2) || (argc > 4))
   {
      ShowHelp();
      return 0;
   }

   if (argc == 2 && ((!strcmp(argv[1],"?")) || (!strcmp(argv[1],"/?")) || (!strcmp(argv[1],"-?")) || (!stricmp(argv[1],"/h")) || (!stricmp(argv[1],"-h"))))
   {
      ShowHelp();
      return 0;
   }

   short    iShift = 0;

   if (((argc > 2) && (!stricmp(argv[1],"+t"))) || ((argc > 3) && (!stricmp(argv[2],"+t"))))
   {
      bRemoveTabs = false;
      ++iShift;
   }

   if (((argc > 2) && (!stricmp(argv[1],"-e"))) || ((argc > 3) && (!stricmp(argv[2],"-e"))))
   {
      bRemoveEmptyLines = true;
      ++iShift;
   }

   FindFile   FF;

   FF.SetMask(argv[1 + iShift]);

   int      iTotal = 0;

   while (FF.Fetch())
   {
      if ((FF._w32fd.dwFileAttributes | FILE_ATTRIBUTE_NORMAL) && !(FF._w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      {
         if (Proceed(FF._w32fd.cFileName))
         {
            ++iTotal;
         }
      }
   }

   printf("\n[i]: Processed %d file(s) total.\n",iTotal);

   return 0;
}

/* ******************************************************************** **
** @@                   The End
** ******************************************************************** */
