// Implementation of the GeometricPlan class.

#include "geometricplan.h"
#include <algorithm>

// Default-construct every named file slot; the other vector starts empty.
GeometricPlan::GeometricPlan()
    : popispole(), nacrt(), zap(), prot(), vymery(), sezvlast(), oprav(), 
      dsps(), vytyc(), gp(), zadost(), overeni(),
      vfk(), ss(), AZI1(), AZI2(), AZI3() 
{}

// --- Setters ---
void GeometricPlan::setPopispole(const FilePDF& popispole) { this->popispole = popispole; }
void GeometricPlan::setNacrt(const FilePDF& nacrt) { this->nacrt = nacrt; }
void GeometricPlan::setZap(const FilePDF& zap) { this->zap = zap; }
void GeometricPlan::setProt(const FilePDF& prot) { this->prot = prot; }
void GeometricPlan::setVymery(const FilePDF& vymery) { this->vymery = vymery; }
void GeometricPlan::setSezvlast(const FilePDF& sezvlast) { this->sezvlast = sezvlast; }
void GeometricPlan::setOprav(const FilePDF& oprav) { this->oprav = oprav; }
void GeometricPlan::setDsps(const FilePDF& dsps) { this->dsps = dsps; }
void GeometricPlan::setVytyc(const FilePDF& vytyc) { this->vytyc = vytyc; }
void GeometricPlan::setGp(const FilePDF& gp) { this->gp = gp; }
void GeometricPlan::setZadost(const FilePDF& zadost) { this->zadost = zadost; }
void GeometricPlan::setOvereni(const FilePDF& overeni) { this->overeni = overeni; }

void GeometricPlan::setVfk(const FileNonPDF& vfk) { this->vfk = vfk; }
void GeometricPlan::setSs(const FileNonPDF& ss) { this->ss = ss; }
void GeometricPlan::setAZI1(const FileNonPDF& AZI1) { this->AZI1 = AZI1; }
void GeometricPlan::setAZI2(const FileNonPDF& AZI2) { this->AZI2 = AZI2; }
void GeometricPlan::setAZI3(const FileNonPDF& AZI3) { this->AZI3 = AZI3; }

// --- Getters ---
const FilePDF& GeometricPlan::getPopispole() const { return popispole; }
const FilePDF& GeometricPlan::getNacrt() const { return nacrt; }
const FilePDF& GeometricPlan::getZap() const { return zap; }
const FilePDF& GeometricPlan::getProt() const { return prot; }
const FilePDF& GeometricPlan::getVymery() const { return vymery; }
const FilePDF& GeometricPlan::getSezvlast() const { return sezvlast; }
const FilePDF& GeometricPlan::getOprav() const { return oprav; }
const FilePDF& GeometricPlan::getDsps() const { return dsps; }
const FilePDF& GeometricPlan::getVytyc() const { return vytyc; }
const FilePDF& GeometricPlan::getGp() const { return gp; }
const FilePDF& GeometricPlan::getZadost() const { return zadost; }
const FilePDF& GeometricPlan::getOvereni() const { return overeni; }

const FileNonPDF& GeometricPlan::getVfk() const { return vfk; }
const FileNonPDF& GeometricPlan::getSs() const { return ss; }
const FileNonPDF& GeometricPlan::getAZI1() const { return AZI1; }
const FileNonPDF& GeometricPlan::getAZI2() const { return AZI2; }
const FileNonPDF& GeometricPlan::getAZI3() const { return AZI3; }

// Replace the other-files vector entirely
void GeometricPlan::setOther(const std::vector<FileOther>& otherFiles)
{
    this->other = otherFiles;
}

// Append an entry to the back of the other-files vector
void GeometricPlan::pushOther(const FileOther& otherFile)
{
    this->other.push_back(otherFile);
}

// Erase all entries from the other-files vector
void GeometricPlan::clearOther()
{
    this->other.clear();
}

// Remove every entry equal to otherFile. 
// Uses the standard erase-remove idiom to process the vector in a single pass.
void GeometricPlan::removeOther(const FileOther& otherFile)
{
    auto it = std::remove(other.begin(), other.end(), otherFile);
    if (it != other.end())
    {
        other.erase(it, other.end());
    }
}

// Read-only access to the underlying other-files vector
const std::vector<FileOther>& GeometricPlan::getOther() const
{
    return other;
}

// Number of entries in the other-files vector
int GeometricPlan::getOtherSize() const
{
    return static_cast<int>(other.size());
}

// Bounds-checked access to a single entry
const FileOther& GeometricPlan::getOtherAt(int index) const
{
    validateIndex(index, "getOtherAt");
    return other.at(index);
}

// Bounds-checked removal of a single entry
void GeometricPlan::removeOtherAt(int index)
{
    validateIndex(index, "removeOtherAt");
    other.erase(other.begin() + index);
}