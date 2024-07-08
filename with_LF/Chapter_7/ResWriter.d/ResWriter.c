#define kBaseResID 128
#define kMoveToFront (WindowPtr)-1L
#define iText 4
#define kDisableButton  255
#define kEnableButton 0
#define kWriteTextOut true 
#define kDontWriteTextOut false
#define kMinTextPosition 0 
#define kMaxTextPosition 32767

/***************/ /* Functions */ /***************/
void ToolBoxInit( void);
Boolean DoTextDialog( StringHandle oldTextHandle );
pascal OSErr SetDialogDefaultitem( DialogPtr theDialog, short newitem) = { 0x303C, 0x0304, 0xAA68 }; 
pascal OSErr SetDialogCancelitem( DialogPtr theDialog, short newitem ) = { 0x303C, 0x0305, 0xAA68 };
pascal OSErr SetDialogTracksCursor( DialogPtr theDialog, Boolean tracks ) = { 0x303C, 0x0306, 0xAA68 } ;

/**************************** main **********************/
void main( void ){
  StringHandle textHandle;
  ToolBoxInit();
  textHandle = GetString( kBaseResID );
  if ( textHandle == nil ) {
    SysBeep( 20 ); 
    ExitToShell ();
  }
  if ( DoTextDialog( textHandle ) == kWriteTextOut  ){
    ChangedResource( (Handle)textHandle );
    WriteResource( (Handle)textHandle );
  }
}

/****************** ToolBoxinit *********************/ 
void ToolBoxInit( void){
  InitGraf( &thePort ); 
  InitFonts();
  InitWindows();
  InitMenus();
  TEInit () ; 
  InitDialogs( nil); 
  InitCursor();
}

/****************** DoTextDialog *********************/ 
Boolean DoTextDialog( StringHandle textHandle ){
  DialogPtr dialog;
  Boolean done;
  short itemHit, itemType;
  Handle  OKitemHandle, textitemHandle;
  Rect itemRect; 
  Str255 itemText;

  dialog = GetNewDialog( kBaseResID, nil, kMoveToFront );
  GetDItem( dialog, ok, &itemType, &OKitemHandle, &itemRect );
  GetDItem( dialog, iText, &itemType, &textitemHandle, &itemRect );
  HLock( (Handle)textHandle );
  SetIText( textitemHandle, *textHandle ); 
  HUnlock( (Handle)textHandle );
  SelIText( dialog, iText, kMinTextPosition, kMaxTextPosition );
  ShowWindow( dialog); 
  SetPort( dialog);
  SetDialogDefaultitem( dialog, ok ); 
  SetDialogCancelitem( dialog, cancel); 
  SetDialogTracksCursor( dialog, true);
  done = false; 
  while ( ! done){
    GetIText( textitemHandle, itemText );

  if ( itemText[ 0 ] == 0 )
    HiliteControl( (ControlHandle)OKitemHandle, kDisableButton );
  else
    HiliteControl( (ControlHandle)OKitemHandle, kEnableButton ); 
  ModalDialog( nil, &itemHit );
  done = ( (itemHit == ok) || (itemHit == cancel));
  }if ( itemHit == ok ){
    GetIText( textitemHandle, itemText ); 
    SetHandleSize( (Handle)textHandle, (Size) (itemText[ 0 ] + 1) );
    HLock( (Handle)textHandle );
    GetIText( textitemHandle, *textHandle ); HUnlock( (Handle)textHandle );
    DisposDialog( dialog);
    return( kWriteTextOut ); 
    }else{
      DisposDialog( dialog); return( kDontWriteTextOut );
    }
}
