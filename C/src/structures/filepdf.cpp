// Implementation of the FilePDF class.

#include "filepdf.h"

// Default constructor
FilePDF::FilePDF() 
    : FileBase(), zpmz_gp(""), version("") 
{}

// Parameterized constructor
FilePDF::FilePDF(bool is_present, int ku, int zpmz, const QString& zpmz_gp,
                 const QString& name, const QString& version,
                 const QString& date, const QString& path,
                 const QString& what_it_is, const QString& letter)
    : FileBase(is_present, ku, zpmz, name, date, path, what_it_is, letter),
      zpmz_gp(zpmz_gp), version(version) 
{}

// Setters
void FilePDF::setZpmz_gp(const QString& zpmz_gp) { this->zpmz_gp = zpmz_gp; }
void FilePDF::setVersion(const QString& version) { this->version = version; }

void FilePDF::setAll(bool is_present, int ku, int zpmz, const QString& zpmz_gp,
                    const QString& name, const QString& version,
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
    
    this->zpmz_gp = zpmz_gp;
    this->version = version;
}

// Getters
QString FilePDF::getZpmz_gp() const { return zpmz_gp; }
QString FilePDF::getVersion() const { return version; }

// Clear all data
void FilePDF::clearAll()
{
    FileBase::clearAll();
    zpmz_gp.clear();
    version.clear();
}