// Declaration of DeleteWorkFiles — workspace cleanup utility.

#ifndef DELETEWORKFILES_H
#define DELETEWORKFILES_H

#include <QDir>
#include <QFile>
#include <QFileInfo>

#include "addpath.h"

// --- DeleteWorkFiles ---
// Static utility class for sanitizing the application workspace.
class DeleteWorkFiles
{
public:
    DeleteWorkFiles();

    // Wipes all generated data across all known workspace directories.
    static void deleteFiles();

private:
    // Internal helper declared for future annotation-specific cleanup isolation.
    static void deleteAnnotations();
    
    // Internal helper to iterate and delete all files in a specific target path.
    static void deleteFilesInFolder(const QString& path);
};

#endif // DELETEWORKFILES_H