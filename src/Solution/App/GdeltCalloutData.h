// GEOINT Monitor is a sample native desktop application for geospatial intelligence workflows.
// Copyright (C) 2020 Jan Tschada (gisfromscratch@live.de)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Additional permission under GNU LGPL version 3 section 4 and 5
// If you modify this Program, or any covered work, by linking or combining
// it with ArcGIS Runtime for Qt (or a modified version of that library),
// containing parts covered by the terms of ArcGIS Runtime for Qt,
// the licensors of this Program grant you additional permission to convey the resulting work.
// See <https://developers.arcgis.com/qt/> for further information.
//
#ifndef GDELTCALLOUTDATA_H
#define GDELTCALLOUTDATA_H

#include <QObject>

class GdeltCalloutData : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString uid READ uniqueId)
    Q_PROPERTY(QString title READ title)
    Q_PROPERTY(QString detail READ detail)
    Q_PROPERTY(QString link READ link)

public:
    explicit GdeltCalloutData(QObject *parent = nullptr);

    void setUniqueId(const QString& uid);
    QString uniqueId() const;

    void setTitle(const QString& title);
    QString title() const;

    void setDetail(const QString& detail);
    QString detail() const;

    void setLink(const QString& link);
    QString link() const;

signals:


private:
    QString m_uid;
    QString m_title;
    QString m_detail;
    QString m_link;
};

#endif // GDELTCALLOUTDATA_H
