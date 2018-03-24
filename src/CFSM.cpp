/******************************************************************************
  文 件 名   : FSM.cpp
  版 本 号   : 初稿
  作    者   : Litost_Cheng
  生成日期   : 2018年1月10日
  最近修改   :
  功能描述   : 自定义状态机
  函数列表   :
  修改历史   :
  1.日    期   : 2018年1月10日
    作    者   : Litost_Cheng
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "CFSM.h"
/*----------------------------------------------*
 * 命名空间                                 *
 *----------------------------------------------*/
using namespace std;







void DBGPrint(const char *file, const char *func, int line, const char *format, ...)
{

#ifdef DEBUG
    va_list args;
    int nCnt = 0;

    if(file == NULL || format == NULL || func == NULL)
        return ;

    time_t timep;
    struct tm *t;
    time(&timep);
    t = gmtime(&timep);

    //截取最短路径
    char *pFileName = strrchr(file, '/');
    if(pFileName != NULL && *(pFileName+1) != 0x00)
    {
        pFileName ++;

        printf("[%s][%s][%d][%04d%02d%02d%02d%02d%02d]", pFileName, func, line, (t->tm_year+1900), (t->tm_mon + 1), t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    }
    else
    {
        printf("[%s][%s][%d][%04d%02d%02d%02d%02d%02d]", file, func, line, (t->tm_year+1900), (t->tm_mon + 1), t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    }

    va_start(args,format);
    nCnt = vprintf(format,args);
    va_end(args);
#else
    //Do nothing
#endif

}

/*----------------------------------------------*
 * 枚举声明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 结构体声明                             *
 *----------------------------------------------*/

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
 * 函数实现                                       *
 *----------------------------------------------*/


CFSM::CFSM()
{
    m_mapState.clear();
    m_mapEvent.clear();
    m_mapAction.clear();
    m_vecBindTable.clear();
    m_pCurState = NULL;              //当前状态默认为空

    
	m_pStateLock = new CThreadLock();
    //FIXME 添加该系统事件
    AddEvent(FSM_EVENT_SLEF_INSPECTION, "FSM_EVENT_SLEF_INSPECTION");
}

CFSM::~CFSM()
{
    std::map<unsigned int, CState *>::iterator iterator = m_mapState.begin();

    for(;iterator != m_mapState.end(); iterator++)
    {
        //FIXME 释放之前申请的资源
        if (NULL != iterator->second)
        {
            delete iterator->second;
        }
    }
}
/*****************************************************************************
 函 数 名  : FSM.SetState
 功能描述  : 设置状态机当前状态，设置之前，需要确定该状态存在；一般用于设置状态机的初始状态
 输入参数  : unsigned int unState  
 输出参数  : 无
 返 回 值  : unsigned
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年1月11日
    作    者   : Litost_Cheng
    修改内容   : 新生成函数

*****************************************************************************/
unsigned int CFSM::SetState(unsigned int unState)
{
    if (StateHasExist(unState))
    {
        m_pStateLock->Lock();
        //FIXME 需要执行离开当前状态，以及进入
        m_pCurState = m_mapState[unState];
    	m_pStateLock->Unlock();
        Dbg("CurState [0X%08X],describe is [%s]\n", m_pCurState, m_pCurState->m_strDesc.c_str());
        return 0;
    }
    else
    {      
        Dbg("State [0X%08X] isn't exist!\n", unState);
        return -1;
    }
}	
/*****************************************************************************
 函 数 名  : FSM.CurState
 功能描述  : 获取当前状态机状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : CState
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年3月19日
    作    者   : Litost_Cheng
    修改内容   : 新生成函数
	
*****************************************************************************/
CState CFSM::CurState()
{
    //FIXME 此种方式有待验证
    if (NULL == m_pCurState)
    {
        Dbg("***NULL == m_pCurState***\n");
        exit (1);
    }
    CState TmpState(m_pCurState);
    Dbg("CurState is [0X%08X], Describe is [%s]\n",m_pCurState, m_pCurState->m_strDesc.c_str());
    return TmpState;
}


/*****************************************************************************
 函 数 名  : StateHasExist
 功能描述  : 检测当前状态是否存在
 输入参数  : unsigned int unState  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年1月10日
    作    者   : Litost_Cheng
    修改内容   : 新生成函数
	
*****************************************************************************/
bool CFSM::StateHasExist(unsigned int unState)
{
    std::map<unsigned int, CState *>::iterator iter = m_mapState.find(unState);
    if(iter != m_mapState.end())//有重名的
    {
        //XXX 有待验证
        Dbg("The State [0X%08X], Describe is [%s] has exist!\n",unState, iter->second->m_strDesc.c_str());
        return true;
    }
    else //没有
    {
        return false;
    }  
}
/*****************************************************************************
 函 数 名  : EventHasExist
 功能描述  : 检测当前事件是否存在
 输入参数  : unsigned int unEvent  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年1月10日
    作    者   : Litost_Cheng
    修改内容   : 新生成函数
	
*****************************************************************************/
bool CFSM::EventHasExist(unsigned int unEvent)
{
    std::map<unsigned int, string>::iterator iter=m_mapEvent.find(unEvent);
    if(iter != m_mapEvent.end())//有重名的
    {
        Dbg("The Event [0X%08X], Describe is [%s] has exist!\n",unEvent, iter->second.c_str());
        return true;
    }
    else //没有
    {
        Dbg("The event isn't exist\n");
        return false;
    }      
}
/*****************************************************************************
 函 数 名  : FSM.AddState
 功能描述  : 添加新的状态，在添加之前，需要检测当前状态是否存在
 输入参数  : unsigned int unState  
             string Describe = ""  
 输出参数  : 无
 返 回 值  : unsigned
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年1月10日
    作    者   : Litost_Cheng
    修改内容   : 新生成函数
	
*****************************************************************************/
unsigned int CFSM::AddState(unsigned int unState, string Describe)
{
    if (StateHasExist(unState))
    {
        return -1;
    }
    CState *pState = new CState(unState, Describe);
    m_mapState[unState] = pState;
    //FIXME 未检测代码
    Dbg("State [0X%08X-%s] add succeed\n", unState, Describe.c_str());
    return 0;
}

unsigned int CFSM::AddState(CState &State)
{
    if (StateHasExist(State.m_unIndex))
    {
        return -1;
    }

    //有待验证
    CState *pState = new CState(State);
    m_mapState[pState->m_unIndex] = pState;
    //FIXME 未检测代码
    Dbg("State [0X%08X-%s] add succeed\n", State.m_unIndex, State.m_strDesc.c_str());
    return 0;
}
unsigned int CFSM::AddState(unsigned int unState, string Describe, CallBack_t &EnterCall, CallBack_t &LeaveCall)
{
    if (StateHasExist(unState))
    {
        return -1;
    }
    CState *pState = new CState(unState, Describe);
    //FIXME 有待验证
    pState->m_stEnterCallBack = EnterCall;
    pState->m_stLeaveCallBack = LeaveCall;
    m_mapState[unState] = pState;
    //FIXME 未检测代码
    Dbg("State [0X%08X-%s] add succeed\n", unState, Describe.c_str());
    return 0;
}



unsigned int CFSM::ModifyState(CState &State, bool bAutoAdd)
{
    int nRst = 0;
    if (!StateHasExist(State.m_unIndex))
    {
        if (bAutoAdd)
        {
            AddState(State);
            return 1;
        }
        else
        {
            return -1;
        }
    }
    CState *pState = m_mapState[State.m_unIndex];
    pState->m_strDesc = State.m_strDesc;
    pState->m_stEnterCallBack = State.m_stEnterCallBack;
    pState->m_stLeaveCallBack = State.m_stLeaveCallBack;
    return 0;
}

unsigned int CFSM::DeleteState(unsigned int unState)
{
    if (StateHasExist(unState))
    {
        return -1;
    }

    //XXX 有待验证
    CState *pState = m_mapState[unState];
    delete pState;
    //FIXME 删除之前是否应该先释放资源呢？
    m_mapState.erase(unState);
    return 0;

}

CState CFSM::QueryState(unsigned int unState)
{
    if (!StateHasExist(unState))
    {
        CState State;
        return State;
    }
    else
    {
        //XXX 有待验证
        CState TmpState(m_mapState[unState]);
        //FIXME 删除之前是否应该先释放资源呢？
        return TmpState;
    }
}

/****************************************
*@工程名   FSM
*@简介

*@返回值

*@作者        Litost_Cheng
*@日期          2018年03月19日

****************************************/

/*****************************************************************************
 函 数 名  : AddEvent
 功能描述  : 添加新的状态，在添加之前，需要检测当前事件件是否存在
 输入参数  : unsigned int unEvent  
             string Describe = ""  
 输出参数  : 无
 返 回 值  : unsigned
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年1月10日
    作    者   : Litost_Cheng
    修改内容   : 新生成函数
	
*****************************************************************************/
unsigned int CFSM::AddEvent(unsigned int unEvent, string Describe)
{
    if (EventHasExist(unEvent))
    {
        return -1;
    }
    else
    {
        m_mapEvent[unEvent] = Describe;
        //FIXME 未检测代码
        Dbg("Event [0X%08X-%s] add succeed\n", unEvent, m_mapEvent[unEvent].c_str());
        
    }   
    return 0;
}
	
/*****************************************************************************
 函 数 名  : Banding
 功能描述  : 完成一条状态绑定
 输入参数  : unsigned int unCurState  
             unsigned int unNxtState  
             unsigned int unEvent     
             NormalFuncHandle pHandle       
 输出参数  : 无
 返 回 值  : unsigned
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年1月10日
    作    者   : Litost_Cheng
    修改内容   : 新生成函数
	
*****************************************************************************/
unsigned int CFSM::Banding(unsigned int unCurState, unsigned int unEvent, unsigned int unNxtState, NormalFuncHandle pHandle)
{
     if (!StateHasExist(unCurState))
    {
        Dbg("CurState[0X%08X] is not exist!\n", unCurState);
        return -1;
    }  
     if (!StateHasExist(unNxtState))
    {
        Dbg("unNxtState[0X%08X] is not exist!\n", unNxtState);
        return -2;
    }
    if (!EventHasExist(unEvent))
    {
        return -3;
    }
    if (NULL == pHandle)
    {
        Dbg("NUll == pHandle\n");
        return -4;
    }
    Bind_t Bind;
    Bind.m_unCurState = unCurState;
    Bind.m_unNextState = unNxtState;  
    Bind.m_unEvent = unEvent;
    //XXX 新添加
    Bind.m_ClassCallBack.pClass = NULL;
    Bind.m_ClassCallBack.pCallBack = NULL;
    
    Bind.m_pNormalHandle = pHandle;
    //XXX 有待验证
    vector<Bind_t>::iterator result = find_if(m_vecBindTable.begin(), m_vecBindTable.end(), CAddFinder(Bind)); //查找是否存在当前元素
     //XXX 有待验证
     if ( result == m_vecBindTable.end( ) ) //没找到
     {
        m_vecBindTable.push_back(Bind);
        Dbg("Binding:CurState[0X%08X-%s]======Event[0X%08X-%s]=====>NextState[0X%08X-%s], Action[0X%08X]\n",
                           unCurState, m_mapState[unCurState]->m_strDesc.c_str(), unEvent, m_mapEvent[unEvent].c_str(),
                            unNxtState, m_mapState[unNxtState]->m_strDesc.c_str(), Bind.m_pNormalHandle);
        return 0;
     }       
    else //找到
    {
        Dbg("The element Has exist!\n");
        return -4;
    }
    return 0;
}

/*****************************************************************************
 函 数 名  : FSM.Banding
 功能描述  : 类成员回调函数
 输入参数  : unsigned int unCurState          
             unsigned int unNxtState          
             unsigned int unEvent             
             ClassCallBack_t & ClassCallBack  
 输出参数  : 无
 返 回 值  : unsigned
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年1月15日
    作    者   : Litost_Cheng
    修改内容   : 新生成函数

*****************************************************************************/
//FIXME 为了获取更好的适配性，最好是可以将参数类型都设为void型，然后再函数内部进行强转操作
//FIXME 上边的同名重构函数，可以开绿将其取代
 unsigned int CFSM::Banding(unsigned int unCurState, unsigned int unEvent,
                            unsigned int unNxtState, void *pClsas, CallBack  PCallBack)
{
     if (!StateHasExist(unCurState))
    {
        return -1;
    }  
     if (!StateHasExist(unNxtState))
    {
        return -2;
    }
     //FIXME 之前错误的原因是没有添加系统事件
    if (!EventHasExist(unEvent))
    {
        return -3;
    }
    if (NULL == pClsas || NULL == PCallBack)
    {
        Dbg("NULL == pClsas || NULL == PCallBack\n");
        return -4;
    }
    Bind_t Bind;
    Bind.m_unCurState = unCurState;
    Bind.m_unNextState = unNxtState;  
    Bind.m_unEvent = unEvent;
    
    Bind.m_ClassCallBack.pClass = (CNullClass*) pClsas;
    Bind.m_ClassCallBack.pCallBack = (CallBack)PCallBack;

    Bind.m_pNormalHandle = NULL;

	
     vector<Bind_t>::iterator result = find_if(m_vecBindTable.begin(), m_vecBindTable.end(), CAddFinder(Bind)); //查找是否存在当前元素
     //XXX 有待验证
     if ( result == m_vecBindTable.end( ) ) //没找到
     {
        m_vecBindTable.push_back(Bind);
        Dbg("Binding:CurState[0X%08X-%s]======Event[0X%08X-%s]=====>NextState[0X%08X-%s], Action[0X%08X::0X%08X]\n",
                           unCurState, m_mapState[unCurState]->m_strDesc.c_str(), unEvent, m_mapEvent[unEvent].c_str(),
                            unNxtState, m_mapState[unNxtState]->m_strDesc.c_str(),  Bind.m_ClassCallBack.pClass ,Bind.m_ClassCallBack.pCallBack);
        return 0;
     }       
    else //找到
    {
        Dbg("The element Has exist!\n");
        return -4;
    }    
    return 0;
}

/*****************************************************************************
 函 数 名  : Upgrade
 功能描述  : 输入发生事件，进行状态迁移
 输入参数  : unsigned int unEvent  
 输出参数  : 无
 返 回 值  : unsigned
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年1月10日
    作    者   : Litost_Cheng
    修改内容   : 新生成函数
	
*****************************************************************************/
unsigned int CFSM::Upgrade(unsigned int unEvent, const char * pData, unsigned int unDataLen)
{
//	    if (NULL == pData)
//	    {
//	        DBG("pData is NULL\n");
//	        return -1;
//	    }
    if (EventHasExist(unEvent))
    {
        
        Bind_t Bind;
	
        Dbg("m_unCurState is [0X%08X - %s]\n", m_pCurState, m_pCurState->m_strDesc.c_str());

        //XXX 离开当前状态前，判断是否需要执行相应动作

        Bind.m_unCurState = m_pCurState->m_unIndex;
        Bind.m_unEvent = unEvent;
        vector<Bind_t>::iterator pRst = m_vecBindTable.begin();
     
        pRst = find_if(m_vecBindTable.begin(), m_vecBindTable.end(), CUpgradeFinder(Bind)); //查找是否存在当前元素
		if (pRst == m_vecBindTable.end())
		{
            Dbg("Find nothing：CurState[0X%08X-%s]===//===Event[0X%08X-%s]\n", m_pCurState, m_mapState[m_pCurState->m_unIndex]->m_strDesc.c_str(),
                                unEvent, m_mapEvent[unEvent].c_str());
			return -1;
		}

        if (NULL != m_pCurState)
        {
            m_pCurState->Leave(pData, unDataLen);
        }

		m_pStateLock->Lock();
		//FIXME 由于在动作内，可能涉及新的状态切换，因此在执行动作前，要求先切换状态
		//FIXME 其实正确的做法应该是严禁在动作出现状态切换代码，否则的话，状态一旦切换，会导致后续的动作无法执行
        m_pCurState = m_mapState[pRst->m_unNextState];
        m_pStateLock->Unlock();
        vector<Bind_t>::iterator pStart = pRst;         //XXX 此处有待验证
    
        while (pRst != m_vecBindTable.end())
        {
            pRst = find_if(pStart, m_vecBindTable.end(), CUpgradeFinder(Bind)); //查找是否存在当前元素
            if (pRst != m_vecBindTable.end())
            {
                //XXX 切换状态

                //XXX 并非所有的状态切换，都会伴随相应动作
                if (NULL == pRst->m_ClassCallBack.pClass && NULL != pRst->m_pNormalHandle)  //普通回调
                { 
                    //此处还应该添加对普通函数的处理过程
                    Dbg("\nExect result is %d\n", pRst->m_pNormalHandle(pData, unDataLen));
                    Dbg("Upgrade:CurState[0X%08X-%s]======Event[0X%08X-%s]=====>NextState[0X%08X-%s],  Action[0X%08X]\n",
                                       pRst->m_unCurState, m_mapState[pRst->m_unCurState]->m_strDesc.c_str(), pRst->m_unEvent, m_mapEvent[pRst->m_unEvent].c_str(),
                                        pRst->m_unNextState, m_mapState[pRst->m_unNextState]->m_strDesc.c_str(),  pRst->m_pNormalHandle);

                }
                if (NULL != pRst->m_ClassCallBack.pClass && NULL != pRst->m_ClassCallBack.pCallBack)   //类的成员函数回调
                {                    
                    Dbg("\nExect result is %d\n", (pRst->m_ClassCallBack.pClass->*(pRst->m_ClassCallBack.pCallBack))(pData, unDataLen));
                    Dbg("Upgrade:CurState[0X%08X-%s]======Event[0X%08X-%s]=====>NextState[0X%08X-%s],  Action[0X%08X::0X%08X]\n",
                                       pRst->m_unCurState, m_mapState[pRst->m_unCurState]->m_strDesc.c_str(), pRst->m_unEvent, m_mapEvent[pRst->m_unEvent].c_str(),
                                        pRst->m_unNextState, m_mapState[pRst->m_unNextState]->m_strDesc.c_str(),  pRst->m_ClassCallBack.pClass,
                                        pRst->m_ClassCallBack.pCallBack);

                }
                //FIXME 打印输出存在问题
                pStart = pRst;   
                pStart++;
            }
        }

        //XXX 离开当前状态前，判断是否需要执行相应动作
        if (NULL != m_pCurState)
        {
            m_pCurState->Enter(pData, unDataLen);
        }
    }
    else
    {
        Dbg("Upgrade failed!\n");
        return -2;
    }
    //return 0;
	    
}
//FIXME 一下都为未经验证的代码
/*****************************************************************************
 函 数 名  : FSM.SelfInspection
 功能描述  : 处理当前状态下的一些周期性事件（关键在于该函数是应该如何触发呢？具体周期由你外部决定）
 输入参数  : 无
 输出参数  : 无
 返 回 值  : unsigned
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年1月16日
    作    者   : Litost_Cheng
    修改内容   : 新生成函数

*****************************************************************************/
//XXX 具体效果有待确认
unsigned int  CFSM::SelfInspection(const char * pData, unsigned int unDataLen)
{
	//XXX 临时打印
	CurState();
	Upgrade(FSM_EVENT_SLEF_INSPECTION, pData, unDataLen);
	return 0;
}
/*****************************************************************************
 函 数 名  : FSM.InternalBanding
 功能描述  : 状态自检过程中，执行的周期性事件
 输入参数  : unsigned int unCurState  
             void *pClass             
             FuncHandle pHandle       
 输出参数  : 无
 返 回 值  : unsigned
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年1月17日
    作    者   : Litost_Cheng
    修改内容   : 新生成函数

*****************************************************************************/
unsigned int CFSM::InternalBanding (unsigned int unCurState, NormalFuncHandle pHandle)
{
    if (0 ==Banding(unCurState, unCurState, FSM_EVENT_SLEF_INSPECTION, pHandle))
    {
        return 0;
    }
    else
    {
        return -1;
    }
    

}
/*****************************************************************************
 函 数 名  : FSM.InternalBanding
 功能描述  : 系统内置事件绑定（需要先判定当前状态是否存在）
 输入参数  : unsigned int unCurState  
             void *pClsas             
             CallBack PCallBack       
 输出参数  : 无
 返 回 值  : unsigned
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年1月18日
    作    者   : Litost_Cheng
    修改内容   : 新生成函数

*****************************************************************************/
unsigned int CFSM::InternalBanding (unsigned int unCurState, void *pClsas, CallBack PCallBack)
{
    Dbg("***InternalBanding Start: Curstate is [0X%08X]***\n", unCurState);
    if (0 ==Banding(unCurState, unCurState, FSM_EVENT_SLEF_INSPECTION, pClsas, PCallBack))
    {
        return 0;
    }
    else
    {
        Dbg("InternalBanding Error\n");
        return -1;
    }
    
    Dbg("***InternalBanding End: Curstate is [0X%08X]***\n", unCurState);

}






//测试例程
#if SAMPLE==1

unsigned int Act_1(const char * pData, unsigned int unDataLen)
{
    DBG("***I am Act_1, Data is [%s]***\n", pData);
    return 0;
}
unsigned int Act_2(const char * pData, unsigned int unDataLen)
{
    DBG("---I am Act_2, Data is [%s]---\n", pData);
    return 0;
}
unsigned int Act_3(const char * pData, unsigned int unDataLen)
{
    DBG("---I am Act_3, Data is [%s]---\n", pData);
    return 0;
}


int main(int argc, char **argv)
{



    FSM TestFsm;
    NormalFuncHandle Handle_1 = Act_1;
    NormalFuncHandle Handle_2 = Act_2;
    NormalFuncHandle Handle_3 = Act_3;
  
    const unsigned int STATE_1 = 0x01;    
    const unsigned int STATE_2 = 0x02;
    const unsigned int STATE_3 = 0x03;
    
    const unsigned int EVENT_12 = 12;
    const unsigned int EVENT_13 = 13;
    const unsigned int EVENT_21 = 21;
    const unsigned int EVENT_23 = 23;
    const unsigned int EVENT_31 = 31;
    const unsigned int EVENT_32 = 32;
    
    TestFsm.AddState(STATE_1, "STATE_1");
    TestFsm.AddState(STATE_2, "STATE_2");
    TestFsm.AddState(STATE_3, "STATE_3");
    
    TestFsm.AddEvent(EVENT_12, "EVENT_12"); 
    TestFsm.AddEvent(EVENT_13, "EVENT_13");
    TestFsm.AddEvent(EVENT_21, "EVENT_21");
    TestFsm.AddEvent(EVENT_23, "EVENT_23");
    TestFsm.AddEvent(EVENT_31, "EVENT_31");
    TestFsm.AddEvent(EVENT_32, "EVENT_32");   

    
    TestFsm.Banding(STATE_1, STATE_2, EVENT_12, Handle_1);
    TestFsm.Banding(STATE_1, STATE_3, EVENT_13, Handle_1);    

    
    TestFsm.Banding(STATE_2, STATE_1, EVENT_21, Handle_2);
    TestFsm.Banding(STATE_2, STATE_3, EVENT_23, Handle_2);     

    TestFsm.Banding(STATE_3, STATE_1, EVENT_31, Handle_3);
    TestFsm.Banding(STATE_3, STATE_2, EVENT_32, Handle_3); 


    TestFsm.CurState();
    TestFsm.SetState(STATE_1);    
    TestFsm.CurState();

	unsigned int  Event = 0;
    printf("Please Input the New Event:\n");
	
	while(1)
	{
		Event = 0;
		if ((scanf("%d", &Event)))
		{
			switch(Event)
			{
				case 12:
                case 13: 
				case 21:
                case 23:
				case 31:
                case 32:                      
					TestFsm.Upgrade(Event, "123", 3);
				break;              
				default:
					printf("Input error\n");
				break;
			}
            printf("Please Input the New Event:\n");
		}
		

	}
    return 0;    
}

#endif

//XXX 有待验证
CState::CState(unsigned int unIndex, const string &Desc):m_unIndex(unIndex),m_strDesc(Desc)
{
    Dbg("m_unIndex is [%d], m_strDesc is [%s]\n",m_unIndex ,m_strDesc.c_str());
    Dbg("Call CState(unsigned int unIndex, const string &Desc)\n");
}

CState::CState():m_unIndex(0), m_strDesc(" ")
{
//    memset(&m_stEnterCallBack, 0, sizeof(CallBack_t));
//    memset(&m_stLeaveCallBack, 0, sizeof(CallBack_t));
    Dbg("Call the Default CState() constructor\n");
}

CState::CState(CState *pState)
{
    m_stEnterCallBack = pState->m_stEnterCallBack;
    m_stLeaveCallBack = pState->m_stLeaveCallBack;
    m_unIndex = pState->m_unIndex;
    m_strDesc = pState->m_strDesc;
}

CState::CState(const CState &State)
{
    m_stEnterCallBack = State.m_stEnterCallBack;
    m_stLeaveCallBack = State.m_stLeaveCallBack;
    m_unIndex = State.m_unIndex;
    m_strDesc = State.m_strDesc;
    Dbg("m_unIndex is [%d], m_strDesc is [%s]\n",m_unIndex ,m_strDesc.c_str());

}

CState::~CState()
{
    Dbg("Call ~CState()\n");
}

int CState::SetEnterHandle(NormalFuncHandle pHandle)
{
    return (_SetHandle(m_stEnterCallBack, pHandle));
}

int CState::SetEnterHandle(void *pClass, CallBack pHandle)
{
    return (_SetHandle(m_stEnterCallBack, pHandle, pClass));
}

int CState::SetLeaveHandle(NormalFuncHandle pHandle)
{
    return (_SetHandle(m_stLeaveCallBack, pHandle));
}

int CState::SetLeaveHandle(void *pClass, CallBack pHandle)
{
    return (_SetHandle(m_stLeaveCallBack, pHandle, pClass));
}

int CState::_SetHandle(CallBack_t &uCallBack, NormalFuncHandle pHandle)
{
    if(NULL == pHandle)
    {
        uCallBack.m_ClassHandle.pClass = (CNullClass*)NULL;
        uCallBack.m_ClassHandle.pCallBack = (CallBack)NULL;
        uCallBack.m_pNormalHandle = (NormalFuncHandle)NULL;
        Dbg("NULL == pHandle\n");
        return -1;
    }

    uCallBack.m_ClassHandle.pClass = (CNullClass*)NULL;
    uCallBack.m_ClassHandle.pCallBack = NULL;
    uCallBack.m_pNormalHandle = (NormalFuncHandle)pHandle;
    return 0;
}

int CState::_SetHandle(CallBack_t &uCallBack, CallBack pHandle, void *pClass)
{
     if(NULL == pClass || NULL == pHandle)
     {
         uCallBack.m_ClassHandle.pClass = (CNullClass*)NULL;
         uCallBack.m_ClassHandle.pCallBack = (CallBack)NULL;
         uCallBack.m_pNormalHandle = (NormalFuncHandle)NULL;
         Dbg("NULL == pClass || NULL == pHandle\n");
         return -1;

     }
     uCallBack.m_ClassHandle.pClass = (CNullClass*)pClass;
     uCallBack.m_ClassHandle.pCallBack = pHandle;
     uCallBack.m_pNormalHandle = (NormalFuncHandle)NULL;
     return 0;
}

int CState::Enter(const char *pData, unsigned int unDataLen)
{
    Dbg("Enter [%s]\n", m_strDesc.c_str());
    int nRst = 0;
    nRst = Execute(m_stEnterCallBack, pData, unDataLen);
    if (-2 == nRst)
    {
        Dbg("There is no Enter Act!\n");
    }
    return nRst;
}

int CState::Leave(const char *pData, unsigned int unDataLen)
{
    Dbg("Leave [%s]\n", m_strDesc.c_str());
    int nRst = 0;
    nRst = Execute(m_stLeaveCallBack, pData, unDataLen);
    if (-2 == nRst)
    {
        Dbg("There is no Leave Act!\n");
    }
    return nRst;
}
int CState::Execute(CallBack_t &pCallBack, const char *pData, unsigned int unDataLen)
{

    if (NULL == pCallBack.m_ClassHandle.pClass && NULL != pCallBack.m_pNormalHandle)
    {
        //XXX 有待验证
        Dbg("Exec Rst is %d\n", (pCallBack.m_pNormalHandle(pData, unDataLen)));

    }
    else if(NULL != pCallBack.m_ClassHandle.pClass && NULL != pCallBack.m_ClassHandle.pCallBack)
    {
        //XXX 有待验证
        Dbg("Exec Rst is %d\n", ((pCallBack.m_ClassHandle.pClass->*(pCallBack.m_ClassHandle.pCallBack))(pData, unDataLen)));
    }
    else
    {

        return -2;
    }
    return 0;
}




