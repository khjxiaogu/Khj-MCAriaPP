#if WIN32
//win32 ssl and boost setup
#define BOOST_ASIO_ENABLE_CANCELIO
#define BOOST_USE_WINDOWS_H
#pragma comment(lib,"Crypt32.lib")
#pragma comment(lib,"libssl32MT.lib")
#pragma comment(lib,"libcrypto32MT.lib")
#pragma comment(lib,"legacy_stdio_definitions.lib")
#if _MSC_VER>=1900
#include "stdio.h" 
_ACRTIMP_ALT FILE* __cdecl __acrt_iob_func(unsigned);
#ifdef __cplusplus 
//for legacy support
extern "C"
#endif 
FILE * __cdecl __iob_func(unsigned i) {
    return __acrt_iob_func(i);
}
#endif
#endif

#include "client_https.hpp"
#include "CPConv.h"
#include<iostream>
#include <fstream>
#include "json.h"
std::wstring readHtml(const char* domain, const char* path) {
    SimpleWeb::Client<SimpleWeb::HTTPS> httpsclient(domain, false);
    std::string str;
    try {
        str = httpsclient.request("GET", path)->content.string();
    }catch(boost::system::system_error er){
        std::cout << er.code() << "  " << er.what();
        exit(0);
    }
    wchar_t* wcs = UTF82WCS(str.c_str());
    std::wstring ret(wcs);
    delete[] wcs;
    return ret;
}
std::wstring SplitMavenFile(std::wstring s) {
    int l = s.substr(0, s.find_first_of(L"-")).length();
    return s.substr(s.find_first_of(L"-") + 1, s.find_last_of(L"-") - l - 1);
}
void GetProjectBranch(std::string& repo, std::string& proj)
{
    variant JSON;
    std::string domain;
    std::string path;
    std::string file;
    if (repo == "PG-Via") {
        JSON = evalJson(readHtml("ci.viaversion.com", std::string("/job/" + proj + "/lastSuccessfulBuild/api/json").c_str()));
        domain = "ci.viaversion.com";
        path="/job/" + proj + "/lastSuccessfulBuild/artifact/" + UnicodeToAnsi(JSON.getObject()[L"artifacts"].getArray()[0].getObject()[L"relativePath"].getString().c_str());
        file = UnicodeToAnsi(JSON.getObject()[L"artifacts"].getArray()[0].getObject()[L"fileName"].getString().c_str());
    }
    else if (repo == "P-LuckPerms") {
        JSON = evalJson(readHtml("ci.lucko.me", "/job/LuckPerms/lastSuccessfulBuild/api/json").c_str());
        auto arts = JSON.getObject()[L"artifacts"].getArray();
        domain = "ci.lucko.me";
        for (auto it = arts.begin(); it < arts.end(); it++) {
            file = UnicodeToAnsi(SplitMavenFile(it->getObject()[L"fileName"].getString().c_str()).c_str());
            if (proj == file.c_str()) {
                 path = "/job/LuckPerms/lastSuccessfulBuild/artifact/" + UnicodeToAnsi(it->getObject()[L"relativePath"].getString().c_str());
                 break;
             }
        }
    }
    else if (repo == "PG-BentoBoxWorld") {
        JSON = evalJson(readHtml("ci.codemc.org",("/job/BentoBoxWorld/job/" + proj + "/lastSuccessfulBuild/api/json").c_str()));
        domain = "ci.codemc.org";
        path = "/job/BentoBoxWorld/job/" + proj + "/lastSuccessfulBuild/artifact/" + UnicodeToAnsi(JSON.getObject()[L"artifacts"].getArray().back().getObject()[L"relativePath"].getString().c_str());
        file = UnicodeToAnsi(JSON.getObject()[L"artifacts"].getArray().back().getObject()[L"fileName"].getString().c_str());
    }
    else if (repo == "S-Paper") {
        JSON = evalJson(readHtml("papermc.io",("/api/v1/paper/" + proj + "/latest").c_str()));
        domain = "papermc.io";
        path = "/api/v1/paper/" + proj + "/latest/download";
        file = "paper-" + UnicodeToAnsi(JSON.getObject()[L"build"].getString().c_str()) + ".jar";
    }
    else if (repo == "S-Travertine") {
        JSON = evalJson(readHtml("papermc.io","/ci/job/Travertine/lastSuccessfulBuild/api/json"));
        domain = "papermc.io";
        auto arts = JSON.getObject()[L"artifacts"].getArray();
        for (auto it = arts.begin(); it < arts.end(); it++) {
            file = UnicodeToAnsi(it->getObject()[L"fileName"].getString().c_str());
            if (proj == file.c_str()) {
                path = "/ci/job/Travertine/lastSuccessfulBuild/artifact/"+ UnicodeToAnsi(it->getObject()[L"relativePath"].getString().c_str());
                break;
            }
        }
    }
    else if (repo == "S-Waterfall") {
        JSON = evalJson(readHtml("papermc.io","/ci/job/Waterfall/lastSuccessfulBuild/api/json"));
        domain = "papermc.io";
        auto arts = JSON.getObject()[L"artifacts"].getArray();
        for (auto it = arts.begin(); it < arts.end(); it++) {
            file = UnicodeToAnsi(it->getObject()[L"fileName"].getString().c_str());
            if (proj == file.c_str()) {
                path = "/ci/job/Waterfall/lastSuccessfulBuild/artifact/" + UnicodeToAnsi(it->getObject()[L"relativePath"].getString().c_str());
                break;
            }
        }
    }
    else if (repo == "S-Mohist") {
        JSON = evalJson(readHtml("ci.codemc.org",("/job/Mohist-Community/job/" + proj + "/lastSuccessfulBuild/api/json").c_str()));
        domain="ci.codemc.org"; //ci.codemc.org/job/Mohist-Community/job/Mohist-1.12.2/lastSuccessfulBuild/api/json
        path = "/job/Mohist-Community/job/" + proj + "/lastSuccessfulBuild/artifact/" + UnicodeToAnsi(JSON.getObject()[L"artifacts"].getArray().back().getObject()[L"relativePath"].getString().c_str());
        file = UnicodeToAnsi(JSON.getObject()[L"artifacts"].getArray().back().getObject()[L"relativePath"].getString().c_str());
    }
    else if (repo == "S-BungeeCord") {
        JSON = evalJson(readHtml("ci.md-5.net","/job/BungeeCord//lastSuccessfulBuild/api/json"));
        domain = "ci.md-5.net";
        auto arts = JSON.getObject()[L"artifacts"].getArray();
        for (auto it = arts.begin(); it < arts.end(); it++) {
            file = UnicodeToAnsi(it->getObject()[L"fileName"].getString().c_str());
            if (proj == file.c_str()) {
                path = "/job/BungeeCord//lastSuccessfulBuild/artifact/" + UnicodeToAnsi(it->getObject()[L"relativePath"].getString().c_str());
                break;
            }
        }
    }
    else if (repo == "PG-ProtocolLib") {
        if (proj == "ProtocolLib-Server-1.7.10") {
            domain = "github.com";
            path = "/dmulloy2/ProtocolLib/releases/download/3.7.0/ProtocolLib.jar";
            file = "ProtocolLib.jar";
        }
        else if (proj == "ProtocolLib-Latest") {
            JSON = evalJson(readHtml("ci.dmulloy2.net","/job/ProtocolLib/lastSuccessfulBuild/api/json"));
            domain = "ci.dmulloy2.net";
            path = "/job/ProtocolLib/lastSuccessfulBuild/artifact/" + UnicodeToAnsi(JSON.getObject()[L"artifacts"].getArray().back().getObject()[L"relativePath"].getString().c_str());
            file = UnicodeToAnsi(JSON.getObject()[L"artifacts"].getArray().back().getObject()[L"fileName"].getString().c_str());
        }
    }
    SimpleWeb::Client<SimpleWeb::HTTPS> downloader(domain,false);
    std::cout << "Ready to download" + repo + "/" + proj + " at " + file + " press any key to continue;press Ctrl+C to cancel\n";
    int u=getchar();
    std::cout << "Downloading... Please wait...\n";
    std::ofstream f(file);
    auto response = downloader.request("GET", path);
    
    f << response->content.string();
    std::cout << "download completed!";
}
const char* projectlist="list of available projects:\n";
int main(int argc, char* argv[]) {
    std::cout << "welcome to MCAriaPP by khjxiaogu\n";
    std::cout << "list of available repositories:\n";
    std::cout << "PG-Via;\nPG-Vle;\nPG-AuthMe;\nPG-BentoBoxWorld;\nP-LuckPerms;\nP-PlotSquared;\nP-FastAsyncWorldEdit;\nS-Paper;\nS-Mohist;\nS-BungeeCord;\nS-Waterfall;\nS-Travertine;\nSG-OneKeyDownload;\n";
    char repo[100];
    std::cout << "select a repository from above:\n";
    std::cin >> repo;
    std::string repostr(repo);
    variant JSON;
    std::cout << "fetching available projects...\n";
    if (repostr == "PG-Via") {
        JSON = evalJson(readHtml("ci.viaversion.com", "/api/json").c_str());
        std::cout << projectlist;
        auto jobs = JSON.getObject()[L"jobs"].getArray();
        for (auto it = jobs.begin(); it < jobs.end(); it++) {
            auto str = it->getObject()[L"name"].getString();
            if (str.find_first_of(L"Via") == 0) {
                auto rep = UnicodeToAnsi(str.c_str());
                std::cout << rep << "\n";
            }
        }
    }
    else
    if (repostr == "P-LuckPerms") {
        JSON = evalJson(readHtml("ci.lucko.me", "/job/LuckPerms/lastSuccessfulBuild/api/json").c_str());
        std::cout << projectlist;
        auto arts = JSON.getObject()[L"artifacts"].getArray();
        for (auto it = arts.begin(); it < arts.end(); it++) {
            auto rep = UnicodeToAnsi(SplitMavenFile(it->getObject()[L"fileName"].getString()).c_str());
            std::cout << rep << "\n";
        }
    }
    else
    if (repostr == "PG-AuthMe") {
     JSON = evalJson(readHtml("ci.codemc.org", "/job/AuthMe/api/json"));
     std::cout << projectlist;
    }
    else
    if (repostr == "PG-BentoBoxWorld") {
        JSON = evalJson(readHtml("ci.codemc.org", "/job/BentoBoxWorld/api/json"));
        std::cout << projectlist;
        auto jobs = JSON.getObject()[L"jobs"].getArray();
        for (auto it = jobs.begin(); it < jobs.end(); it++) {
            auto rep = UnicodeToAnsi(it->getObject()[L"name"].getString().c_str());
            std::cout << rep << "\n";
        }
    }
    else if (repostr == "PG-ProtocolLib") {
        std::cout<<"ProtocolLib-Server-1.7.10\nProtocolLib-Latest\n";
    }
    else
    if (repostr == "P-PlotSquared") {
        JSON = evalJson(readHtml("ci.athion.net","/job/PlotSquared-Breaking/lastSuccessfulBuild/api/json"));
        std::cout << projectlist;
    }
    else
    if (repostr == "P-FastAsyncWorldEdit") {
       // JSON = evalJson(readHtml(""));
        std::cout << projectlist;
    }
    else if (repostr == "S-Mohist") {
        JSON = evalJson(readHtml("ci.codemc.io","/job/Mohist-Community/api/json"));
        std::cout << projectlist;
        auto jobs = JSON.getObject()[L"jobs"].getArray();
        for (auto it = jobs.begin(); it < jobs.end(); it++) {
            auto rep = UnicodeToAnsi(it->getObject()[L"name"].getString().c_str());
            std::cout << rep << "\n";
        }
    }else
    if (repostr == "S-Paper") {
        JSON = evalJson(readHtml("papermc.io","/api/v1/paper/"));
        std::cout << projectlist;
        auto vers = JSON.getObject()[L"versions"].getArray();
        for (auto it = vers.begin(); it < vers.end(); it++) {
            auto str = it->getString();
            auto rep = UnicodeToAnsi(str.c_str());
            std::cout << rep << "\n";
        }
    }
    else
    if (repostr == "S-BungeeCord") {
        JSON = evalJson(readHtml("ci.md-5.net","/job/BungeeCord/lastSuccessfulBuild/api/json"));
        std::cout << projectlist;
        auto arts = JSON.getObject()[L"artifacts"].getArray();
        for (auto it = arts.begin(); it < arts.end(); it++) {
            auto rep = UnicodeToAnsi(it->getObject()[L"fileName"].getString().c_str());
            std::cout << rep << "\n";
        }
    }
    else
    if (repostr == "S-Waterfall") {
       JSON = evalJson(readHtml("papermc.io","/ci/job/Waterfall/lastSuccessfulBuild/api/json"));
       std::cout << projectlist;
        auto arts = JSON.getObject()[L"artifacts"].getArray();
        for (auto it = arts.begin(); it < arts.end(); it++) {
            auto rep = UnicodeToAnsi(it->getObject()[L"fileName"].getString().c_str());
            std::cout << rep << "\n";
        }
    }
    else
    if (repostr == "S-Travertine") {
        JSON = evalJson(readHtml("papermc.io","/ci/job/Travertine/lastSuccessfulBuild/api/json"));
        std::cout << projectlist;
        auto arts = JSON.getObject()[L"artifacts"].getArray();
        for (auto it = arts.begin(); it < arts.end(); it++) {
            auto rep = UnicodeToAnsi(it->getObject()[L"fileName"].getString().c_str());
            std::cout << rep << "\n";
        }
    }
    else
    if (repostr == "SG-OneKeyDownload") {
        std::cout << projectlist;
        std::cout<<("BungeeCord\nWaterfall\nTravertine\nPaper-1.12\nPaper-1.13\nPaper-1.13\n");
    }
    else {
        std::cout << "invalid repositoty name!\n";
            return 0;
    }
    std::cout << "select a project from above\n";
    std::cin >> repo;
    std::string pojstr(repo);
    std::cout << "fetching download address...";
    GetProjectBranch(repostr, pojstr);
}
