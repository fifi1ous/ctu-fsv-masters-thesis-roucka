// Implementation of the FileOther class.

#include "fileother.h"

// Default constructor
FileOther::FileOther() {}

// Parameterized constructor
FileOther::FileOther(const QString& name, const QString& format,
                    const QString& path, const QString& what_it_is)
    : name(name), format(format), path(path), what_it_is(what_it_is) 
{}

// Setters
void FileOther::setName(const QString& name) { this->name = name; }
void FileOther::setFormat(const QString& format) { this->format = format; }
void FileOther::setPath(const QString& path) { this->path = path; }
void FileOther::setWhatItIs(const QString& what_it_is) { this->what_it_is = what_it_is; }

// Getters
QString FileOther::getName() const { return name; }
QString FileOther::getFormat() const { return format; }
QString FileOther::getPath() const { return path; }
QString FileOther::getWhatItIs() const { return what_it_is; }