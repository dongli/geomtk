#ifndef __GEOMTK_SystemTools__
#define __GEOMTK_SystemTools__

#include "geomtk_commons.h"

namespace geomtk {

class SystemTools {
public:
    int getNumFiles(const string &fileRoot, const string &filePattern);

    static void
    getFileNames(const string &fileRoot, const string &filePattern,
                 vector<string> &fileNames);

    static void
    writeFile(const string &filePath, const string &content);

    static void
    removeFile(const string &filePath);
}; // SystemTools

} // geomtk

#endif // __GEOMTK_SystemTools__