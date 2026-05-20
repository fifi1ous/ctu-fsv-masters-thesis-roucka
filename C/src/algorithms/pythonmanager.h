// Declaration of PythonManager — embedded Python interpreter and model lifecycle manager.

#ifndef PYTHONMANAGER_H
#define PYTHONMANAGER_H

#include <QString>
#include <QStringList>
#include <vector>
#include <string>

#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/pytypes.h>

#include "addpath.h"

namespace py = pybind11;

// --- PythonManager ---
// Static utility class managing the embedded pybind11 interpreter state.
class PythonManager
{
public:
    PythonManager();

    // Starts the embedded CPython interpreter with PYTHONHOME pointed at the bundled
    // "python-embed" directory next to the executable. Must be called exactly once
    // AFTER QApplication is constructed, BEFORE any Python code runs.
    // Throws std::runtime_error if Python initialization fails.
    static void bootstrapInterpreter();

    // Tears down the CPython interpreter. Paired with bootstrapInterpreter().
    static void shutdownInterpreter();

    // Loads Python modules and pre-instantiates the YOLO models.
    // Call after bootstrapInterpreter(). Safe to call multiple times.
    static void initialize();

    // Releases cached py::object instances before the interpreter halts.
    static void finalize();

    // Python-bridged execution commands.
    static void create_annotaions(QString path_to_pdf);
    static std::vector<QString> classify(QString folder);

private:
    // Lazy path getters.
    static const QString& pathToSegmentModel() { static const QString s = AddPath::getPathToModelSegment(); return s; }
    static const QString& pathToClassifyModel(){ static const QString s = AddPath::getPathToModelClassify();return s; }
    static const QString& pathToAnnotation()   { static const QString s = AddPath::getAnnotationPath();     return s; }
    static const QString& pathToWorkspace()    { static const QString s = AddPath::getWorkspacePath();      return s; }
    static const QString& pathToScripts()      { static const QString s = AddPath::getScriptsPath();        return s; }
    static const QString& pathToGP()           { static const QString s = AddPath::getGPPath();             return s; }
    static const QString& pathToGNSS()         { static const QString s = AddPath::getGNSSPath();           return s; }

    static bool s_interpreterRunning;
    static bool s_initialized;

    // Cached py::object instances to prevent garbage collection and reload overhead.
    static py::object s_bridge;          // Python module: work_with_pdf
    static py::object s_yoloModel;       // Instantiated YOLO model (Segmentation)
    static py::object s_classify_model;  // Instantiated YOLO model (Classification)
};

#endif // PYTHONMANAGER_H