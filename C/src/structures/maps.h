// Mapping configurations for bounding box detection classes and file types.

#ifndef MAPS_H
#define MAPS_H

#include <QMap>
#include "structures.h"
#include "addpath.h"
#include "filepdf.h"

// --- BB_NAMES ---
// Configuration and metadata mapping for bounding box detection classes.
inline const QMap<int, MapPartsGP>& BB_NAMES()
{
    static const QMap<int, MapPartsGP> map = {
        {0, {"Mapové pole", "map", AddPath::getGPMapPath() }},
        {1, {"Výkaz výměr", "vykaz", AddPath::getGPVykazPath() }},
        {2, {"BPEJ", "bpej", AddPath::getGPBpejPath()}},
        {3, {"SS", "ss", AddPath::getGPSSPath()}},
        {4, {"Popisové pole", "popispole", AddPath::getGPPopispolePath()}},
        {5, {"Klad", "klad", AddPath::getGPKladPath() }}
    };
    return map;
}

// --- NAMESMAP ---
// Global mapping of internal file/document abbreviations to human-readable Czech names.
inline const QMap<QString, QString> NAMESMAP = {
    {"dsps", "DSPS"},
    {"nacrt", "Náčrt"},
    {"GP", "Geometrický plán"},
    {"zadost", "Žádost"},
    {"prot", "Protokol o výpočtech"},
    {"zap", "Zápisník"},
    {"vymery", "Výměry"},
    {"popispole", "Popisové pole"},
    {"sezvlast", "Seznámení vlastníků"},
    {"oprav", "Opravy"},
    {"vytyc", "Dokumentace o vytyčení"}
};

#endif // MAPS_H