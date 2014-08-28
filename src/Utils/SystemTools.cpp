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

    int numFile = 0, pos = 0;
    while(pos < result.length()) {
        pos = static_cast<int>(result.find("\n", pos))+1;
        numFile++;
    }

    return numFile;
}

void SystemTools::getFileNames(const string &fileRoot,
                               const string &filePattern,
                               vector<string> &fileNames) {
    string cmd = "find "+fileRoot+" -name \""+filePattern+"\"";

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

    int pos = 0, prev_pos;
    while(pos < result.length()) {
        prev_pos = pos;
        pos = static_cast<int>(result.find("\n", pos))+1;
        fileNames.push_back(result.substr(prev_pos, pos-1-prev_pos));
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