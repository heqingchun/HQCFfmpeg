#ifndef HQCCTHREAD_H
#define HQCCTHREAD_H

#include <QObject>
#include <QThread>
#include <QVariant>
#include <QDebug>

class HQCCThread : public QThread
{
    Q_OBJECT
signals:
    void signalFinish(const QVariant& msg);
public:
    HQCCThread();
protected:
    virtual void Init() = 0;
    virtual void Work() = 0;
public:
    virtual void AddWork(const QVariant& msg);
    virtual void PauseWork();
    virtual void ContinueWork();
protected:
    void run() override;
public:
    bool m_Enable = true;
    int m_Step = 0;
    QVariant m_Work;
};

#endif // HQCCTHREAD_H
