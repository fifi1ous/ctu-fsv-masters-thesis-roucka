// Implementation of PythonManager.

#include "pythonmanager.h"
#include <QDir>
#include <QFileInfo>
#include "maps.h"

bool       PythonManager::s_initialized = false;
py::object PythonManager::s_bridge;
py::object PythonManager::s_yoloModel;
py::object PythonManager::s_classify_model;

PythonManager::PythonManager() {}

void PythonManager::initialize()
{
    if (s_initialized)
        return;

    try {
        py::module_ sys = py::module_::import("sys");

        // Inject application scripts directory into Python sys.path
        sys.attr("path").attr("append")(pathToScripts().toStdString());

        // Inject bundled site-packages (pypdf, ultralytics, torch) at priority 0
        QString sitePackages = pathToWorkspace() + "/../python/Lib/site-packages";
        sys.attr("path").attr("insert")(0, sitePackages.toStdString());

        // Pre-import the custom bridge script, triggering imports of external libraries
        s_bridge = py::module_::import("work_with_pdf");

        // Pre-load YOLO models into memory to eliminate inference startup latency
        py::module_ ultralytics = py::module_::import("ultralytics");
        s_yoloModel = ultralytics.attr("YOLO")(pathToSegmentModel().toStdString());
        s_classify_model = ultralytics.attr("YOLO")(pathToClassifyModel().toStdString());

        s_initialized = true;
        qDebug() << "PythonManager: Initialization complete (modules and YOLO models loaded).";

    } catch (py::error_already_set &e) {
        qDebug() << "PythonManager: Initialization error:" << e.what();
    }
}

// This code was modified by Claude Opus 4.7 //
void PythonManager::finalize()
{
    s_yoloModel       = py::object();
    s_classify_model  = py::object();
    s_bridge          = py::object();
    s_initialized     = false;
}
// End of code modified by Claude Opus 4.7 //

void PythonManager::create_annotaions(QString path_to_pdf)
{
    if (!s_initialized)
        initialize();

    try {
        // Dispatch to Python passing the cached model to bypass load times
        s_bridge.attr("segment_geometric_plan")(
            path_to_pdf.toStdString(),
            pathToGP().toStdString(),
            s_yoloModel,
            pathToAnnotation().toStdString()
        );

        qDebug() << "Segmentation and annotation completed successfully.";

    } catch (py::error_already_set &e) {
        qDebug() << "Critical Python error during segmentation:" << e.what();
    }
}

std::vector<QString> PythonManager::classify(QString folder)
{
    if (!s_initialized)
        initialize();

    std::vector<QString> results;

    QDir dir(folder);
    dir.setNameFilters({"*.pdf"});
    dir.setFilter(QDir::Files | QDir::NoSymLinks);

    QFileInfoList fileList = dir.entryInfoList();

    // Ensure the GNSS target directory exists before Python attempts to write images
    QDir(pathToGNSS()).mkpath(".");

    for (const QFileInfo &fileInfo : fileList) {
        QString filePath = fileInfo.absoluteFilePath();

        // Heuristic: Auto-classify files explicitly named "oprav" bypassing the model
        if (fileInfo.fileName().toLower().contains("oprav")) {
            results.push_back(QStringLiteral("oprav"));
            continue;
        }

        try {
            auto py_result = s_bridge.attr("classify_pdf")(
                filePath.toStdString(),
                pathToGNSS().toStdString(),
                s_classify_model
            );
            results.push_back(QString::fromStdString(py_result.cast<std::string>()));
        } catch (const std::exception &e) {
            qDebug() << "Error classifying" << filePath << ":" << e.what();
            results.push_back(QString());
        }
    }

    // Cleanup: Remove any temporary PNG extractions left over in the target folder
    QDir pngDir(folder);
    pngDir.setNameFilters({"*.png"});
    pngDir.setFilter(QDir::Files | QDir::NoSymLinks);

    QFileInfoList pngList = pngDir.entryInfoList();
    for (const QFileInfo &pngInfo : pngList) {
        QFile::remove(pngInfo.absoluteFilePath());
    }

    return results;
}