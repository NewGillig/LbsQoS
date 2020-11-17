/***********************************************************
 * Author: Wen Li
 * Date  : 4/14/2020
 * Describe: traffic capture
 * History:
   <1> 11/10/2020 , create
************************************************************/
#include "Capture.h"
#include "FlowCsv.h"
#include "Split.h"

FlowManage m_Flmange;


VOID Help ()
{
    printf("************************************************\r\n");
    printf("*                help information              *\r\n");
    printf("************************************************\r\n");
    printf("-d <device name> \r\n");
    printf("************************************************\r\n\r\n");

    return;
}

void *F2CsvThread (void* Arg)
{
    FlowCsv FC (&m_Flmange);
    while (1)
    {
        sleep (60);
        
        FC.Dump ();
    }
    
    return NULL;
}


int main(int argc, char *argv[])
{
    char ch;
    pthread_t Tid;
    string Device = "";
    string File = "";
    DWORD IsCsv = 0;
    
    while((ch = getopt(argc, argv, "d:s:ch")) != -1)
    {
        switch(ch)
        {
            case 'd':
            {
                Device = optarg;
                break;
            }
            case 's':
            {
                File = optarg;
                break;
            }
            case 'c':
            {
                IsCsv = 1;
                break;
            } 
            default:
            {
                Help ();
                return 0;
            }
        }
    }

    if (File != "")
    {
        Split Sp (File, IsCsv);
        Sp.SplitFlows ();
        return 0;
    }

    if (Device == "")
    {
        Help ();
        return 0;
    }

    int Ret = pthread_create(&Tid, NULL, F2CsvThread, NULL);
    assert (Ret == 0);
           
    Capture Cap(Device);
    Cap.CapturePacket ();
	
	return 0;
}

