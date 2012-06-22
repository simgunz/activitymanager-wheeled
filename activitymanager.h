/***************************************************************************
* Copyright © 2010 Abdurrahman AVCI <abdurrahmanavci@gmail.com
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the
* Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
***************************************************************************/

#ifndef ACTIVITYMANAGER_H
#define ACTIVITYMANAGER_H

#include <Plasma/DataEngine>
#include <Plasma/PopupApplet>

#include <QHash>

class ActivityWidget;

class QAction;

namespace Plasma {
  class ExtenderItem;
};

class ActivityManager : public Plasma::PopupApplet {
  Q_OBJECT
public:
  ActivityManager(QObject *, const QVariantList &);

  void init();
  void initExtenderItem(Plasma::ExtenderItem *item);

public slots:
  void toolTipAboutToShow();
  void dataUpdated(QString source, Plasma::DataEngine::Data data);
  void activityAdded(QString id);
  void activityRemoved(QString id);

  void add(QString id);
  void add(QString id, QString name);
  void setCurrent(QString id);
  void stop(QString id);
  void start(QString id);
  void setName(QString id, QString name);
  void setIcon(QString id, QString name);
  void remove(QString id);

protected slots:
  virtual void wheelEvent(QGraphicsSceneWheelEvent *event);

private slots:
  void toggleLock();

private:
  void sortActivities();

  QHash<QString, ActivityWidget *> m_activities;
  QAction *toggleLockAction;
  QString m_currentName;
  QString m_currentIcon;
};

#endif
