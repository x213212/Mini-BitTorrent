/************************************************************************
**      RollNo:2018201033          Name : Darshan Kansagara			   **
************************************************************************/
#include "clientheader.h"
#include "socket.cpp"
#include <fstream>

#include <openssl/md5.h>
#include <time.h>
// using namespace std;
// using namespace FastCRC;
map<string, int> m;
map<string, string> m2;

struct complexData
{
    char *replydata1, *destpath1, *getcmdmtorrentpath1;
    int sock1;
};

#define CSIZE 512 * 1024

const char *logpath;
string clientsocketstr, trackersocket1str, trackersocket2str;
map<string, string> downloadstatus; // to maintain download status
vector<pair<string, string>> clientfilepath;

//***************************************************************************
// This function is act as leecher to download file from seeders
//***************************************************************************
int merge_binary(string destpath, string cmd5)
{
    char *d_path = new char[destpath.length() + 20];
    strcpy(d_path, destpath.c_str());
    ofstream myfile(d_path, ofstream::binary);
    cout << "====merge binary====" << endl;
    int count_b = 0;
    string download_file_size = destpath.c_str();
    download_file_size += "count";

    for (int i = 0; i < m.find(download_file_size)->second; i++)
    {
        // myfile.write(buffer2, n);
        string s = destpath.c_str();
        string s2;
        char str[20];
        sprintf(str, "%d", i); // integer to string
        std::stringstream ss;
        ss << str;
        ss >> s2;
        s += ".tmp." + s2;
        std::ifstream fin(s.c_str(), ios_base::in | ifstream::binary);
        char *buffer2 = new char[m.find(s)->second];
        if (!fin.is_open())
        {
            cout << "errror" << endl;
        }
        fin.read(buffer2, m.find(s)->second);
        fin.close();
        count_b += m.find(s)->second;
        // ofstream myfile2(s.c_str(), ofstream::binary);
        myfile.write(buffer2, m.find(s)->second);
    }
    cout << count_b << endl;
    cout << "====merge binary ok====" << endl;
    cout << "=====md5 check======" << endl;
    cout << cmd5<<endl;
    // cout << cmd5<<endl;
    cout <<getmd5(destpath) <<endl;
    if (cmd5 == getmd5(destpath))
        cout << "=====md5 check ok======" << endl;
    else
        cout << "=====md5 fail======" << endl;
    myfile.close();
}

