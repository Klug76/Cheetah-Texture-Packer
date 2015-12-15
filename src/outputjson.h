#ifndef OUTPUTJSON_H
#define OUTPUTJSON_H

#include "imagepacker.h"
#include <QDir>
#include <QTextStream>

template<class PackerData>//TODO refactor without template (and without void* cast)
class OutputJson
{
public:
    static bool export_File(QString const& outDir, QString const& outFile, QString const& imageFormat,
                         QList<QImage> const& texture, ImagePacker const& packer)
    {
        QString outputFile = outDir;
        outputFile += QDir::separator();
        outputFile += outFile;
        outputFile += ".json";
        QFile positionsFile(outputFile);
        if(!positionsFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            return false;
        }
        QTextStream out(&positionsFile);
        //out.setCodec("UTF-8");
        out << "{\n";
        out << "\t\"item\":\n";
        out << "\t[\n";
        for(int j = 0; j < texture.count(); ++j)
        {
            QImage const& tex = texture[j];
            QString imgFile = outFile;
            if(texture.count() > 1)
            {
                imgFile += QString("_") + QString::number(j + 1);
            }
            imgFile += ".";
            imgFile += imageFormat;
            out << "\t\t{\n";
            out << "\t\t\t\"name\": \"" << imgFile << "\"";
            out << ", \"width\": " << tex.width();
            out << ", \"height\": " << tex.height();
            out << ",\n";
            out << "\t\t\t\"item\":\n";
            out << "\t\t\t[\n";
            int last_idx = 0;
            for(int i = 0; i < packer.images.size(); ++i)
            {
                if(packer.images.at(i).textureId != j)
                {
                    continue;
                }
                last_idx = i;
            }
            for(int i = 0; i < packer.images.size(); ++i)
            {
                inputImage const& img = packer.images.at(i);
                if(img.textureId != j)
                {
                    continue;
                }
                QPoint pos(img.pos.x() + packer.border.l + packer.extrude,
                           img.pos.y() + packer.border.t + packer.extrude);
                QSize size, sizeOrig;
                QRect crop;
                sizeOrig = img.size;
                if(!packer.cropThreshold)
                {
                    size = img.size;
                    crop = QRect(0, 0, size.width(), size.height());
                }
                else
                {
                    size = img.crop.size();
                    crop = img.crop;
                }
                if(img.rotated)
                {
                    size.transpose();
                    crop = QRect(crop.y(), crop.x(), crop.height(), crop.width());
                }
                out << "\t\t\t\t{\n";
                out << "\t\t\t\t\t\"name\": \"" << (static_cast<PackerData*>(img.id))->get_File_Name() << "\"";
                out << ", \"x\": " << pos.x();
                out << ", \"y\": " << pos.y();
                out << ", \"width\": " << crop.width();
                out << ", \"height\": " << crop.height();
                bool need_frame = (crop.x() != 0) || (crop.y() != 0);
                if (img.rotated)
                {
                    if (need_frame || (crop.width() != sizeOrig.height()) || (crop.height() != sizeOrig.width()))
                    {
                        out << ", \"frameX\": " << -crop.x();
                        out << ", \"frameY\": " << -crop.y();
                        out << ", \"frameWidth\": " << sizeOrig.width();
                        out << ", \"frameHeight\": " << sizeOrig.height();
                    }
                    out << ", \"rotated\": \"true\"";
                }
                else
                {
                    if (need_frame || (crop.width() != sizeOrig.width()) || (crop.height() != sizeOrig.height()))
                    {
                        out << ", \"frameX\": " << -crop.x();
                        out << ", \"frameY\": " << -crop.y();
                        out << ", \"frameWidth\": " << sizeOrig.width();
                        out << ", \"frameHeight\": " << sizeOrig.height();
                    }
                }
                out << "\n";
                out << "\t\t\t\t}";
                if (i != last_idx)
                    out << ",";
                out << "\n";
            }
            out << "\t\t\t]\n";
            out << "\t\t}";
            if (j < texture.count() - 1)
                out << ",";
            out << "\n";
        }
        out << "\t]\n";
        out << "}";
        return true;
    }
};

#endif // OUTPUTJSON_H
