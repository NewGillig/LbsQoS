/***********************************************************
 * Author: Wen Li
 * Date  : 11/10/2020
 * Describe: traffic flow split
 * History:
   <1> 11/10/2020 , create
************************************************************/
#ifndef _FLOWCSV_H_
#define _FLOWCSV_H_
#include <BasicType.h>
#include <Context.h>
#include <iostream> 
#include <fstream>


using namespace std;

#define BUF_LEN (4096)


class FlowCsv
{
protected:
    FlowManage *m_Flm;


public:

    FlowCsv (FlowManage *Flm)
    {
        m_Flm = Flm;
    }

    VOID Dump ()
    {
        DWORD DumpNum = 0;
        BYTE* Buffer = (BYTE* )malloc (16 * 1024);
        assert (Buffer != NULL);

        ofstream CsvFile;
        string FileName = "CSVs/" + to_string (time(NULL)) + ".csv";
	    CsvFile.open(FileName, ios::app);

        for (auto It = m_Flm->begin (); It != m_Flm->end (); It++)
        {
            Flow *F = *It;
            if (F->m_Fin != 0)
            {
                continue;
            }
       
            if (F->m_SduNum < CAPTURE_NUM)
            {
                continue;
            }
            //printf ("F->m_SduNum = %u, Age = %u, Packet Num = %u \r\n", F->m_SduNum, Age, F->m_PakcetData.size());

            /* flow name: src_srcport_dst_dst_port_protocol */
            snprintf ((char *)Buffer, BUF_LEN, "%.2X_%.2X_%.2X_%.2X_%.2X", 
                      F->m_SrcIp, F->m_SrcPort, F->m_DstIp, F->m_DstPort, F->m_ProtoType);
            CsvFile<< Buffer << ',';

            /* write packet data */
            for (auto pIt = F->m_PakcetData.begin (); pIt != F->m_PakcetData.end (); pIt++)
            {
                IpPacket *Packet = *pIt;

                memset (Buffer, 0, BUF_LEN);
                DWORD offset = 0;
                while (offset < Packet->m_PayloadLen)
                {
                    CHAR Value[8];
                    snprintf (Value, sizeof (Value), "%.2X ", Packet->m_Payload[offset]);
                    strcat ((char*)Buffer, Value);
                    offset++;
                    if (offset >= FORMAT_LEN)
                    {
                        break;
                    }
                }

                while (offset < FORMAT_LEN)
                {
                    CHAR Value[8];
                    snprintf (Value, sizeof (Value), "%.2X ", 0);
                    strcat ((char*)Buffer, Value);
                    offset++;
                }

                CsvFile<< Buffer<<",";
            }

            CsvFile<<"\r\n";

            F->m_Fin = 1;
            F->DelPackets ();
            DumpNum++;
        }

        delete Buffer;
        if (DumpNum == 0)
        {
            remove (FileName.c_str());
        }
        else
        {
            cout<<"Dump "<<DumpNum <<" flows into "<<FileName<<"\r\n";
        }

        CsvFile.close ();    
        return;
    }
};



#endif 
