// Entry point for the Control_GP application.

#include "mainwindow.h"
#include "pythonmanager.h"

#include <QApplication>
#include <pybind11/embed.h>

namespace py = pybind11;

int main(int argc, char *argv[])
{
    // Initialize the Qt GUI application framework.
    QApplication a(argc, argv);

    // Start the embedded Python interpreter.
    py::scoped_interpreter guard{};

    // Initialize Python-side dependencies (e.g., appending system paths, 
    PythonManager::initialize();

    // Construct and display the main application window.
    MainWindow w;
    w.show();

    // Enter the Qt event loop. This call blocks until the application is closed.
    int result = a.exec();

    // Clean up any lingering Python resources or threads before the application terminates.
    PythonManager::finalize();

    return result;
}