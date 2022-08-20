/************************************************************************
**      RollNo:2018201033          Name : Darshan Kansagara			   **
************************************************************************/

//**********************************************************************
//class to maintain Tracker Data
//**********************************************************************
#include "trackerheader.h"

class trackerdata
{
  public:
    string shash;
    string csocket;
    string cfpath;
    string cmd5;

    trackerdata()
    {
        shash = "";
        csocket = "";
        cfpath = "";
        cmd5 ="";
    }

    trackerdata(string hash, string ipport, string path,string md5)
    {
        shash = hash;
        csocket = ipport;
        cfpath = path;
        cmd5 =md5;
    }
    trackerdata(string hash, string ipport, string path)
    {
        shash = hash;
        csocket = ipport;
        cfpath = path;
        // cmd5 =md5;
    }
};
