// Implementation of the FileBase class.

#include "filebase.h"

// Default constructor
FileBase::FileBase()
    : is_present(false), ku(0), zpmz(0), name(""), date(""), path(""), what_it_is(""), letter("") 
{}

// Parameterized constructor
FileBase::FileBase(bool is_present, int ku, int zpmz, const QString& name,
                   const QString& date, const QString& path,
                   const QString& what_it_is, const QString& letter)
    : is_present(is_present), ku(ku), zpmz(zpmz), name(name),
      date(date), path(path), what_it_is(what_it_is), letter(letter) 
{}

// Setters
void FileBase::setPresent(bool is_present) { this->is_present = is_present; }
void FileBase::setKu(int ku) { this->ku = ku; }
void FileBase::setZpmz(int zpmz) { this->zpmz = zpmz; }
void FileBase::setName(const QString& name) { this->name = name; }
void FileBase::setDate(const QString& date) { this->date = date; }
void FileBase::setPath(const QString& path) { this->path = path; }
void FileBase::setWhatItIs(const QString& what_it_is) { this->what_it_is = what_it_is; }
void FileBase::setLetter(const QString& letter) { this->letter = letter; }

// Getters
bool FileBase::getPresent() const { return is_present; }
int FileBase::getKu() const { return ku; }
int FileBase::getZpmz() const { return zpmz; }
QString FileBase::getName() const { return name; }
QString FileBase::getDate() const { return date; }
QString FileBase::getPath() const { return path; }
QString FileBase::getWhatItIs() const { return what_it_is; }
QString FileBase::getLetter() const { return letter; }

// Clear all data
void FileBase::clearAll()
{
    is_present = false;
    ku = 0;
    zpmz = 0;
    name.clear();
    date.clear();
    path.clear();
    what_it_is.clear();
    letter.clear();
}