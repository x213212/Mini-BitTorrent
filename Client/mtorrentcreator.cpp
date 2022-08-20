/************************************************************************
**      RollNo:2018201033          Name : Darshan Kansagara			   **
************************************************************************/

#include "clientheader.h"

#define CSIZE 512 * 1024

char *filepath, *mtorrentpath;

//***************************************************************************
// It calculate hash(SHA1) of given data chunk
// shorthashflag = 1 means return 20 character of hash
// shorthashflag = 0 means return all 40 character of hash
//***************************************************************************
string calHashofchunk(char *schunk, int length1, int shorthashflag)
{

    unsigned char hash[SHA_DIGEST_LENGTH];
    char buf[SHA_DIGEST_LENGTH * 2];
    SHA1((unsigned char *)schunk, length1, hash);

    // printf("\n*****hash ********");
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
        sprintf((char *)&(buf[i * 2]), "%02x", hash[i]);

    // cout<<"hash : "<<buf<<endl;
    string ans;
    if (shorthashflag == 1)
    {
        for (int i = 0; i < 20; i++)
        {
            ans += buf[i];
        }
    }
    else
    {
        for (int i = 0; i < SHA_DIGEST_LENGTH * 2; i++)
        {
            ans += buf[i];
        }
    }
    return ans;
}

//***************************************************************************
// It returns combined hash of all chunk for any file (20+20+20...)
//***************************************************************************
string getFileHash(char *fpath)
{
    string fileHash;
    ifstream file1(fpath, ifstream::binary);

    /* basic sanity check */
    if (!file1)
    {
        cout << "FILE DOES NOT EXITST : " << string(fpath) << endl;
        return "-1";
    }

    struct stat fstatus;
    stat(fpath, &fstatus);

    // Logic for deviding file1 into chunks
    long int total_size = fstatus.st_size;
    long int chunk_size = CSIZE;

    int total_chunks = total_size / chunk_size;
    int last_chunk_size = total_size % chunk_size;

    if (last_chunk_size != 0) // if file1 is not exactly divisible by chunks size
    {
        ++total_chunks; // add last chunk to count
    }
    else // when file1 is completely divisible by chunk size
    {
        last_chunk_size = chunk_size;
    }

    // loop to getting each chunk
    for (int chunk = 0; chunk < total_chunks; ++chunk)
    {
        int cur_cnk_size;
        if (chunk == total_chunks - 1)
            cur_cnk_size = last_chunk_size;
        else
            cur_cnk_size = chunk_size;

        char *chunk_data = new char[cur_cnk_size];
        file1.read(chunk_data,    /* address of buffer start */
                   cur_cnk_size); /* this many bytes is to be read */

        string sh1out = calHashofchunk(chunk_data, cur_cnk_size, 1);
        fileHash = fileHash + sh1out;
    }

    return fileHash;
}

