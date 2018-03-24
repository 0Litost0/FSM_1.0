#ifndef __FSM_H__
#define __FSM_H__
/******************************************************************************
  文 件 名   : FSM.h
  版 本 号   : 初稿
  作    者   : Litost_Cheng
  生成日期   : 2018年1月10日
  最近修改   :
  功能描述   : FSM.cpp 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2018年1月10日
    作    者   : Litost_Cheng
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <fstream>
#include <map> 
#include<vector>
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <stdio.h>




/*----------------------------------------------*
 * 命名空间                               s  *
 *----------------------------------------------*/
using namespace std;

/*----------------------------------------------*
 * 宏定义                             *
 *----------------------------------------------*/



/*----------------------------------------------*
 * 宏定义                             *
 *----------------------------------------------*/
#define SAMPLE 0
//#define DEBUG
#define Dbg(fmt, args...) \
    DBGPrint(__FILE__,__FUNCTION__, __LINE__, fmt, ##args)
//定义一个空类
class CNullClass
{
public:
        CNullClass(){}
        ~CNullClass(){}
};

/*----------------------------------------------*
 * 函数指针声明                             *
 *----------------------------------------------*/
typedef unsigned int (*NormalFuncHandle)(const char *, unsigned int);


typedef int (CNullClass::*CallBack)(const char *, unsigned int);


//状态机系统事件
typedef enum tagFsmSysEvent
{   
    FSM_EVENT_SLEF_INSPECTION = 0x80000000        	 //状态自检测事件

}FsmSysState_e; 

/*----------------------------------------------*
 * 联合声明                             *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 结构体声明                             *
 *----------------------------------------------*/





typedef struct tag_ClassCallBack
{
    CNullClass* pClass;
    CallBack pCallBack;
}ClassCallBack_t;


typedef struct tagBind
{
    unsigned int m_unCurState;                 //现态（用索引指代具体的实例）
    unsigned int m_unNextState;                //次态
    unsigned int m_unEvent;                     //发生时间
    ClassCallBack_t m_ClassCallBack;            //类成员函数回调
    NormalFuncHandle m_pNormalHandle;             //动作句柄
}Bind_t;



typedef struct tag_CallBack
{
    //FIXME 此种写法有待验证
    tag_CallBack()
    {
        m_ClassHandle.pClass = NULL;
        m_ClassHandle.pCallBack = NULL;
        m_pNormalHandle = NULL;
    }

    ClassCallBack_t m_ClassHandle;            //类成员函数回调
    NormalFuncHandle m_pNormalHandle;           //一般回调
}CallBack_t;
/*----------------------------------------------*
 * 外部变量说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                       *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 类定义                                      *
 *----------------------------------------------*/
//状态类
class CState
{
    public:
        CState(unsigned int unIndex, const string &Desc);
        CState();
        CState(CState *pState);
        CState(const CState& State);
        ~CState();
        int SetEnterHandle(NormalFuncHandle pHandle); //设置进入事件的回调句柄
        int SetEnterHandle(void *pClass, CallBack pHandle); //设置进入事件的回调句柄

        int SetLeaveHandle(NormalFuncHandle pHandle); //设置离开事件的回调句柄
        int SetLeaveHandle(void *pClass, CallBack pHandle); //设置离开事件的回调句柄

        CState operator =(const CState& State)
        {
            m_unIndex = State.m_unIndex;
            m_strDesc = State.m_strDesc;
            m_stEnterCallBack = State.m_stEnterCallBack;
            m_stLeaveCallBack = State.m_stLeaveCallBack;
            return *this;
        }

     public:
        int _SetHandle(CallBack_t &uCallBack, NormalFuncHandle pHandle); //设置回调句柄
        int _SetHandle(CallBack_t &uCallBack, CallBack pHandle,  void *pClass); //设置回调句柄

        int Enter(const char *pData, unsigned int unDataLen);    //进入状态
        int Leave(const char *pData, unsigned int unDataLen);    //离开状态
        int Execute(CallBack_t &pCallBack, const char *pData, unsigned int unDataLen);  //执行回调
        int Modify();   //修改相应事件

     public:
        //XXX 以下变量属性暂时都设置为public
        unsigned int m_unIndex; //索引
        string m_strDesc; //状态描述
        CallBack_t m_stEnterCallBack; //进入状态事件句柄
        CallBack_t m_stLeaveCallBack; //离开状态事件句柄
};


//将线程锁进行封装
class CThreadLock
{
    public:
       CThreadLock()
       {
            if(pthread_mutex_init(&m_thread_lock, NULL) != 0)
            {
                //DBG("CThread_lock::CThreadLock pthread_mutex_init FAILED\n");
            }
       };
       ~CThreadLock()
        {
            if(pthread_mutex_destroy(&m_thread_lock) != 0)
            {
                //DBG("CThreadLock::~CThreadLock pthread_mutex_destroy FAILED\n");
            }
        };

    public:
        int Lock()
        {
            if(pthread_mutex_lock(&m_thread_lock) != 0)
            {
                //DBG("CThreadLock::Lock pthread_mutex_lock FAILED\n");
                return -1;
            }
            return 0;
        };

        int Unlock()
        {
            if(pthread_mutex_unlock(&m_thread_lock) != 0)
            {
                //DBG("CThreadLock::Unlock pthread_mutex_unlock FAILED\n");
                return -1;
            }

            return 0;
        };

        int Trylock()
        {
            if(pthread_mutex_trylock(&m_thread_lock) != 0)
            {
                //DBG("CThreadLock::Trylock pthread_mutex_trylock FAILED\n");
                return -1;
            }

            return 0;
        }

    private:
        pthread_mutex_t m_thread_lock;
};


//用以实现Vector事件表查询
//暂且不允许在状态切换的过程中，添加多个事件
class CAddFinder
{  
    public:  
        CAddFinder(Bind_t &Bint):m_Bind(Bint){}
        bool operator ()( vector<Bind_t>::value_type Value)  
        {  
            return ((Value.m_unCurState == m_Bind.m_unCurState)
                      && (Value.m_unEvent == m_Bind.m_unEvent));
        }  
    private:  
        Bind_t m_Bind;   
};  
class CUpgradeFinder
{  
    public:  
        CUpgradeFinder(Bind_t &Bint):m_Bind(Bint){}
        bool operator ()( vector<Bind_t>::value_type Value)  
        {  
            return ((Value.m_unCurState == m_Bind.m_unCurState) && (Value.m_unEvent == m_Bind.m_unEvent));  
        }  
    private:  
        Bind_t m_Bind;   
};  


class CFSM
{
    public:
        CFSM();
        ~CFSM();

        //状态更新
        unsigned int Upgrade(unsigned int unEvent, const char * pData, unsigned int unDataLen);
        
        //状态    
        unsigned int SetState(unsigned int unState);
        CState CurState();
        //其中将状态作为主键
        unsigned int AddState(unsigned int unState, string Describe = " ");
        unsigned int AddState(CState &State);
        unsigned int AddState(unsigned int unState, string Describe, CallBack_t &EnterCall, CallBack_t &LeaveCall);

        unsigned int ModifyState(CState &State, bool bAutoAdd = false);
        unsigned int DeleteState(unsigned int unState);
        CState QueryState(unsigned int unState);
        //其中将事件作为主键，描述作为非必须元素，应该限制输入描述符的最大长度
        unsigned int AddEvent(unsigned int unEvent, string Describe = " ");
        unsigned int DeleteEvent();
        unsigned int QueryEvent();
        //动作
        unsigned int AddAction();
        unsigned int DeleteAction();
        unsigned int QueryAction();

        //绑定
        unsigned int Banding(unsigned int unCurState, unsigned int unEvent,
                             unsigned int unNxtState, NormalFuncHandle pHandle);

        unsigned int Banding(unsigned int unCurState, unsigned int unEvent,
                             unsigned int unNxtState, void *pClsas, CallBack  PCallBack);


        //辅助
        bool StateHasExist(unsigned int unState);
        bool EventHasExist(unsigned int unEvent);
        
        //状态自检，用以处理当前状态的一些周期性事件
        unsigned int SelfInspection(const char * pData, unsigned int unDataLen);
        unsigned int InternalBanding (unsigned int unCurState, NormalFuncHandle pHandle);

        unsigned int  InternalBanding (unsigned int unCurState, void *pClsas, CallBack PCallBack);

        
        
               
    private:
        std::map<unsigned int, CState *> m_mapState;   //保存状态   //FIXME 对它的操作是否需要确保是线程安全的呢？
        std::map<unsigned int, string> m_mapEvent;   //保存事件
        std::map<unsigned int, string> m_mapAction;   //保存动作      
        
        std::vector<Bind_t> m_vecBindTable;         //事件表，因为本状态机本质上还是事件驱动的

        CState *m_pCurState;                     //当前状态

        CThreadLock* m_pStateLock;                  //状态切换锁 状态改变时，需要加上相应的锁

        
};

#endif /* __FSM_H__ */
