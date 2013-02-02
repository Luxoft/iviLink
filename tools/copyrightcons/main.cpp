/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 */ 


#include <QtCore/QCoreApplication>
#include <QDebug>

#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <QVector>
#include <QStringList>

bool        infomode(false);
bool        testmode(false);
QString     copyright_header;
QStringList reject_copyrights;//blacklist
QStringList gray_copyrights;//graylist
QStringList skip_copyrights;//whitelist
QStringList alert_copyrights;//alert list
QStringList skip_paths;
QStringList our_paths;
QFile     * log=0;
bool        filereported(false);
bool        onlyalerts(false);
const int   maxEmptyLines = 2;

QString fileToString(const QString& path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if(log)
        {
            log->write("READ ERROR: ");
            log->write(file.errorString().toAscii());
            log->write("\n");
        }
        return QString();
    }
    QByteArray data = file.readAll();
    return QString(data);
}
void fileToStringList(const QString& path)
{
    QStringList *out = NULL;
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    while(!file.atEnd())
    {
        QByteArray data=file.readLine();
        if(data.length() && data.at(data.length()-1)=='\n')
            data.resize(data.size()-1);
        if(data.length() && data.at(data.length()-1)=='\r')
            data.resize(data.size()-1);

        QString str(data);
        if(str=="#*** FILTER ***")
        {   out=&gray_copyrights;}
        else if(str=="#*** 3RD_PARTY COPYRIGHTS ***")
        {   out=&skip_copyrights;}
        else if(str=="#*** OUR COPYRIGHTS ***")
        {   out=&reject_copyrights;}
        else if(str=="#*** 3RD_PARTY PATHS ***")
        {   out=&skip_paths;}
        else if(str=="#*** OUR PATHS ***")
        {   out=&our_paths;}
        else if(str=="#*** ALERT LIST ***")
        {    out=&alert_copyrights;}
        else if(out != NULL && str.length()){
            if(str.at(0)=='#')continue;
            (*out)<<str;
        }
    }
}
void reportFile(const QString& path)
{
    if(log && !filereported)
    {
        log->write("==============================================\n");
        log->write("* FILE: ");
        log->write(path.toAscii());
        log->write("\n");
        filereported=true;
    }
}
bool inList(const QStringList& list, const QString& text)
{
    QStringListIterator iter(list);
    while(iter.hasNext())
    {
        const QString& str=iter.next();
        if(str.startsWith("+re:"))
        {
            QRegExp re(str.mid(4));
            if(re.isValid() && !re.isEmpty())
            {
                re.setCaseSensitivity(Qt::CaseInsensitive);
                re.setMinimal(true);
                if(text.contains(re))
                    return true;
            }
        }else if(str.startsWith("-re:"))
        {
            QRegExp re(str.mid(4));
            if(re.isValid() && !re.isEmpty())
            {
                re.setCaseSensitivity(Qt::CaseInsensitive);
                re.setMinimal(true);
                if(text.contains(re))
                    return false;
            }
        }else if(text.contains(str, Qt::CaseInsensitive))
            return true;
    }
    return false;
}

bool RemoveComment(const QString& path, const QString& text, int from, int to, bool& skip, bool& modify)
{
    bool commentprinted(false);
    QString comment=text.mid(from, to-from);
    if(inList(alert_copyrights, comment))
    {
        reportFile(path);
        log->write("**** COMMENT **********************************\n");
        log->write("\n\n   ALERT!!!! FORBIDDEN LICENSE DETECTED!!!! \n\n");
        log->write(comment.toAscii());
        commentprinted=true;
    }
    bool flag=inList(gray_copyrights, comment);
    if(!flag)
        return false;
    bool comment_must_be_removed(inList(reject_copyrights, comment));
    bool comment_must_be_saved(inList(skip_copyrights, comment));
    skip|=comment_must_be_saved;
    modify|=comment_must_be_removed;
    if(infomode && log)
    {
        if(!commentprinted && !onlyalerts)
        {
            reportFile(path);

            log->write("**** COMMENT **********************************\n");
            log->write(comment.toAscii());
            log->write("\n**** ");
        }
        if(comment_must_be_removed)log->write("[REJECT] ");
        if(comment_must_be_saved)log->write("[KEEP] ");
        log->write("\n**********************************************\n");
    }
    if(comment_must_be_removed && !comment_must_be_saved)
        return true;

    return false;
}

void storeFile(const QString& orig, const QString& path, const QStringList& lines, bool skip, bool modify, bool bashcomment=false)
{

    if(skip && modify)
    {
        reportFile(path);
        if(log)
            log->write("PROBLEM: file contains both our and third-party copyrights");
    }
    if(infomode)
        return;


    QString outpath=path;
    if(testmode)
        outpath+=".test";

    QFile ofile(outpath);
    if(ofile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        if(!skip)
        {
            if(!copyright_header.isEmpty())
            {
                QStringList lines=copyright_header.split('\n');
                QStringListIterator iter(lines);
                if(!bashcomment)
                    ofile.write("/* \n");
                while(iter.hasNext())
                {
                    if(bashcomment)
                        ofile.write("# ");
                    else
                        ofile.write(" * ");
                    ofile.write(iter.next().toAscii());
                    ofile.write("\n");
                }
                if(!bashcomment)
                    ofile.write(" */ \n");
            }
            QStringListIterator iter(lines);
            while(iter.hasNext())
            {
                ofile.write(iter.next().toAscii());
            }
        }
        else
            ofile.write(orig.toAscii());
    }
    else if(log)
    {
        reportFile(path);
        log->write("WRITE ERROR: ");
        log->write(ofile.errorString().toAscii());
        log->write("\n");
    }
}