//***************************************************************************
// This function create mtorrent file by adding all information into it
//***************************************************************************
string createTorrentFile(char *fpath, char *mtpath, string tcksocket1, string tcksocket2)
{
    writelog("Mtorrent creater called for filepath : " + string(fpath));
    struct stat sb;
    if (stat(fpath, &sb) == -1)
    {
        cout << "FILE NOT FOUND" << endl;
        return "-1";
    }

    ofstream myfile;
    myfile.open(string(mtpath));
    myfile << tcksocket1 << endl;
    myfile << tcksocket2 << endl;
    myfile << string(fpath) << endl;
    myfile << "md5:" << getmd5(fpath) << endl;
    myfile << "size:" << sb.st_size << endl;
    string flhash = getFileHash(fpath);
    myfile << flhash << endl;
    myfile.close();
    writelog("Mtorrent Succesfully created");
    return flhash;
}
char *strrpc(char *str, char *oldstr, char *newstr)
{
    char bstr[strlen(str)]; //轉換緩衝區
    memset(bstr, 0, sizeof(bstr));

    for (int i = 0; i < strlen(str); i++)
    {
        if (!strncmp(str + i, oldstr, strlen(oldstr)))
        { //查詢目標字串
            strcat(bstr, newstr);
            i += strlen(oldstr) - 1;
        }
        else
        {
            strncat(bstr, str + i, 1); //儲存一位元組進緩衝區
        }
    }

    strcpy(str, bstr);
    return str;
}
int charToInt(char c)
{
    int num = 0;

    // Substract '0' from entered char to get
    // corresponding digit
    num = c - '0';

    return num;
}
int getfilesize(string tmp2)
{
    string tmp = "./";
    tmp += tmp2;
    int n = tmp.length();

    // declaring character array
    char filename[n + 1];

    // copying the contents of the
    // string to char array
    strcpy(filename, tmp.c_str());
    cout << tmp << endl;
    // char filename[] = tmp; //文件名
    FILE *fp;
    char StrLine[1024];                      //每行最大读取的字符数
    if ((fp = fopen(filename, "r")) == NULL) //判断文件是否存在及可读
    {
        printf("error!");
        return -1;
    }
    // char str1[] = "abcd", str2[] = "abCd", str3[] = "size";
    // int result;
    char *pch;
    char *pch2;
    int chartonum;
    while (!feof(fp))
    {

        fgets(StrLine, 1024, fp); //读取一行

        // printf("%s\n", StrLine); //输出
        pch = strstr(StrLine, "size:");
        if (pch != NULL)
        {

            // printf("found: %s\n", pch);
            strrpc(StrLine, "size:", "");
            // printf("%s\n", StrLine); //输出
            // char *pChar = &StrLine;
            chartonum = atoi(StrLine);

            // printf("%s\n", pch2);    //输出
        }
        else
            continue;
    }
    fclose(fp); //关闭文件
    return chartonum;
}


string getfilemd5(string tmp)
{
    // string tmp = "./";
    // tmp += tmp2;
    int n = tmp.length();

    // declaring character array
    char filename[n + 1];

    // copying the contents of the
    // string to char array
    strcpy(filename, tmp.c_str());
    cout << tmp << endl;
    // char filename[] = tmp; //文件名
    FILE *fp;
    char StrLine[1024];                      //每行最大读取的字符数
    if ((fp = fopen(filename, "r")) == NULL) //判断文件是否存在及可读
    {
        printf("get file md5 error!");
        // return -1;
    }
    // char str1[] = "abcd", str2[] = "abCd", str3[] = "size";
    // int result;
    char *pch;
    char *pch2;
    string cmd5;
    while (!feof(fp))
    {

        fgets(StrLine, 1024, fp); //读取一行

        // printf("%s\n", StrLine); //输出
        pch = strstr(StrLine, "md5:");
        if (pch != NULL)
        {

            // printf("found: %s\n", pch);
            strrpc(StrLine, "md5:", "");
            strrpc(StrLine, "\n", "");
            // printf("%s\n", StrLine); //输出
            // char *pChar = &StrLine;
            cmd5 = (StrLine);

            // printf("%s\n", pch2);    //输出
        }
        else
            continue;
    }
    fclose(fp); //关闭文件
    
    return cmd5;
}

double getfileprocess(string tmp)
{
    char filename[] = "/root/x21321219/Mini-BitTorrent/Client/result.txt"; //文件名
    FILE *fp;
    char StrLine[1024];                      //每行最大读取的字符数
    if ((fp = fopen(filename, "r")) == NULL) //判断文件是否存在及可读
    {
        printf("error!");
        return -1;
    }
    // char str1[] = "abcd", str2[] = "abCd", str3[] = "size";
    // int result;
    char *pch;
    char *pch2;
    double chartonum = atof(StrLine);
    while (!feof(fp))
    {

        fgets(StrLine, 1024, fp); //读取一行
        chartonum = (chartonum >= atof(StrLine)) ? chartonum : atof(StrLine);
        // printf("%s\n", StrLine); //输出
        // pch = strstr(StrLine, "size:");
        // if (pch != NULL)
        // {

        //     // printf("found: %s\n", pch);
        //     strrpc(StrLine, "size:", "");
        //     // printf("%s\n", StrLine); //输出
        //     // char *pChar = &StrLine;
        //     int chartonum =atoi(StrLine);
        //     return chartonum;
        //     // printf("%s\n", pch2);    //输出
        // }
        // else
        //     continue;
    }
    // double chartonum = atof(StrLine);
    printf("%s\n", StrLine); //输出
    fclose(fp);              //关闭文件
    return chartonum;
}