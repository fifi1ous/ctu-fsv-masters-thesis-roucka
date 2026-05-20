// Implementation of PythonManager.

#include "pythonmanager.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>
#include <stdexcept>
#include "maps.h"

bool       PythonManager::s_interpreterRunning = false;
bool       PythonManager::s_initialized        = false;
py::object PythonManager::s_bridge;
py::object PythonManager::s_yoloModel;
py::object PythonManager::s_classify_model;

PythonManager::PythonManager() {}

// ---------------------------------------------------------------------------
// Interpreter bootstrap — replaces py::scoped_interpreter so we can point
// PYTHONHOME at the bundled Python runtime, making the app portable.
// ---------------------------------------------------------------------------
void PythonManager::bootstrapInterpreter()
{
    if (s_interpreterRunning)
        return;

    // Build absolute paths relative to the executable. This is the key change
    // that makes the app portable: everything Python needs sits next to the .exe.
    const QString exeDir  = QCoreApplication::applicationDirPath();
    const QString pyHome  = QDir(exeDir).filePath("python-embed");
    const QString pyStdlib       = pyHome + "/python313.zip";
    const QString pySitePackages = pyHome + "/Lib/site-packages";
    const QString scriptsDir     = QDir(exeDir).filePath("scripts");

    // Sanity-check: if python-embed isn't there, fail loudly with a useful message.
    if (!QFileInfo::exists(pyHome)) {
        throw std::runtime_error(
            ("Python runtime directory not found: " + pyHome).toStdString());
    }
    if (!QFileInfo::exists(pyStdlib)) {
        throw std::runtime_error(
            ("Python standard library zip not found: " + pyStdlib +
             "\n(Check that the Python version in pythonmanager.cpp matches "
             "the embeddable package you bundled.)").toStdString());
    }

    // Configure CPython.
    PyConfig config;
    PyConfig_InitIsolatedConfig(&config);
    // Isolated config = ignore PYTHONPATH/PYTHONHOME env vars, ignore the Windows
    // registry, ignore the user's site-packages. We control everything explicitly,
    // which is exactly what we want for a portable deployment.

    PyStatus status;

    // Set PYTHONHOME — tells CPython where to find its standard library.
    {
        std::wstring w = pyHome.toStdWString();
        status = PyConfig_SetString(&config, &config.home, w.c_str());
        if (PyStatus_Exception(status)) {
            PyConfig_Clear(&config);
            throw std::runtime_error("PyConfig_SetString(home) failed");
        }
    }

    // Build sys.path manually: stdlib zip, site-packages, and our scripts dir.
    config.module_search_paths_set = 1;

    auto appendPath = [&](const QString &p) {
        std::wstring w = p.toStdWString();
        PyStatus st = PyWideStringList_Append(&config.module_search_paths, w.c_str());
        if (PyStatus_Exception(st)) {
            PyConfig_Clear(&config);
            throw std::runtime_error(
                ("PyWideStringList_Append failed for: " + p).toStdString());
        }
    };

    appendPath(pyStdlib);        // stdlib (encodings, os, io, ...)
    appendPath(pyHome);          // python-embed itself (DLLs, etc.)
    appendPath(pySitePackages);  // pip-installed packages (numpy, ultralytics, ...)
    appendPath(scriptsDir);      // our own Python modules (work_with_pdf.py)

    // Fire up the interpreter.
    status = Py_InitializeFromConfig(&config);
    PyConfig_Clear(&config);
    if (PyStatus_Exception(status)) {
        std::string msg = "Py_InitializeFromConfig failed";
        if (status.err_msg) {
            msg += std::string(": ") + status.err_msg;
        }
        throw std::runtime_error(msg);
    }

    s_interpreterRunning = true;
    qDebug() << "PythonManager: interpreter bootstrapped, PYTHONHOME =" << pyHome;
}

void PythonManager::shutdownInterpreter()
{
    if (!s_interpreterRunning)
        return;
    Py_Finalize();
    s_interpreterRunning = false;
}

// ---------------------------------------------------------------------------
// Module + model initialization (unchanged in spirit, simplified).
// ---------------------------------------------------------------------------
void PythonManager::initialize()
{
    if (s_initialized)
        return;

    try {
        // sys.path is already correctly configured by bootstrapInterpreter().
        // We don't need to mess with it here anymore.

        // Pre-import the custom bridge script.
        s_bridge = py::module_::import("work_with_pdf");

        // Pre-load YOLO models into memory to eliminate inference startup latency.
        py::module_ ultralytics = py::module_::import("ultralytics");
        s_yoloModel      = ultralytics.attr("YOLO")(pathToSegmentModel().toStdString());
        s_classify_model = ultralytics.attr("YOLO")(pathToClassifyModel().toStdString());

        s_initialized = true;
        qDebug() << "PythonManager: Initialization complete (modules and YOLO models loaded).";

    } catch (py::error_already_set &e) {
        qDebug() << "PythonManager: Initialization error:" << e.what();
        QMessageBox::critical(
            nullptr,
            QStringLiteral("Chyba inicializace Pythonu"),
            QStringLiteral("Nepodařilo se načíst Python moduly nebo YOLO modely.\n\n%1")
                .arg(QString::fromUtf8(e.what())));
    }
}

void PythonManager::finalize()
{
    s_yoloModel       = py::object();
    s_classify_model  = py::object();
    s_bridge          = py::object();
    s_initialized     = false;
}

void PythonManager::create_annotaions(QString path_to_pdf)
{
    if (!s_initialized)
        initialize();

    // If initialize() failed, s_bridge is still a null py::object — calling
    // .attr() on it would crash. Bail out cleanly; the user already saw the
    // initialization error via QMessageBox.
    if (!s_initialized)
        return;

    try {
        s_bridge.attr("segment_geometric_plan")(
            path_to_pdf.toStdString(),
            pathToGP().toStdString(),
            s_yoloModel,
            pathToAnnotation().toStdString()
            );

        qDebug() << "Segmentation and annotation completed successfully.";

    } catch (py::error_already_set &e) {
        qDebug() << "Critical Python error during segmentation:" << e.what();
        QMessageBox::critical(
            nullptr,
            QStringLiteral("Chyba segmentace"),
            QStringLiteral("Segmentace PDF selhala — anotace nebyly vytvořeny.\n\n%1")
                .arg(QString::fromUtf8(e.what())));
    }
}

std::vector<QString> PythonManager::classify(QString folder)
{
    if (!s_initialized)
        initialize();

    std::vector<QString> results;

    if (!s_initialized)
        return results;

    QDir dir(folder);
    dir.setNameFilters({"*.pdf"});
    dir.setFilter(QDir::Files | QDir::NoSymLinks);

    QFileInfoList fileList = dir.entryInfoList();

    QDir(pathToGNSS()).mkpath(".");

    for (const QFileInfo &fileInfo : fileList) {
        QString filePath = fileInfo.absoluteFilePath();

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
            QMessageBox::critical(
                nullptr,
                QStringLiteral("Chyba klasifikace"),
                QStringLiteral("Klasifikace souboru selhala: %1\n\n%2")
                    .arg(filePath, QString::fromUtf8(e.what())));
            results.push_back(QString());
        }
    }

    // Cleanup PNGs.
    QDir pngDir(folder);
    pngDir.setNameFilters({"*.png"});
    pngDir.setFilter(QDir::Files | QDir::NoSymLinks);

    for (const QFileInfo &pngInfo : pngDir.entryInfoList()) {
        QFile::remove(pngInfo.absoluteFilePath());
    }

    return results;
}