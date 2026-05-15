// Declaration of the GeometricPlan class — aggregate container for cadastral file metadata.

#ifndef GEOMETRICPLAN_H
#define GEOMETRICPLAN_H

#include <vector>
#include <stdexcept>

#include "filepdf.h"
#include "filenonpdf.h"
#include "fileother.h"

// --- GeometricPlan ---
// Container for every file that makes up a single geometric plan submission.
class GeometricPlan
{
private:
    // --- PDF Fields ---
    FilePDF popispole;  // Popisové pole (description field).
    FilePDF nacrt;      // Náčrt (sketch).
    FilePDF zap;        // Záznam podrobného měření změn (ZPMZ).
    FilePDF prot;       // Protokol (protocol).
    FilePDF vymery;     // Výkaz výměr (area statement).
    FilePDF sezvlast;   // Seznam vlastníků (list of owners).
    FilePDF oprav;      // Opravy (corrections).
    FilePDF dsps;       // Dokumentace skutečného provedení stavby (DSPS).
    FilePDF vytyc;      // Vytyčovací náčrt (staking sketch).
    FilePDF gp;         // Geometrický plán (geometric plan).
    FilePDF zadost;     // Žádost (application/request).
    FilePDF overeni;    // Ověření (verification).

    // --- Non-PDF Fields ---
    FileNonPDF vfk;     // VFK exchange-format file.
    FileNonPDF ss;      // Srovnávací sestavení (SS) file.
    FileNonPDF AZI1;    // AZI measurement file 1.
    FileNonPDF AZI2;    // AZI measurement file 2.
    FileNonPDF AZI3;    // AZI measurement file 3.

    // --- Miscellaneous Fields ---
    std::vector<FileOther> other;   // Additional files included beyond the standard set.

    // Utility: Validate that an index is within the bounds of the "other" vector.
    void validateIndex(int index, const char* functionName) const
    {
        if (index < 0 || index >= static_cast<int>(other.size()))
        {
            throw std::out_of_range(QString("%1: Index out of range").arg(functionName).toStdString());
        }
    }

public:
    // Default constructor. Default-initializes every file slot and leaves the other vector empty.
    GeometricPlan();

    // --- PDF Setters ---
    void setPopispole(const FilePDF& popispole);    
    void setNacrt(const FilePDF& nacrt);            
    void setZap(const FilePDF& zap);                
    void setProt(const FilePDF& prot);              
    void setVymery(const FilePDF& vymery);          
    void setSezvlast(const FilePDF& sezvlast);      
    void setOprav(const FilePDF& oprav);            
    void setDsps(const FilePDF& dsps);              
    void setVytyc(const FilePDF& vytyc);            
    void setGp(const FilePDF& gp);                  
    void setOvereni(const FilePDF& overeni);        
    void setZadost(const FilePDF& zadost);          

    // --- Non-PDF Setters ---
    void setVfk(const FileNonPDF& vfk);             
    void setSs(const FileNonPDF& ss);               
    void setAZI1(const FileNonPDF& AZI1);           
    void setAZI2(const FileNonPDF& AZI2);           
    void setAZI3(const FileNonPDF& AZI3);           

    // --- PDF Getters ---
    const FilePDF& getPopispole() const;    
    const FilePDF& getNacrt() const;        
    const FilePDF& getZap() const;          
    const FilePDF& getProt() const;         
    const FilePDF& getVymery() const;       
    const FilePDF& getSezvlast() const;     
    const FilePDF& getOprav() const;        
    const FilePDF& getDsps() const;         
    const FilePDF& getVytyc() const;        
    const FilePDF& getGp() const;           
    const FilePDF& getOvereni() const;      
    const FilePDF& getZadost() const;       

    // --- Non-PDF Getters ---
    const FileNonPDF& getVfk() const;       
    const FileNonPDF& getSs() const;        
    const FileNonPDF& getAZI1() const;      
    const FileNonPDF& getAZI2() const;      
    const FileNonPDF& getAZI3() const;      

    // --- Other-files API ---
    // Operations on the variable-length vector of miscellaneous files.

    // Replace the entire collection of miscellaneous files.
    void setOther(const std::vector<FileOther>& otherFiles);

    // Append a single file entry to the back of the collection.
    void pushOther(const FileOther& otherFile);

    // Remove every entry from the collection.
    void clearOther();

    // Remove every occurrence of a specific file from the collection.
    //   otherFile - Entry to remove (equality uses FileOther::operator==).
    void removeOther(const FileOther& otherFile);

    // Returns a const reference to the underlying vector.
    const std::vector<FileOther>& getOther() const;

    // Returns the number of entries currently stored in the collection.
    int getOtherSize() const;

    // Bounds-checked read access to an entry. Throws std::out_of_range on failure.
    const FileOther& getOtherAt(int index) const;

    // Bounds-checked removal of an entry by index. Throws std::out_of_range on failure.
    void removeOtherAt(int index);
};

#endif // GEOMETRICPLAN_H