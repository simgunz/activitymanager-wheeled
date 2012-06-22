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

#include "activitymanager.h"

#include "activitywidget.h"

#include <Plasma/Extender>
#include <Plasma/ExtenderItem>
#include <Plasma/Service>
#include <Plasma/ServiceJob>
#include <Plasma/ToolTipContent>
#include <Plasma/ToolTipManager>

#include <QAction>
#include <QGraphicsLinearLayout>
#include <QString>
#include <QGraphicsSceneWheelEvent>

ActivityManager::ActivityManager(QObject *parent, const QVariantList &args): Plasma::PopupApplet(parent, args), toggleLockAction(0), m_currentName(""), m_currentIcon("") {
  setPopupIcon("preferences-activities");
  setAspectRatioMode(Plasma::IgnoreAspectRatio);
}

void ActivityManager::init() {
  Plasma::ToolTipManager::self()->registerWidget(this);
  extender()->setEmptyExtenderMessage(i18n("No Activities..."));
  // don't grow too much height
  extender()->setMaximumHeight(300);
  if (extender()->item("Activities") == 0) {
    // create the item
    Plasma::ExtenderItem *item = new Plasma::ExtenderItem(extender());
    // initialize the item
    initExtenderItem(item);
    // set item name and title
    item->setName("Activities");
    item->setTitle("Activities");
  }
  // connect data sources
  Plasma::DataEngine *engine = dataEngine("org.kde.activities");
  foreach (const QString source, engine->sources())
    activityAdded(source);
  // activity addition and removal
  connect(engine, SIGNAL(sourceAdded(QString)), this, SLOT(activityAdded(QString)));
  connect(engine, SIGNAL(sourceRemoved(QString)), this, SLOT(activityRemoved(QString)));
}

void ActivityManager::initExtenderItem(Plasma::ExtenderItem *item) {
  // create the widget
  QGraphicsWidget *widget = new QGraphicsWidget(this);
  // TODO: use the size of the longest activity name
  widget->setPreferredWidth(350);
  // create the layout
  QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(widget);
  layout->setOrientation(Qt::Vertical);
  widget->setLayout(layout);
  // set up the widget
  item->setWidget(widget);
  // create a lock/unlock action
  toggleLockAction = new QAction(item);
  toggleLockAction->setIcon(KIcon("object-locked"));
  toggleLockAction->setEnabled(true);
  toggleLockAction->setVisible(true);
  toggleLockAction->setToolTip(i18n("Activities are unlocked. Click to lock."));
  item->addAction("toggleLock", toggleLockAction);
  connect(toggleLockAction, SIGNAL(triggered()), this, SLOT(toggleLock()));
}

void ActivityManager::toolTipAboutToShow() {
  Plasma::ToolTipContent toolTip;

  toolTip.setMainText(i18n("Current Activity: %1").arg(m_currentName));
  if (!m_currentIcon.isEmpty())
    toolTip.setImage(KIcon(m_currentIcon));

  Plasma::ToolTipManager::self()->setContent(this, toolTip);
}

void ActivityManager::dataUpdated(QString source, Plasma::DataEngine::Data data) {
  if (!m_activities.contains(source))
    return;
  ActivityWidget *activity = m_activities[source];
  // update activity info
  activity->setName(data["Name"].toString());
  activity->setState(data["State"].toString());
  activity->setIcon(data["Icon"].toString());
  activity->setCurrent(data["Current"].toBool());
  // update current activity name and icon
  if (data["Current"].toBool()) {
    m_currentName = data["Name"].toString();
    m_currentIcon = data["Icon"].toString();
  }
  // sort activities
  sortActivities();
}

void ActivityManager::activityAdded(QString id) {
  // skip the Status source
  if (id == "Status")
    return;
  // create a new activity object
  ActivityWidget *activity = new ActivityWidget(extender()->item("Activities"), id);
  // add activity to the list
  m_activities.insert(id, activity);
  // connect activity update signal
  dataEngine("org.kde.activities")->connectSource(id, this);
  // connect activity start/stop signals
  connect(activity, SIGNAL(setCurrent(QString)), this, SLOT(setCurrent(QString)));
  connect(activity, SIGNAL(startActivity(QString)), this, SLOT(start(QString)));
  connect(activity, SIGNAL(stopActivity(QString)), this, SLOT(stop(QString)));
  connect(activity, SIGNAL(addActivity(QString)), this, SLOT(add(QString)));
  connect(activity, SIGNAL(removeActivity(QString)), this, SLOT(remove(QString)));
  connect(activity, SIGNAL(renameActivity(QString,QString)), this, SLOT(setName(QString,QString)));
}

void ActivityManager::activityRemoved(QString id) {
  if (!m_activities.contains(id))
    return;
  // delete the activity
  delete m_activities.take(id);
}

void ActivityManager::add(QString id) {
  add(id, "New Activity");
}

