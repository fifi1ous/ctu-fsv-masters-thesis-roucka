#include "controlzpmz.h"

ControlZPMZ::ControlZPMZ() {}

FilePDF* ControlZPMZ::pdfFile(FileType type)
{
    switch (type) {
    case Popispole: return &popispole;
    case Nacrt:     return &nacrt;
    case Zap:       return &zap;
    case Prot:      return &prot;
    case Vymery:    return &vymery;
    case Sezvlast:  return &sezvlast;
    case Oprav:     return &oprav;
    case Dsps:      return &dsps;
    case Vytyc:     return &vytyc;
    case GNSS:      return &gnss;
    default:        return nullptr;
    }
}

const FilePDF* ControlZPMZ::pdfFile(FileType type) const
{
    return const_cast<ControlZPMZ*>(this)->pdfFile(type);
}

void ControlZPMZ::setFilePath(FileType type, const QString& path)
{
    if (type == Vfk) {
        vfk.setPath(path);
    } else if (auto* f = pdfFile(type)) {
        f->setPath(path);
    }
}

QString ControlZPMZ::getFilePath(FileType type) const
{
    if (type == Vfk)
        return vfk.getPath();
    if (auto* f = pdfFile(type))
        return f->getPath();
    return {};
}

void ControlZPMZ::clearFile(FileType type)
{
    if (auto* f = pdfFile(type))
        f->clearAll();
}

void ControlZPMZ::clearAll()
{
    for (int i = Popispole; i <= GNSS; ++i)
        clearFile(static_cast<FileType>(i));
}
