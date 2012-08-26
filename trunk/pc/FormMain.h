//---------------------------------------------------------------------------

#ifndef FormMainH
#define FormMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdTCPServer.hpp>
#include <IdTelnetServer.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ScktComp.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------

typedef struct __packed{
	UINT8  head1;
	UINT8  head2;
	UINT8  cmd;
	UINT8  dir;
	UINT16 dev_id;
	union{	
		UINT8	  u8_val;
		UINT16       u16_val;
		UINT32	  u32_val;
		float f32_val;
		struct{
			UINT8 pad[4];
		}__pad;
	}param;
	UINT8 check;
}TCommMsg;


class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TClientSocket *skt;
    TGroupBox *grp1;
    TLabeledEdit *lbledt_ip;
    TBitBtn *btnConn;
    TGroupBox *grp2;
    TLabeledEdit *lbledt_int1;
    TLabeledEdit *lbledt_int2;
    TLabeledEdit *lbledt_int3;
    TLabeledEdit *lbledt_int4;
    TLabeledEdit *lbledt_fl1;
    TLabeledEdit *lbledt_fl2;
    TLabeledEdit *lbledt_fl3;
    TLabeledEdit *lbledt_fl4;
    TLabeledEdit *lbledt_date;
    TLabeledEdit *lbledt_time;
    TMainMenu *mm1;
    TMenuItem *N1;
    TMenuItem *N2;
    TLabeledEdit *lbledt_localip;
    TLabeledEdit *lbledt_netmask;
    TLabeledEdit *lbledt_gateway;
    void __fastcall btnConnClick(TObject *Sender);
    void __fastcall sktConnect(TObject *Sender, TCustomWinSocket *Socket);
    void __fastcall sktDisconnect(TObject *Sender,
          TCustomWinSocket *Socket);
    void __fastcall lbledt_int1DblClick(TObject *Sender);
    void __fastcall sktRead(TObject *Sender, TCustomWinSocket *Socket);
    void __fastcall lbledt_int2DblClick(TObject *Sender);
    void __fastcall lbledt_int3DblClick(TObject *Sender);
    void __fastcall lbledt_int4DblClick(TObject *Sender);
    void __fastcall lbledt_fl1DblClick(TObject *Sender);
    void __fastcall lbledt_fl2DblClick(TObject *Sender);
    void __fastcall lbledt_fl3DblClick(TObject *Sender);
    void __fastcall lbledt_fl4DblClick(TObject *Sender);
    void __fastcall lbledt_dateDblClick(TObject *Sender);
    void __fastcall lbledt_timeDblClick(TObject *Sender);
    void __fastcall lbledt_int1KeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall lbledt_int2KeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall lbledt_int3KeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall lbledt_int4KeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall lbledt_fl1KeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall lbledt_fl2KeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall lbledt_fl3KeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall lbledt_fl4KeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall N2Click(TObject *Sender);
    void __fastcall lbledt_dateKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall lbledt_timeKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall lbledt_localipDblClick(TObject *Sender);
    void __fastcall lbledt_netmaskDblClick(TObject *Sender);
    void __fastcall lbledt_gatewayDblClick(TObject *Sender);
    void __fastcall lbledt_localipKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall lbledt_netmaskKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall lbledt_gatewayKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);

private:	// User declarations
void __fastcall ReadParam(UINT8 cmd);
void __fastcall ParseData(void);
void __fastcall WriteIntParam(UINT8 code,int value);
void __fastcall DealData(TCommMsg* msg);
void __fastcall WriteFloatParam(UINT8 code,float value);
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
