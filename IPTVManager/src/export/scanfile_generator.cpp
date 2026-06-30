#include "scanfile_generator.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

namespace Iptv::Export {

bool ScanFileGenerator::generate(const QString &path, const QStringList &testUrls,
                                 const QStringList &scanIps, const ExportOptions &opts)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << path;
        return false;
    }
    
    QTextStream stream(&file);
    
    // Write test URLs
    for (const QString &ip : testUrls) {
        stream << opts.urlPrefix << "/" << opts.urlType << "/" << ip;
        if (opts.fccEnabled && !opts.fccUrl.isEmpty()) {
            stream << "?fcc=" << opts.fccUrl;
        }
        stream << "\n";
    }
    
    // Write scan IPs
    for (const QString &ip : scanIps) {
        stream << opts.urlPrefix << "/" << opts.urlType << "/" << ip;
        if (opts.fccEnabled && !opts.fccUrl.isEmpty()) {
            stream << "?fcc=" << opts.fccUrl;
        }
        stream << "\n";
    }
    
    file.close();
    return true;
}

} // namespace Iptv::Export
