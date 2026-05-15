#ifndef CONTROLZPMZ_H
#define CONTROLZPMZ_H

#include "filepdf.h"
#include "filenonpdf.h"
#include <QString>

class ControlZPMZ
{
public:
    enum FileType { Popispole, Nacrt, Zap, Prot, Vymery, Sezvlast, Oprav, Dsps, Vytyc, GNSS, Vfk };

    ControlZPMZ();

    void setFilePath(FileType type, const QString& path);
    QString getFilePath(FileType type) const;
    void clearFile(FileType type);
    void clearAll();

private:
    FilePDF popispole;
    FilePDF nacrt;
    FilePDF zap;
    FilePDF prot;
    FilePDF vymery;
    FilePDF sezvlast;
    FilePDF oprav;
    FilePDF dsps;
    FilePDF vytyc;
    FilePDF gnss;
    FileNonPDF vfk;

    FilePDF* pdfFile(FileType type);
    const FilePDF* pdfFile(FileType type) const;
};

#endif // CONTROLZPMZ_H