int dofiletransfering(string replydata, string destpath, string filesize, string filemd5)
{

    writelog("dofiletransfering called : " + replydata);
    stringstream check1(replydata);
    string intermediate;
    cout << "reply : " << replydata << endl;
    while (getline(check1, intermediate, '@'))
    {
        stringstream check2(intermediate);
        vector<string> clientsocketvc;
        string subintermediate;
        while (getline(check2, subintermediate, '#'))
        {
            clientsocketvc.push_back(subintermediate);
        }
        clientfilepath.push_back(pair<string, string>(clientsocketvc[0], clientsocketvc[1]));
        // if(clientsocketvc[0]!= "")
        // cout << clientsocketvc[0]<<endl;
        // cout << clientsocketvc[2]<<endl;
        m2[clientsocketvc[0]] = clientsocketvc[2];

        // cout << clientsocketvc[2]<<endl;
    }
    // cout <<"qweqwe"<<endl;
    writelog("******Avialable seeders for downloading file*******");
    for (unsigned int i = 0; i < clientfilepath.size(); i++)
    {
        writelog(clientfilepath[i].first + "--->" + clientfilepath[i].second);
    }
    writelog("****************************************************");

    socketclass csocket;
    string filepath;
    int sock = 0;
    char *d_path = new char[destpath.length() + 1];
    char *d_path2 = new char[destpath.length() + 1];
    strcpy(d_path, destpath.c_str());
    strcpy(d_path2, destpath.c_str());

    char *token = strtok(d_path2, "/");
    char *last;
    // loop through the string to extract all other tokens
    while (token != NULL)
    {
        // printf(" %s\n", token); // printing each token
        // if(strtok(NULL, "/"))
        token = strtok(NULL, "/");
        if (token)
            last = token;
    }
    printf(" %s\n", last);
    // cout << token << endl;
    // printf(" hahah%s\n", token);
    for (unsigned int i = 0; i < clientfilepath.size(); i++)
    {
        cout << "serach source..\n"
             << clientfilepath[i].first << " md5:" << m2.find(clientfilepath[i].first)->second << endl;
        // size_t found = clientfilepath[i].second.find(last);

        if ((string)m2.find(clientfilepath[i].first)->second == filemd5)
        {
            sock = 0;
            // cout << "socket : " << clientfilepath[i].first << endl;
            cout << "found same md5 source socket:" << clientfilepath[i].first << " md5:" << m2.find(clientfilepath[i].first)->second << "\n";
            csocket.setsocketdata((clientfilepath[i].first));
            filepath = clientfilepath[i].second;
            struct sockaddr_in serv_addr;
            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                cout << "trying another source.." << endl;
                printf("\n Socket creation error \n");
                continue;
            }

            memset(&serv_addr, '0', sizeof(serv_addr));

            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(csocket.port);

            // Convert IPv4 and IPv6 addresses from text to binary form
            if (inet_pton(AF_INET, csocket.ip, &serv_addr.sin_addr) <= 0)
            {
                printf("\nClient File  : Invalid address/ Address not supported \n");
                continue;
            }

            if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
            {
                printf("\nConnection Failed in Client\n");
                continue;
            }
            cout << "equal\n";
            break;
        }
        else
        {
            continue;
            cout << "not equal\n";
        }

        // if (found != string::npos)
        // {

        // }
        // else
    }
    writelog("******Connection  established for file transffered !!!");

    ofstream myfile(d_path, ofstream::binary);

    cout << d_path << endl;
    // ofstream myfile2(d_path + "tmp", ofstream::binary);

    // dowloading chunk of data from seeder and write it into a file
    char *clientreply = new char[filepath.length() + 1];
    strcpy(clientreply, filepath.c_str());
    send(sock, clientreply, strlen(clientreply), 0);
    int n;
    downloadstatus[destpath] = "D";
    // double keepdownload;
    // keepdownload = getfileprocess("");
    // cout << getfileprocess("") << endl;
    // // cout << getfilesize(filesize) << endl;
    // const char *pFileName = "result.txt";
    // FILE *pFile;
    // pFile = fopen(pFileName, "w");
    // if (NULL == pFile)
    // {
    //     printf("error");
    //     // return;
    // }
    // else
    // {
    //     char answer;
    //     printf("\nWould you like to play? Enter y or n: \n");
    //     // fclose(pFile);
    // }

    double size = getfilesize(filesize);
    // size = size * 1.2;
    // std::map<std::string, int> m;

    float progress = 0.0;
    int count = 0;
    int count_n = 0;
    double speed = 0;
    time_t start, end;
    start = time(NULL);
    do
    {
        string s = destpath.c_str();
        string s2;
        char str[20];
        sprintf(str, "%d", count); // integer to string
        std::stringstream ss;
        ss << str;
        ss >> s2;
        s += ".tmp." + s2;

        ofstream chunk(s.c_str(), ofstream::binary);
        // cout << d_path << endl;

        char *buffer = new char[CSIZE];
        // usleep(100000); // test speed
        n = read(sock, buffer, CSIZE);
        speed += (n / 1024.0) / 1024.0;
        end = time(NULL);
        double diff = difftime(end, start);

        m[s] = n;
        count_n += n;

        chunk.write(buffer, n);

        int barWidth = 70;

        cout << "[";
        int pos = barWidth * progress;
        for (int i = 0; i < barWidth; ++i)
        {
            if (i < pos)
                cout << "=";
            else if (i == pos)
                cout << ">";
            else
                cout << " ";
        }

        cout << "] " << speed << "\t(mb)/s ." << int(progress * 100.0) << "% \r";
        if (diff >= 1)
        {
            start = end = speed = 0;
            start = time(NULL);
        }
        cout.flush();
        progress += (double)((n / size)); // for demonstration only
        // if ((keepdownload <= (double)progress) || (!keepdownload))
        // myfile.write(buffer, n);
        // fprintf(pFile, "%f:%f:%f\n",  (double)progress,(double)(progress * 100.0),(double)((n / size)));
        // if(keepdownload <=(double)progress )
        // fprintf(pFile, "%f\n", (double)progress);
        // fprintf(pFile, "%f\n",  (double)(progress * 100.0));
        // cout << (double)((n / size)) << endl;
        // sleep(1);
        // cout << progress << endl;
        count++;
        chunk.close();
    } while (n > 0);
    string download_file_size = destpath.c_str();
    download_file_size += "count";
    m[download_file_size] = count;
    cout << count_n << endl;

    // fclose(pFile);

    /// merge bin

    // ofstream outFile(d_path, ofstream::binary);

    // if (outFile.fail())
    // {
    //     cerr << "Unable to open file for writing." << endl;
    //     exit(1);
    // }
    // string s3 = destpath.c_str();
    // s3 += ".tmp.0";
    // char *buffer2 = new char[CSIZE];
    // cout << s3.c_str();
    // std::ifstream fin(s3.c_str(), ios_base::in | ifstream::binary);
    // if (!fin.is_open())
    // {
    //     cout << "errror" << endl;
    // }
    // fin.read(buffer2, 64 * 1024);
    // fin.close();
    // cout << "merge binary" << endl;
    // int count_b = 0;
    // for (int i = 0; i < count; i++)
    // {
    //     // myfile.write(buffer2, n);
    //     string s = destpath.c_str();
    //     string s2;
    //     char str[20];
    //     sprintf(str, "%d", i); // integer to string
    //     std::stringstream ss;
    //     ss << str;
    //     ss >> s2;
    //     s += ".tmp." + s2;
    //     std::ifstream fin(s.c_str(), ios_base::in | ifstream::binary);
    //     // int length = fin.tellg();
    //     // struct stat sb;
    //     // if (stat(s.c_str(), &sb) == -1)
    //     // {
    //     //     cout << s.c_str() << endl;
    //     //     cout << "FILE NOT FOUND" << endl;
    //     //     // return "-1";
    //     // }

    //     // cout << sb.st_size << endl;
    //     //  m.find(s->second);
    //     char *buffer2 = new char[m.find(s)->second];

    //     if (!fin.is_open())
    //     {
    //         cout << "errror" << endl;
    //     }
    //     fin.read(buffer2, m.find(s)->second);
    //     fin.close();
    //     count_b += m.find(s)->second;
    //     // ofstream myfile2(s.c_str(), ofstream::binary);
    //     myfile.write(buffer2, m.find(s)->second);
    // }
    // cout << count_b << endl;
    // cout << count_n << endl;
    // myfile.close();
    // std::ofstream input(d_path + ".tmp.0", std::ios::binary);

    // copies all data into buffer
    // std::vector<unsigned char> buffer2(std::ostreambuf_iterator<char>(input), {});
    // string s4 = destpath.c_str();
    // s4 += ".new";
    // cout << s4.c_str();
    // ofstream outFile(s4.c_str(), ofstream::binary );
    // if (outFile.fail())
    // {
    //     cerr << "Unable to open file for writing." << endl;
    //     exit(1);
    // }
    // outFile.write(buffer2,  64 * 1024);
    // outFile.close();
    return 1;
}

