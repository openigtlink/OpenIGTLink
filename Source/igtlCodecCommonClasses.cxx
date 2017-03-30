//
//  CodecCommonClasses.cpp
//  OpenIGTLink
//
//  Created by Longquan Chen on 3/30/17.
//
//

#include "igtlCodecCommonClasses.h"


ReadConfigFile::ReadConfigFile()
: configFile(NULL)
, configFileName ("")
, lineNums(0) {
}

ReadConfigFile::ReadConfigFile (const char* fileName)
: configFile (0)
, configFileName (fileName)
, lineNums (0) {
  if (strlen (fileName) > 0) {
    configFile = fopen (fileName, "r");
  }
}

ReadConfigFile::ReadConfigFile (const std::string& fileName)
: configFile (0)
, configFileName (fileName.c_str())
, lineNums (0) {
  if (strlen (fileName.c_str()) > 0) {
    configFile = fopen (fileName.c_str(), "r");
  }
}

ReadConfigFile::~ReadConfigFile() {
  if (configFile) {
    fclose (configFile);
    configFile = NULL;
  }
}

void ReadConfigFile::OpenFile (const char* strFile) {
  if (strFile != NULL && strlen (strFile) > 0) { // confirmed_safe_unsafe_usage
    configFileName = strFile;
    configFile = fopen (strFile, "r");
  }
}

long ReadConfigFile::ReadLine (std::string* var, const int valSize/* = 4*/) {
  if (configFile == NULL || var == NULL || valSize <= 1)
    return 0;
  
  std::string* strTags = &var[0];
  int nTagNum = 0, n = 0;
  bool bCommentFlag = false;
  
  while (n < valSize) {
    var[n] = "";
    ++ n;
  }
  do {
    const char singleChar = (char)fgetc (configFile);
    
    if (singleChar == '\n' || feof (configFile)) {
      ++ lineNums;
      break;
    }
    if (singleChar == '#')
      bCommentFlag = true;
    if (!bCommentFlag) {
      if (singleChar == '\t' || singleChar == ' ') {
        if (nTagNum >= valSize-1)
          break;
        if (! (*strTags).empty()) {
          ++ nTagNum;
          strTags = &var[nTagNum];
        }
      } else
        *strTags += singleChar;
    }
    
  } while (true);
  
  return 1 + nTagNum;
}

const bool ReadConfigFile::EndOfFile() {
  if (configFile == NULL)
    return true;
  return feof (configFile) ? true : false;
}

const int ReadConfigFile::GetLines() {
  return lineNums;
}

const bool ReadConfigFile::ExistFile() {
  return (configFile != NULL);
}

const std::string& ReadConfigFile::GetFileName() {
  return configFileName;
}