void ActivityManager::add(QString id, QString name) {
  Plasma::Service *service = dataEngine("org.kde.activities")->serviceForSource(id);
  KConfigGroup op = service->operationDescription("add");
  op.writeEntry("Name", name);
  Plasma::ServiceJob *job = service->startOperationCall(op);
  connect(job, SIGNAL(finished(KJob*)), service, SLOT(deleteLater()));
}

void ActivityManager::setCurrent(QString id) {
  Plasma::Service *service = dataEngine("org.kde.activities")->serviceForSource(id);
  Plasma::ServiceJob *job = service->startOperationCall(service->operationDescription("setCurrent"));
  connect(job, SIGNAL(finished(KJob*)), service, SLOT(deleteLater()));
}

void ActivityManager::stop(QString id) {
  // TODO: when activity is stopped, take a screenshot and use that icon
  Plasma::Service *service = dataEngine("org.kde.activities")->serviceForSource(id);
  Plasma::ServiceJob *job = service->startOperationCall(service->operationDescription("stop"));
  connect(job, SIGNAL(finished(KJob*)), service, SLOT(deleteLater()));
}

void ActivityManager::start(QString id) {
  Plasma::Service *service = dataEngine("org.kde.activities")->serviceForSource(id);
  Plasma::ServiceJob *job = service->startOperationCall(service->operationDescription("start"));
  connect(job, SIGNAL(finished(KJob*)), service, SLOT(deleteLater()));
}

void ActivityManager::setName(QString id, QString name) {
  Plasma::Service *service = dataEngine("org.kde.activities")->serviceForSource(id);
  KConfigGroup op = service->operationDescription("setName");
  op.writeEntry("Name", name);
  Plasma::ServiceJob *job = service->startOperationCall(op);
  connect(job, SIGNAL(finished(KJob*)), service, SLOT(deleteLater()));
}

void ActivityManager::setIcon(QString id, QString name) {
  Plasma::Service *service = dataEngine("org.kde.activities")->serviceForSource(id);
  KConfigGroup op = service->operationDescription("setIcon");
  op.writeEntry("Icon", name);
  Plasma::ServiceJob *job = service->startOperationCall(op);
  connect(job, SIGNAL(finished(KJob*)), service, SLOT(deleteLater()));
}

void ActivityManager::remove(QString id) {
  Plasma::Service *service = dataEngine("org.kde.activities")->serviceForSource(id);
  KConfigGroup op = service->operationDescription("remove");
  op.writeEntry("Id", id);
  Plasma::ServiceJob *job = service->startOperationCall(op);
  connect(job, SIGNAL(finished(KJob*)), service, SLOT(deleteLater()));
}

void ActivityManager::toggleLock() {
  if (toggleLockAction->toolTip() == i18n("Activities are locked. Click to unlock.")) {
    toggleLockAction->setIcon(KIcon("object-unlocked"));
    toggleLockAction->setToolTip(i18n("Activities are unlocked. Click to lock."));
    // unlock activities
    foreach (ActivityWidget *activity, m_activities)
      activity->unlock();
  } else {
    toggleLockAction->setIcon(KIcon("object-locked"));
    toggleLockAction->setToolTip(i18n("Activities are locked. Click to unlock."));
    // lock activities
    foreach (ActivityWidget *activity, m_activities)
      activity->lock();
  }
}

void ActivityManager::sortActivities() {
  QGraphicsLinearLayout *layout = static_cast<QGraphicsLinearLayout *>(static_cast<QGraphicsWidget *>(extender()->item("Activities")->widget())->layout());
  // remove all activities from the layout
  while (layout->count())
    layout->removeAt(0);
  // insert all activities to a map
  QMap<QString, ActivityWidget *> activities;
  foreach (ActivityWidget *activity, m_activities.values())
    activities.insertMulti(activity->name(), activity);
  // add activities into the layout at ascending order of their names
  foreach (ActivityWidget *activity, activities.values())
    layout->addItem(activity);
}

void ActivityManager::wheelEvent(QGraphicsSceneWheelEvent *e)
{
    Plasma::DataEngine *engine = dataEngine("org.kde.activities");
    Plasma::DataEngine::Data data = engine->query("Status");
    QStringList activities = data["Running"].toStringList();
    QString currentActivity = data["Current"].toString();

    int nextIndex = activities.indexOf(currentActivity);

    if(e->delta() < 0)
        nextIndex = (nextIndex + 1) % activities.size();
    else
    {
        if(nextIndex - 1 < 0)
            nextIndex = activities.size() - 1;
        else
            nextIndex = nextIndex - 1;
    }

    Plasma::Service *service = engine->serviceForSource(activities[nextIndex]);
    service->startOperationCall(service->operationDescription("setCurrent"));
}

K_EXPORT_PLASMA_APPLET(activitymanager, ActivityManager)