//*********************************************************************************
// This function is used to handle whether a file successfully downloaded or not
//*********************************************************************************
void *getcommandExecution(void *complexstruct)
{
    struct complexData obj = *(struct complexData *)complexstruct;
    string replydata = string(obj.replydata1);
    string destpath = string(obj.destpath1);
    string getcmdmtorrentpath = string(obj.getcmdmtorrentpath1);
    string cmd5 = getfilemd5(obj.getcmdmtorrentpath1);

    // cout << getfilemd5(obj.getcmdmtorrentpath1)<<endl;
    // cout<<"Reply data : "<<replydata<<endl;
    cout << "destpath : " << destpath << endl;
    // cout<<"getcmdtorrentpath : "<<getcmdmtorrentpath<<endl;
    int sock = obj.sock1;
    // cout << getfilesize(getcmdmtorrentpath) << endl;
    int ans = dofiletransfering(replydata, destpath, getcmdmtorrentpath, cmd5);
    if (ans == 1)
    {
        downloadstatus[destpath] = "S";
        vector<string> temptokens;
        temptokens.push_back("share");
        temptokens.push_back(destpath);
        srand(time(NULL));
        /* 產生亂數 */
        int x = rand();

        getcmdmtorrentpath += to_string(x);
        temptokens.push_back(getcmdmtorrentpath);
        merge_binary(destpath, cmd5);
        string complexdata = executeshareclient(temptokens, clientsocketstr, trackersocket1str, trackersocket2str);
        if (complexdata != "-1")
        {
            char *clt = new char[complexdata.length() + 1];
            strcpy(clt, complexdata.c_str());
            send(sock, clt, strlen(clt), 0);
            char buff[1024] = {0};
            read(sock, buff, 1024);
        }
        // merge_binary(destpath);
        cout << "FILE SUCCESSFULLY DOWNLOADED" << endl;
    }
    else
    {
        cout << "ERROR IN DOWNLOADING FILE" << endl;
    }

    return complexstruct;
}

