/************************************************************************
**      RollNo:2018201033          Name : Darshan Kansagara			   **
************************************************************************/

#include "clientheader.h"

#include <openssl/md5.h>
string getmd5(string filepath)
{

    string file_path = filepath;
    ifstream fin(file_path.c_str(), ios::in | ios::binary);
    if (fin.fail())
    {
        return "";
    }
    unsigned char md5[MD5_DIGEST_LENGTH];
    MD5_CTX ctx;
    MD5_Init(&ctx);
    string line;
    while (getline(fin, line))
    {
        MD5_Update(&ctx, line.c_str(), line.size());
    }
    MD5_Final(md5, &ctx);

    string md5_hex;
    const char map[] = "0123456789abcdef";
    for (size_t i = 0; i < MD5_DIGEST_LENGTH; ++i)
    {
        // cout << int(md5[i]) << " ";
        md5_hex += map[md5[i] / 16];
        md5_hex += map[md5[i] % 16];
    }
    // cout <<md5_hex<<endl;
    return md5_hex;
}
//***************************************************************************
// This called mtorrent creator to create mtorrent file make appropiate
// complex data that need to send to tracker for sharing of file
// (command + shorthash(40) + clientsocket + filepath )
//***************************************************************************
string executeshareclient(vector<string> tokens, string clntsckstr, string trcksck1str, string trcksck2str)
{
    string cmd = tokens[0];
    string fpath = tokens[1];
    string mtpath = tokens[2];

    cout << "Command  : " << cmd << endl;
    cout << "File path : " << fpath << endl;
    cout << "File md5 :" << getmd5(fpath) << endl;
    cout << "Mtorrent path : " << mtpath << endl;
    // srand(time(NULL));
    /* 產生亂數 */
    // int x = rand();

    // mtpath += to_string(x);
    char *fp, *tp;
    fp = new char[fpath.length() + 1];
    strcpy(fp, fpath.c_str());

    tp = new char[mtpath.length() + 1];
    strcpy(tp, mtpath.c_str());
    string filehash = createTorrentFile(fp, tp, trcksck1str, trcksck2str);
    if (filehash == "-1")
    {
        writelog("\nError Encounter for creating hash of file in sharing: " + fpath);
        return "-1";
    }
    writelog("share cmd gets Long Hash : " + filehash); // need to do hash of hash
    char *longhash = new char[filehash.length() + 1];
    strcpy(longhash, filehash.c_str());
    string shorthash = calHashofchunk(longhash, filehash.length(), 0);

    string ans = cmd + "#" + shorthash + "#" + clntsckstr + "#" + fpath + "#" + getmd5(fpath);
    writelog("(SHARE cmd)Complex Data need to send to tracker :" + ans);
    return ans;
}