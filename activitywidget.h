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

#ifndef ACTIVITYWIDGET_H
#define ACTIVITYWIDGET_H

#include <QGraphicsWidget>
#include <QString>

namespace Plasma {
  class IconWidget;
  class LineEdit;
};

class QGraphicsGridLayout;

class ActivityWidget : public QGraphicsWidget {
  Q_OBJECT
public:
  ActivityWidget(QGraphicsItem *parent, QString id);

  QString name();
  void setName(QString name);
  void setIcon(QString icon);
  void setCurrent(bool current);
  void setState(QString state);

  void lock();
  void unlock();

public slots:
  void setCurrent();
  void toggleStatus();
  void beginRemove();
  void acceptRemove();
  void cancelRemove();
  void beginEdit();
  void acceptEdit();
  void cancelEdit();
  void beginAdd();

signals:
  void setCurrent(QString id);
  void startActivity(QString id);
  void stopActivity(QString id);
  void addActivity(QString id);
  void removeActivity(QString id);
  void renameActivity(QString id, QString name);

private:
  QGraphicsGridLayout *m_layout;
  QGraphicsWidget *m_actionsWidget;
  QGraphicsWidget *m_removeWidget;
  QGraphicsWidget *m_editWidget;
  Plasma::LineEdit *m_lineEdit;
  Plasma::IconWidget *m_label;
  Plasma::IconWidget *m_stateIcon;
  Plasma::IconWidget *m_addIcon;
  Plasma::IconWidget *m_editIcon;
  Plasma::IconWidget *m_removeIcon;
  QString m_id;
  QString m_name;
  QString m_state;
  bool m_dialogShown;
};

#endif