//***************************************************************************
// When client start it read all mtorrent file from current folder and
// share its data with tracker automatically
//***************************************************************************
void readallmtorrentfile(int sc)
{
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            string mtfile = string(dir->d_name);
            string fileextention = ".mtorrent";
            if (mtfile.find(fileextention) != string::npos)
            {
                ifstream fileptr(mtfile, ifstream::binary);
                int count = 4;
                string linecontent;
                string fpath;
                while ((count > 0) && getline(fileptr, linecontent))
                {
                    if (count == 2)
                        fpath = linecontent;
                    count--;
                }
                getline(fileptr, linecontent);
                string mtorrenthash = linecontent;
                fileptr.close();

                char *longhash = new char[mtorrenthash.length() + 1];
                strcpy(longhash, mtorrenthash.c_str());
                string shorthash = calHashofchunk(longhash, mtorrenthash.length(), 0);
                string ans = "share#" + shorthash + "#" + clientsocketstr + "#" + fpath + "#" + getmd5(fpath);
                char *clientrply = new char[ans.length() + 1];
                strcpy(clientrply, ans.c_str());
                send(sc, clientrply, strlen(clientrply), 0);
                char buffer[1024] = {0};
                read(sc, buffer, 1024);
                writelog("read mtorrentfile sent to server & got response : " + string(buffer));
            }
        }
        closedir(d);
    }
}

vector<string> stringProcessing(string command, char delimeter)
{
    vector<string> temptokens;
    string token = "";
    for (unsigned int i = 0; i < command.length(); i++)
    {
        char ch = command[i];
        if (ch == '\\')
        {
            i++;
            token += command[i];
        }
        else if (ch == delimeter)
        {
            temptokens.push_back(token);
            token = "";
        }
        else
        {
            token += ch;
        }
    }
    temptokens.push_back(token);
    return temptokens;
}

