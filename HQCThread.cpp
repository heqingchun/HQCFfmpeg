#include "HQCThread.h"

HQCCThread::HQCCThread(){
}
void HQCCThread::run(){
    Init();
    while(m_Enable){
        Work();
        msleep(10);
    }
}
void HQCCThread::AddWork(const QVariant& msg){
    if(m_Step == 0){
        m_Work = msg;
    }else{
        qDebug()<<"当前正在执行操作，无法添加新操作";
    }
}
void HQCCThread::PauseWork(){
    m_Step = 50;
}
void HQCCThread::ContinueWork(){
    m_Work.clear();
    m_Step = 0;
}