void processCXXFile(const QFileInfo& file, bool assume_our)
{
    int offset(0);
    bool skipfile(false), modify_file(assume_our);
    QString text=fileToString(file.absoluteFilePath());
    QStringList output;

    while(offset<text.length())
    {
        //qDebug()<<"==========================";
        int off_sd=text.indexOf("/*", offset);
        int off_ss=text.indexOf("//", offset);

        if(off_sd<0 && off_ss<0)
        {
            //qDebug()<<"tail="<<text.mid(offset);
            output<<text.mid(offset);
            break;
        }
        if(off_sd<0)
            off_sd=text.length();
        if(off_ss<0)
            off_ss=text.length();

        if(off_ss<off_sd)
        {
            bool comment_from_newline(true);
            int newlinebefore=off_ss==0 ? 0 : text.lastIndexOf('\n', off_ss)+1;
            for(int i=newlinebefore+1; i<off_ss; ++i)
            {
                if(!text.at(i).isSpace()){
                    comment_from_newline=false;
                    break;
                }
            }
            int end=text.indexOf('\n', off_ss);
            if(end<0)
                end=text.length();
            else
                ++end;
            //grab more lines
            if(comment_from_newline)
            {
                int tabspace=off_ss-newlinebefore;
                //find more comments from newline
                while(end!=text.length())
                {
                    int nextss=text.indexOf("//",end);
                    if(nextss<0)
                        break;
                    //check if there is \n between // and end
                    int moreN=text.indexOf('\n', end);
                    if(moreN>=0 && moreN<nextss)
                        break;
                    //check for [end]...spaces...//
                    bool spaces_between(true);
                    for(int i=end; i<nextss; ++i)
                    {
                        if(!text.at(i).isSpace())
                        {
                            spaces_between=false;
                            break;
                        }
                    }
                    if(!spaces_between || nextss-end!=tabspace)
                        break;
                    end=text.indexOf('\n', nextss);
                    if(end<0)end=text.length();else ++end;
                }
            }
            if(RemoveComment(file.absoluteFilePath(), text, off_ss, end, skipfile, modify_file))
            {
                if(offset<off_ss)
                    output<<text.mid(offset,off_ss-offset);
            }
            else
                output<<text.mid(offset, end-offset);

            offset=end;
        }else{
            int nextoffset;
            int end=text.indexOf("*/", off_sd+2);
            if(end<0)
                end=nextoffset=text.length();
            else
                nextoffset=end+2;

            if(RemoveComment(file.absoluteFilePath(), text, off_sd, end, skipfile, modify_file))
            {
                if(offset<off_sd)
                    output<<text.mid(offset, off_sd-offset);
            }
            else
                output<<text.mid(offset, end+2-offset);

            offset=nextoffset;
        }
    }
    storeFile(text, file.absoluteFilePath(), output, skipfile, modify_file);
}
void processMakeFile(const QFileInfo& file, bool assume_our)
{
    int offset(0);
    bool skipfile(false), modify_file(assume_our);
    QString text=fileToString(file.absoluteFilePath());
    QStringList output;
    while(offset<text.length())
    {
        //qDebug()<<"==========================";
        int off_ss=text.indexOf('#', offset);

        if(off_ss<0)
        {
            //qDebug()<<"tail="<<text.mid(offset);
            output<<text.mid(offset);
            break;
        }

        bool comment_from_newline(true);
        int newlinebefore=off_ss==0 ? 0 : text.lastIndexOf('\n', off_ss)+1;
        for(int i=newlinebefore+1; i<off_ss; ++i)
        {
            if(!text.at(i).isSpace())
            {
                comment_from_newline=false;
                break;
            }
        }
        int end=text.indexOf('\n', off_ss);
        if(end<0)
            end=text.length();
        else
            ++end;//end if after \n
        //grab more lines
        if(comment_from_newline)
        {
            int tabspace=off_ss-newlinebefore;
            //find more comments from newline
            while(end<text.length())
            {
                int nextss=text.indexOf('#',end);
                if(nextss<0)
                    break;
                //check if there is \n between // and end
                int moreN=text.indexOf('\n', end);
                if(moreN>=0 && moreN<nextss)
                    break;
                //check for [end]...spaces...//
                bool spaces_between(true);
                for(int i=end; i<nextss; ++i)
                {
                    if(!text.at(i).isSpace())
                    {
                        spaces_between=false;
                        break;
                    }
                }
                if(!spaces_between || nextss-end!=tabspace)
                    break;
                end=text.indexOf('\n', nextss);
                if(end<0)
                    end=text.length();
                else
                    ++end;
            }
        }
        //qDebug()<<"... ... comment at "<<off_ss<<"--"<<end;
        //qDebug()<<text.mid(off_ss, end-off_ss);
        if(RemoveComment(file.absoluteFilePath(), text, off_ss, end, skipfile, modify_file))
        {
            if(offset<off_ss)
            {
                //qDebug()<<"output {"<<text.mid(offset, off_ss-offset)<<"}";
                output<<text.mid(offset,off_ss-offset);
            }
        }
        else
        {
            //qDebug()<<"output2{"<<text.mid(offset, end-offset)<<"}";
            output<<text.mid(offset, end-offset);
        }
        offset=end;
    }
    storeFile(text, file.absoluteFilePath(), output, skipfile, modify_file, true);
}

