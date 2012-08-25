//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormMain.h"
#include <queue>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
TCustomWinSocket* m_Skt = NULL;
static BOOL bConnected = false;
static UINT8 m_RecvBuf[1024];
static UINT8 m_DataBuf[1024];
typedef std::queue<UINT8>  TBufList;
static  TBufList m_BufList;
typedef enum{
	DIR_CONTRL=0,
	DIR_READ,
	DIR_WRITE,
	DIR_ACK,
	MAX_DIR_SIZE
}TCmdDir;
typedef enum{
	CMD_READ_INT1=0,
    CMD_READ_INT2,
    CMD_READ_INT3,
    CMD_READ_INT4,
    CMD_READ_FLOAT1,
    CMD_READ_FLOAT2,
    CMD_READ_FLOAT3,
    CMD_READ_FLOAT4,
    CMD_READ_DATE,
    CMD_READ_TIME
}TCmdType;
static TCommMsg __CommMsg;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnConnClick(TObject *Sender)
{
    skt->Host = lbledt_ip->Text;
    skt->Port = 8000;
    if(bConnected)
    {
        skt->Active = false;

    }
    else
    {
        skt->Active = true;
        //btnConn->Caption = "断开设备";

    }

}
//---------------------------------------------------------------------------

void __fastcall TForm1::sktConnect(TObject *Sender,
      TCustomWinSocket *Socket)
{
    btnConn->Caption = "断开设备";
    bConnected = true;
    m_Skt = Socket;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::sktDisconnect(TObject *Sender,
      TCustomWinSocket *Socket)
{
  btnConn->Caption = "连接设备";
  bConnected = false;
  m_Skt = NULL;
}
//---------------------------------------------------------------------------
UINT8 checkSum(UINT8 *buf,int len)
{
   UINT8 sum = 0;
   for(int i =0 ; i < len; i++)
    sum+= buf[i];
   return sum;
}
void __fastcall TForm1::ReadParam(UINT8 code)
{
      UINT8 cmd[11];
      cmd[0] = 0xFE;
      cmd[1] = 0x7F;
      cmd[2] = 0x0;
      cmd[3] = 0x0;
      cmd[4] = DIR_READ;
      cmd[5] = code;
      cmd[6] = 0;
      cmd[7] = 0;
      cmd[8] = 0;
      cmd[9] = 0;
      cmd[10] = checkSum(cmd,10);
      if(m_Skt)
      {
        m_Skt->SendBuf(cmd,11);
      }
}
void __fastcall TForm1::WriteIntParam(UINT8 code,int value)
{
      UINT8 cmd[11];
      cmd[0] = 0xFE;
      cmd[1] = 0x7F;
      cmd[2] = 0x0;
      cmd[3] = 0x0;
      cmd[4] = DIR_WRITE;
      cmd[5] = code;
      cmd[6] = value>>24;
      cmd[7] = value>>16;;
      cmd[8] = value>>8;
      cmd[9] = value;
      cmd[10] = checkSum(cmd,10);
      if(m_Skt)
      {
        m_Skt->SendBuf(cmd,11);
      }
}

void __fastcall TForm1::lbledt_int1DblClick(TObject *Sender)
{
     ReadParam(CMD_READ_INT1);
}
void __fastcall TForm1::DealData(TCommMsg* msg)
{
    //if(msg->dir == DIR_READ)
    {
        if(msg->cmd == CMD_READ_INT1)
        {
           lbledt_int1->Text = msg->param.u32_val;
        }
        else if(msg->cmd == CMD_READ_INT2)
        {
           lbledt_int2->Text = msg->param.u32_val;
        }
        else if(msg->cmd == CMD_READ_INT3)
        {
           lbledt_int3->Text = msg->param.u32_val;
        }
        else if(msg->cmd == CMD_READ_INT4)
        {
           lbledt_int4->Text = msg->param.u32_val;
        }
        else if(msg->cmd == CMD_READ_FLOAT1)
        {
           lbledt_fl1->Text = msg->param.f32_val;
        }
        else if(msg->cmd == CMD_READ_FLOAT2)
        {
           lbledt_fl2->Text = msg->param.f32_val;
        }
        else if(msg->cmd == CMD_READ_FLOAT3)
        {
           lbledt_fl3->Text = msg->param.f32_val;
        }
        else if(msg->cmd == CMD_READ_FLOAT4)
        {
           lbledt_fl4->Text = msg->param.f32_val;
        }
        else if(msg->cmd == CMD_READ_DATE)
        {
           lbledt_date->Text = msg->param.u32_val;
        }
        else if(msg->cmd == CMD_READ_TIME)
        {
           lbledt_time->Text = msg->param.u32_val;
        }
    }
}
void __fastcall TForm1::ParseData(void)
{
    while( m_BufList.size() >= 11)
    {
        UINT8 ch = m_BufList.front();
        m_BufList.pop();
        if(ch == 0xFE)
        {
             ch = m_BufList.front();
             m_BufList.pop();
             if(ch == 0x7F)
             {
                  m_DataBuf[0] = 0xFE;
                  m_DataBuf[1] = 0x7F;
                  for(int i = 2; i < 11;i++)
                  {
                        m_DataBuf[i] =m_BufList.front();
                        m_BufList.pop();
                  }

                  	__CommMsg.head1  = m_DataBuf[0];
					__CommMsg.head2  = m_DataBuf[1];
					__CommMsg.dev_id = (m_DataBuf[2]<<8) + m_DataBuf[3];
					__CommMsg.dir 	 = m_DataBuf[4];
					__CommMsg.cmd 	 = m_DataBuf[5];
					__CommMsg.param.__pad.pad[0] = m_DataBuf[9];
					__CommMsg.param.__pad.pad[1] = m_DataBuf[8];
					__CommMsg.param.__pad.pad[2] = m_DataBuf[7];
					__CommMsg.param.__pad.pad[3] = m_DataBuf[6];
                    DealData(&__CommMsg);
             }
        }
    }



}
//---------------------------------------------------------------------------

void __fastcall TForm1::sktRead(TObject *Sender, TCustomWinSocket *Socket)
{
      int len = Socket->ReceiveLength();
      Socket->ReceiveBuf(m_RecvBuf,len);

      for(int i =0; i < len;i++)
        m_BufList.push(m_RecvBuf[i]);
      ParseData();
      
}
//---------------------------------------------------------------------------


void __fastcall TForm1::lbledt_int2DblClick(TObject *Sender)
{
    ReadParam(CMD_READ_INT2);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::lbledt_int3DblClick(TObject *Sender)
{
   ReadParam(CMD_READ_INT3);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::lbledt_int4DblClick(TObject *Sender)
{
   ReadParam(CMD_READ_INT4);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::lbledt_fl1DblClick(TObject *Sender)
{
    ReadParam(CMD_READ_FLOAT1);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::lbledt_fl2DblClick(TObject *Sender)
{
    ReadParam(CMD_READ_FLOAT2);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::lbledt_fl3DblClick(TObject *Sender)
{
    ReadParam(CMD_READ_FLOAT3);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::lbledt_fl4DblClick(TObject *Sender)
{
    ReadParam(CMD_READ_FLOAT4);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::lbledt_dateDblClick(TObject *Sender)
{
     ReadParam(CMD_READ_DATE);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::lbledt_timeDblClick(TObject *Sender)
{
     ReadParam(CMD_READ_TIME);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::lbledt_int1KeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
      if(Key == 13)
      {
          WriteIntParam(CMD_READ_INT1, lbledt_int1->Text.ToInt());
          lbledt_int1->Clear();
          ReadParam(CMD_READ_INT1);
      }
}
//---------------------------------------------------------------------------

