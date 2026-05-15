// Implementation of the FileNonPDF class.

#include "filenonpdf.h"

// Default constructor
FileNonPDF::FileNonPDF() 
    : FileBase(), zpmz_gp_noth(""), format("") 
{}

// Parameterized constructor
FileNonPDF::FileNonPDF(bool is_present, int ku, int zpmz, const QString& zpmz_gp_noth,
                      const QString& name, const QString& format,
                      const QString& date, const QString& path,
                      const QString& what_it_is, const QString& letter)
    : FileBase(is_present, ku, zpmz, name, date, path, what_it_is, letter),
      zpmz_gp_noth(zpmz_gp_noth), format(format) 
{}

// Setters
void FileNonPDF::setZpmz_gp_noth(const QString& zpmz_gp_noth) { this->zpmz_gp_noth = zpmz_gp_noth; }
void FileNonPDF::setFormat(const QString& format) { this->format = format; }

void FileNonPDF::setAll(bool is_present, int ku, int zpmz, const QString& zpmz_gp_noth,
                       const QString& name, const QString& format,
                       const QString& date, const QString& path,
                       const QString& what_it_is, const QString& letter)
{
    setPresent(is_present);
    setKu(ku);
    setZpmz(zpmz);
    setName(name);
    setDate(date);
    setPath(path);
    setWhatItIs(what_it_is);
    setLetter(letter);
    
    this->zpmz_gp_noth = zpmz_gp_noth;
    this->format = format;
}

// Getters
QString FileNonPDF::getZpmz_gp_noth() const { return zpmz_gp_noth; }
QString FileNonPDF::getFormat() const { return format; }

// Clear all data
void FileNonPDF::clearAll()
{
    FileBase::clearAll();
    zpmz_gp_noth.clear();
    format.clear();
}