// only spaces are in the string
bool isSpaces(const QString str)
{
    for (int i = 0; i < str.length(); i++)
    {
        if (!str[i].isSpace())
        {
            return false;
        }
    }
    return true;
}

void removeExtraLines(const QFileInfo& file)
{
    //to string list
    QStringList fileList;
    int countOfEmptyLines = 0;
    const QString& path = file.absoluteFilePath();
    QFile readfile(path);
    if(!readfile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QString outpath=path;
    if(testmode)
        outpath+=".test";

    while(!readfile.atEnd())
    {
        QByteArray data=readfile.readLine();
        if(data.length() && data.at(data.length()-1)=='\n')
            data.resize(data.size()-1);
        if(data.length() && data.at(data.length()-1)=='\r')
            data.resize(data.size()-1);

        QString str(data);
        fileList += str;
    }

    //new file without empty lines
    QFile outfile(outpath);
    if(outfile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        for (int i = 0; i < fileList.size(); ++i)
        {
            if (fileList.at(i).isEmpty() || isSpaces(fileList.at(i)))
            {
                countOfEmptyLines++;
            }
            else
            {
                countOfEmptyLines = 0;
            }
            if (countOfEmptyLines <= maxEmptyLines)
            {
                outfile.write(fileList.at(i).toAscii());
                outfile.write("\n");
            }
        }
        outfile.close();
    }
}

void parseFile(const QFileInfo& file, bool force_update=false)
{
    if(!file.exists())return;
    if(file.isSymLink())return;
    //skip files in skiplist
    if(skip_paths.contains(file.fileName()))return;
    //force update on our files
    if(!force_update)force_update=our_paths.contains(file.filePath());

    if(file.isDir())
    {
        QString name=file.baseName();
        //ignore system dirs
        if(name.length()==0 || name.at(0)==QChar('.'))return;

        QDir dir(file.absoluteFilePath());
        QFileInfoList list = dir.entryInfoList(
            QDir::NoDotAndDotDot|QDir::Readable|QDir::Dirs|QDir::Files,
            QDir::DirsFirst|QDir::Name);
        QFileInfoList::const_iterator iter=list.constBegin();
        for(;iter!=list.constEnd(); ++iter)
        {
            qDebug()<<"... "<<iter->filePath();

            parseFile(*iter,force_update);
        }
        return;
    }
    if(file.isFile())
    {
        filereported=false;//reset flag
        QStringList exts;
        exts<<"cpp"<<"c"<<"hpp"<<"h"<<"java"<<"qml";
        QString ext=file.completeSuffix().toLower();
        if(exts.contains(ext))
        {
            qDebug()<<"Parsing "<<file.baseName();
            processCXXFile(file.absoluteFilePath(),force_update);
            removeExtraLines(file.absoluteFilePath());

        }
        else if(file.baseName().toLower()=="makefile")
        {
            qDebug()<<"Parsing "<<file.baseName();
            processMakeFile(file.absoluteFilePath(),force_update);
            removeExtraLines(file.absoluteFilePath());
        }

    }

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QFile _log;
    for(int i=1;i<argc; ++i)
    {
        QString arg(argv[i]);
        if(arg=="-c")
        {
            //lisence.txt
            if(i+1==argc)return 1;
            copyright_header=fileToString(QString(argv[i+1]));
            ++i;
            continue;
        }
        if(arg=="-mask")
        {
            //mask.txt
            if(i+1==argc)return 1;
            fileToStringList(QString(argv[i+1]));
            ++i;
            continue;
        }
        if(arg=="-log")
        {
            //log.txt
            if(i+1==argc)return 1;
            _log.setFileName(QString(argv[i+1]));
            if(!_log.open(QIODevice::WriteOnly | QIODevice::Text))return 1;
            log=&_log;
            continue;
        }

        if(arg=="-a")
        {
            onlyalerts=true;
            continue;
        }
        if(arg=="-i")
        {
            infomode=true;
            continue;
        }
        if(arg=="-t")
        {
            testmode=true;
            continue;
        }
        QFileInfo file(arg);
        parseFile(file);
    }
    qDebug()<<gray_copyrights;
    return 0;
    //return a.exec();
}
