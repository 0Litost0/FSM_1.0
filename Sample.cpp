#include <iostream>
#include "CFSM.h"
using namespace std;

unsigned int Act_1(const char * pData, unsigned int unDataLen)
{
    printf("***I am Act_1, Data is [%s]***\n", pData);
    return 0;
}
unsigned int Act_2(const char * pData, unsigned int unDataLen)
{
    printf("---I am Act_2, Data is [%s]---\n", pData);
    return 0;
}
unsigned int Act_3(const char * pData, unsigned int unDataLen)
{
    printf("---I am Act_3, Data is [%s]---\n", pData);
    return 0;
}

unsigned int Enter_1(const char * pData, unsigned int unDataLen)
{
    printf("---I am Enter_1, Data is [%s]---\n", pData);
    return 0;
}
unsigned int Leave_1(const char * pData, unsigned int unDataLen)
{
    printf("---I am Leave_1, Data is [%s]---\n", pData);
    return 0;
}
unsigned int Enter_2(const char * pData, unsigned int unDataLen)
{
    printf("---I am Enter_2, Data is [%s]---\n", pData);
    return 0;
}
unsigned int Leave_2(const char * pData, unsigned int unDataLen)
{
    printf("---I am Leave_2, Data is [%s]---\n", pData);
    return 0;
}
unsigned int Enter_3(const char * pData, unsigned int unDataLen)
{
    printf("---I am Enter_3, Data is [%s]---\n", pData);
    return 0;
}
unsigned int Leave_3(const char * pData, unsigned int unDataLen)
{
    printf("---I am Leave_3, Data is [%s]---\n", pData);
    return 0;
}
int main(int argc, char **argv)
{
    CFSM TestFsm;

    //添加状态：方式1
    CState Stat_1(0x01, "Stat_1");
    TestFsm.AddState(Stat_1);

    CallBack_t State2Enter;
    CallBack_t State2Leave;
    State2Enter.m_pNormalHandle = Enter_2;
    State2Leave.m_pNormalHandle = Leave_2;

    //添加状态：方式2
    TestFsm.AddState(0x02, "Stat_2", State2Enter, State2Leave);

    //添加状态：方式3
    TestFsm.AddState(0X03, "Stat_3");
    CState Stat_3 = TestFsm.QueryState(0X03);
    Stat_3.m_strDesc = "Stat_3 modify";
    Stat_3.SetEnterHandle(Enter_3);
    Stat_3.SetLeaveHandle(Leave_3);
    TestFsm.ModifyState(Stat_3);



    TestFsm.AddEvent(12, "1->2");
    TestFsm.AddEvent(13, "1->3");
    TestFsm.AddEvent(21, "2->1");
    TestFsm.AddEvent(23, "2->3");
    TestFsm.AddEvent(31, "3->1");
    TestFsm.AddEvent(32, "3->2");

    TestFsm.Banding(0x01, 12, 0x02, Act_1);
    TestFsm.Banding(0x01, 13, 0x03, Act_1);
    TestFsm.Banding(0x02, 21, 0x01, Act_2);
    TestFsm.Banding(0x02, 23, 0x03, Act_2);
    TestFsm.Banding(0x03, 31, 0x01, Act_3);
    TestFsm.Banding(0x03, 32, 0x02, Act_3);


    //设置初始状态
    TestFsm.SetState(0x01);
    TestFsm.CurState();

    int Event = 0;
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
