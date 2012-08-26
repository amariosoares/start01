object Form1: TForm1
  Left = 374
  Top = 81
  Width = 423
  Height = 608
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = mm1
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object grp1: TGroupBox
    Left = 16
    Top = 16
    Width = 329
    Height = 105
    Caption = #36890#35759#65306
    TabOrder = 0
    object lbledt_ip: TLabeledEdit
      Left = 24
      Top = 48
      Width = 161
      Height = 21
      EditLabel.Width = 82
      EditLabel.Height = 13
      EditLabel.Caption = #35774#22791'IP'#22320#22336#65306'    '
      LabelPosition = lpAbove
      LabelSpacing = 3
      TabOrder = 0
      Text = '192.168.1.101'
    end
    object btnConn: TBitBtn
      Left = 232
      Top = 40
      Width = 81
      Height = 33
      Caption = #36830#25509#35774#22791
      TabOrder = 1
      OnClick = btnConnClick
    end
  end
  object grp2: TGroupBox
    Left = 24
    Top = 144
    Width = 337
    Height = 377
    Caption = #21442#25968#65306
    TabOrder = 1
    object lbledt_int1: TLabeledEdit
      Left = 32
      Top = 40
      Width = 121
      Height = 21
      EditLabel.Width = 48
      EditLabel.Height = 13
      EditLabel.Caption = #25972#24418'1'#65306'  '
      LabelPosition = lpAbove
      LabelSpacing = 3
      TabOrder = 0
      OnDblClick = lbledt_int1DblClick
      OnKeyUp = lbledt_int1KeyUp
    end
    object lbledt_int2: TLabeledEdit
      Left = 32
      Top = 91
      Width = 121
      Height = 21
      EditLabel.Width = 48
      EditLabel.Height = 13
      EditLabel.Caption = #25972#24418'2'#65306'  '
      LabelPosition = lpAbove
      LabelSpacing = 3
      TabOrder = 1
      OnDblClick = lbledt_int2DblClick
      OnKeyUp = lbledt_int2KeyUp
    end
    object lbledt_int3: TLabeledEdit
      Left = 32
      Top = 141
      Width = 121
      Height = 21
      EditLabel.Width = 48
      EditLabel.Height = 13
      EditLabel.Caption = #25972#24418'3'#65306'  '
      LabelPosition = lpAbove
      LabelSpacing = 3
      TabOrder = 2
      OnDblClick = lbledt_int3DblClick
      OnKeyUp = lbledt_int3KeyUp
    end
    object lbledt_int4: TLabeledEdit
      Left = 32
      Top = 192
      Width = 121
      Height = 21
      EditLabel.Width = 48
      EditLabel.Height = 13
      EditLabel.Caption = #25972#24418'4'#65306'  '
      LabelPosition = lpAbove
      LabelSpacing = 3
      TabOrder = 3
      OnDblClick = lbledt_int4DblClick
      OnKeyUp = lbledt_int4KeyUp
    end
    object lbledt_fl1: TLabeledEdit
      Left = 168
      Top = 40
      Width = 121
      Height = 21
      EditLabel.Width = 48
      EditLabel.Height = 13
      EditLabel.Caption = #28014#28857'1'#65306'  '
      LabelPosition = lpAbove
      LabelSpacing = 3
      TabOrder = 4
      OnDblClick = lbledt_fl1DblClick
      OnKeyUp = lbledt_fl1KeyUp
    end
    object lbledt_fl2: TLabeledEdit
      Left = 168
      Top = 91
      Width = 121
      Height = 21
      EditLabel.Width = 48
      EditLabel.Height = 13
      EditLabel.Caption = #28014#28857'2'#65306'  '
      LabelPosition = lpAbove
      LabelSpacing = 3
      TabOrder = 5
      OnDblClick = lbledt_fl2DblClick
      OnKeyUp = lbledt_fl2KeyUp
    end
    object lbledt_fl3: TLabeledEdit
      Left = 168
      Top = 141
      Width = 121
      Height = 21
      EditLabel.Width = 48
      EditLabel.Height = 13
      EditLabel.Caption = #28014#28857'3'#65306'  '
      LabelPosition = lpAbove
      LabelSpacing = 3
      TabOrder = 6
      OnDblClick = lbledt_fl3DblClick
      OnKeyUp = lbledt_fl3KeyUp
    end
    object lbledt_fl4: TLabeledEdit
      Left = 168
      Top = 192
      Width = 121
      Height = 21
      EditLabel.Width = 48
      EditLabel.Height = 13
      EditLabel.Caption = #28014#28857'4'#65306'  '
      LabelPosition = lpAbove
      LabelSpacing = 3
      TabOrder = 7
      OnDblClick = lbledt_fl4DblClick
      OnKeyUp = lbledt_fl4KeyUp
    end
    object lbledt_date: TLabeledEdit
      Left = 32
      Top = 240
      Width = 121
      Height = 21
      EditLabel.Width = 42
      EditLabel.Height = 13
      EditLabel.Caption = #26085#26399#65306'  '
      LabelPosition = lpAbove
      LabelSpacing = 3
      TabOrder = 8
      OnDblClick = lbledt_dateDblClick
      OnKeyUp = lbledt_dateKeyUp
    end
    object lbledt_time: TLabeledEdit
      Left = 168
      Top = 240
      Width = 121
      Height = 21
      EditLabel.Width = 42
      EditLabel.Height = 13
      EditLabel.Caption = #26102#38388#65306'  '
      LabelPosition = lpAbove
      LabelSpacing = 3
      TabOrder = 9
      OnDblClick = lbledt_timeDblClick
      OnKeyUp = lbledt_timeKeyUp
    end
    object lbledt_localip: TLabeledEdit
      Left = 32
      Top = 288
      Width = 121
      Height = 21
      EditLabel.Width = 49
      EditLabel.Height = 13
      EditLabel.Caption = #35774#22791'IP'#65306' '
      LabelPosition = lpAbove
      LabelSpacing = 3
      TabOrder = 10
      OnDblClick = lbledt_localipDblClick
      OnKeyUp = lbledt_localipKeyUp
    end
    object lbledt_netmask: TLabeledEdit
      Left = 168
      Top = 288
      Width = 121
      Height = 21
      EditLabel.Width = 42
      EditLabel.Height = 13
      EditLabel.Caption = #32593#20851#65306'  '
      LabelPosition = lpAbove
      LabelSpacing = 3
      TabOrder = 11
      OnDblClick = lbledt_netmaskDblClick
      OnKeyUp = lbledt_netmaskKeyUp
    end
    object lbledt_gateway: TLabeledEdit
      Left = 32
      Top = 328
      Width = 121
      Height = 21
      EditLabel.Width = 66
      EditLabel.Height = 13
      EditLabel.Caption = #23376#32593#25513#30721#65306'  '
      LabelPosition = lpAbove
      LabelSpacing = 3
      TabOrder = 12
      OnDblClick = lbledt_gatewayDblClick
      OnKeyUp = lbledt_gatewayKeyUp
    end
  end
  object skt: TClientSocket
    Active = False
    ClientType = ctNonBlocking
    Port = 0
    OnConnect = sktConnect
    OnDisconnect = sktDisconnect
    OnRead = sktRead
    Left = 200
    Top = 24
  end
  object mm1: TMainMenu
    Left = 216
    Top = 136
    object N1: TMenuItem
      Caption = #25805#20316
      object N2: TMenuItem
        Caption = #21516#27493#26102#38388
        OnClick = N2Click
      end
    end
  end
end
