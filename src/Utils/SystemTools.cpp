#include "SystemTools.h"

namespace geomtk {

int SystemTools::getNumFiles(const string &fileRoot, const string &filePattern) {
    string cmd = "ls "+fileRoot+"/"+filePattern;

    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        REPORT_ERROR("Failed to call system command \""+cmd+"\"!");
    }

    char buffer[128];
    string result;
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL) {
            result += buffer;
        }
    }

    pclose(pipe);

    uword numFile = 0, pos = 0;
    while(pos < result.length()) {
        pos = static_cast<int>(result.find("\n", pos))+1;
        numFile++;
    }

    return numFile;
}

void SystemTools::getFileNames(const string &fileRoot,
                               const string &filePattern,
                               vector<string> &fileNames) {
    boost::filesystem::path _fileRoot(fileRoot);
    boost::regex _filePattern(filePattern);
    boost::filesystem::directory_iterator i(_fileRoot), end;
    for (; i != end; ++i) {
        if(!boost::filesystem::is_regular_file(i->status())) continue;
        if (boost::regex_match(i->path().leaf().string(), _filePattern)) {
            fileNames.push_back(i->path().leaf().string());
        }
    }
}

void SystemTools::writeFile(const string &filePath, const string &content) {
    ofstream file;
    file.open(filePath, ios::out);
    file << content;
    file.close();
}

void SystemTools::removeFile(const string &filePath) {
    if (!boost::filesystem::exists(filePath)) {
        REPORT_WARNING("File \"" << filePath << "\" to be removed does not exist!");
    }
    boost::filesystem::remove_all(filePath);
}

}