//***************************************************************************
// This is main client function from where client starts and comminicate
// with tracker via socket programming
//***************************************************************************
int main(int argc, char const *argv[])
{

    socketclass clientsocket;
    socketclass trackersocket1;
    socketclass trackersocket2;
    if (argc != 5)
    {
        cout << "Invalid Argument in client!!!" << endl;
    }
    else
    {
        clientsocketstr = string(argv[1]);
        trackersocket1str = string(argv[2]);
        trackersocket2str = string(argv[3]);
        clientsocket.setsocketdata(clientsocketstr);
        trackersocket1.setsocketdata(trackersocket1str);
        trackersocket2.setsocketdata(trackersocket2str);
        logpath = argv[4];
        ofstream myfile(logpath, std::ios_base::out);
        myfile.close();
        writelog("********new client started *********");
        writelog("Client socket : " + clientsocketstr);
        writelog("Tracker 1 socket : " + trackersocket1str);
        writelog("Tracker 2 socket : " + trackersocket2str);
        // cout<<"Tracker 1 socket: "<<trackersocket1.ip<<" : "<<trackersocket1.port<<endl;
        // cout<<"Tracker 2 socket: "<<trackersocket2.ip<<" : "<<trackersocket2.port<<endl;

        // create new thread from client which act as seeder(server) to provide data to others
        pthread_t cserverid;
        if (pthread_create(&cserverid, NULL, seederserverservice, (void *)&clientsocketstr) < 0)
        {
            perror("\ncould not create thread in client side\n");
        }

        // socket programming on client side
        int sock = 0;
        struct sockaddr_in serv_addr;
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("\n Socket creation error in client side\n");
            return -1;
        }

        memset(&serv_addr, '0', sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(trackersocket1.port);

        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, trackersocket1.ip, &serv_addr.sin_addr) <= 0)
        {
            printf("\nClient File  : Invalid address/ Address not supported \n");
            return -1;
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            printf("\nConnection Failed in client side\n");
            return -1;
        }
        writelog("******Connection stablished successfully with tracker!!!");
        readallmtorrentfile(sock);

        // continuously listening to client for his entring command
        while (1)
        {

            int getflag = 0, closeflag = 0;
            char *mtorrentfilepath;
            string strcmd, destpath, getcmdmtorrentpath;

            // writelog("Enter the command : ");
            getline(cin >> ws, strcmd);
            writelog("Command from cient : " + strcmd);

            vector<string> tokens = stringProcessing(strcmd, ' ');
            string complexdata;

            // To handle which command enter by client
            if (tokens[0] == "share")
            {
                if (tokens.size() != 3)
                {
                    cout << "INVALID_ARGUMENTS --- SHARE Command" << endl;
                    continue;
                }
                writelog("SHARE command exe in client side");
                complexdata = executeshareclient(tokens, clientsocketstr, trackersocket1str, trackersocket2str);
                if (complexdata == "-1")
                    continue;
            }
            else if (tokens[0] == "get")
            {
                if (tokens.size() != 3)
                {
                    cout << "INVALID_ARGUMENTS --- GET Command" << endl;
                    continue;
                }
                writelog("GET command exe in client side");
                complexdata = executegetclient(tokens);
                destpath = tokens[2];
                getcmdmtorrentpath = tokens[1];

                if (complexdata == "-1")
                    continue;
                else
                {
                    getflag = 1;
                }
            }
            else if (tokens[0] == "remove")
            {
                if (tokens.size() != 2)
                {
                    cout << "INVALID_ARGUMENTS --- REMOVE Command" << endl;
                    continue;
                }
                mtorrentfilepath = new char[tokens[1].length() + 1];
                strcpy(mtorrentfilepath, tokens[1].c_str());
                writelog("REMOVE command exe in client side");
                complexdata = executeremoveclient(tokens, clientsocketstr);
                if (complexdata == "-1")
                    continue;
            }
            else if (tokens[0] == "show_downloads")
            {
                if (downloadstatus.empty())
                {
                    cout << "NO DOWNLOADS TILL NOW" << endl;
                }
                else
                {
                    cout << "********* DOWNLOADS **********" << endl;
                    for (auto item : downloadstatus)
                    {
                        cout << item.second << " : " << item.first << endl;
                    }
                }
                continue;
            }
            else if (tokens[0] == "close")
            {
                writelog("CLOSE command exe in client side");
                complexdata = "close#" + clientsocketstr;
                closeflag = 1;
            }
            else
            {
                cout << "INVALID COMMAND" << endl;
                continue;
            }

            char *clientreply = new char[complexdata.length() + 1];
            strcpy(clientreply, complexdata.c_str());
            // cout<<"clientreply : "<<clientreply<<endl;

            // to send client request to tracker
            send(sock, clientreply, strlen(clientreply), 0);

            writelog("client msg message sent to Tracker");

            // to recieve tracker responce
            char buffer[1024] = {0};
            read(sock, buffer, 1024);
            writelog("client(" + clientsocketstr + ")got reply from tracker ===> " + string(buffer));

            if (getflag != 1)
                cout << string(buffer) << endl;

            string responce = string(buffer);

            // when getting response of get command as list of  client socket having file
            if (getflag == 1)
            {
                struct complexData obj1;
                obj1.replydata1 = new char[responce.length() + 1];
                strcpy(obj1.replydata1, responce.c_str());
                obj1.destpath1 = new char[destpath.length() + 1];
                strcpy(obj1.destpath1, destpath.c_str());
                obj1.getcmdmtorrentpath1 = new char[getcmdmtorrentpath.length() + 1];
                strcpy(obj1.getcmdmtorrentpath1, getcmdmtorrentpath.c_str());
                obj1.sock1 = sock;

                // for non-blocking get command (create separate thread for downloading file)
                pthread_t getclientid;
                if (pthread_create(&getclientid, NULL, getcommandExecution, (void *)&obj1) < 0)
                {
                    perror("\ncould not create thread in client side\n");
                }
            }
            getflag = 0;

            // When Server Send Response for remove command
            if (responce == "FILE SUCCESSFULLY REMOVED")
            {
                if (remove(mtorrentfilepath) != 0)
                    perror("\nError deleting mtorrent file\n");
            }

            // When Client is closed
            if (closeflag == 1)
            {
                cout << "Thank You !!!" << endl;
                close(sock);
                break;
            }
        }
    }

    return 0;
}
