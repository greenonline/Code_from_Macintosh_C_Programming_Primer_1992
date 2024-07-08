/* WorldClock.c */
#include <Packages.h> 
#include <GestaltEqu.h>
#define kBaseResID 128
#define kMoveToFront (WindowPtr)-1L
#define kUseDefaultProc (void *)-1L
#define kSleep 20L
#define kLeaveWhereitis false
#define kincludeSeconds true
#define kTicksPerSecond 60
#define kSecondsPerHour 3600L
#define kAddCheckMark true
#define kRemoveCheckMark false
#define kPopupControlID kBaseResID 
#define kNotANormalMenu -1
#define mApple kBaseResID
#define iAbout 1
#define mFile kBaseResID+1
#define iQuit 1
#define mFont 100
#define mStyle 101
#define iPlain 1
#define iBold 2
#define iItalic 3
#define iUnderline 4
#define iOutline 5
#define iShadow 6

#define kPlainStyle 0 

#define kExtraPopupPixels 25
#define kClockLeft 12
#define kClockTop 25
#define kClockSize 24
#define kCurrentTimeZone 1
#define kNewYorkTimeZone 2

#define kMoscowTimeZone 3
#define kUlanBatorTimeZone 4
#define TopLeft( r) (*(Point*) &(r).top)
#define BottomRight( r ) (*(Point *) &(r).bottom)

 
#define IsHighBitSet(longNum) ((longNum >> 23) & 1)
#define SetHighByte(longNum) ( longNum |= 0xFF000000 ) 
#define ClearHighByte(longNum) ( longNum &= 0x00FFFFFF)
/*************/ /* Globals */ /*************/
Boolean gDone, gHasPopupControl;
short gLastFont = 1, gCurrentZoneID =kCurrentTimeZone;
Style gCurrentStyle = kPlainStyle; 
Rect gClockRect;
/***************/ /* Functions */ /***************/

void ToolBoxInit( void);
void WindowInit( void);
void MenuBarInit( void);
void EventLoop( void);
void DoEvent( EventRecord *eventPtr );
void HandleNull( EventRecord *eventPtr );
void HandleMouseDown( EventRecord *eventPtr ); 
void SetUpZoomPosition( WindowPtr window, short zoominOrOut ); 
void HandleMenuChoice( long menuChoice );

void HandleAppleChoice(short item);
void HandleFileChoice(short item);
void HandleFontChoice(short item);
void HandleStyleChoice(short item);
void DoUpdate( EventRecord *eventPtr ); 
long GetZoneOffset( void);

/**************************** main *****************t*1**/ 
void main( void){
  ToolBoxInit () ; 
  WindowInit(); 
  MenuBarInit();
  EventLoop();
}
/****************** ToolBoxinit *********************/ 
void ToolBoxInit( void){
  InitGraf( &thePort ); 
  InitFonts(); 
  InitWindows (); 
  InitMenus () ;
  TEInit () ; 
  InitDialogs( nil); 
  InitCursor();
}
/****************** W indowinit *********************/
void WindowInit ( void){
  WindowPtr window;

  window = GetNewWindow( kBaseResID, nil, kMoveToFront ) ;
  if ( window == nil){
    SysBeep( 10 ); /* Couldn't load the WIND resource ! ! ! */
    ExitToShell ();
  }
  SetPort( window); TextSize( kClockSize );

  gClockRect = window->portRect; ShowWindow( window);
}

/****************** MenuBarlnit ***********************/
void MenuBarInit( void ){

  Handle menuBar; MenuHandle menu; 
  ControlHandle control; 
  OSErr myErr; 
  long feature;

  menuBar =GetNewMBar( kBaseResID ); 
  SetMenuBar( menuBar );
  menu= GetMHandle( mApple ); AddResMenu( menu, 'DRVR' ) ;
  menu= GetMenu( mFont );
  InsertMenu( menu, kNotANormalMenu );
  AddResMenu( menu, 'FONT' ) ;
  menu= GetMenu( mStyle ); InsertMenu( menu, kNotANormalMenu ); 
  CheckItem( menu, iPlain, true);
  DrawMenuBar();
  HandleFontChoice( gLastFont );
  myErr =Gestalt( gestaltPopupAttr, &feature ); 
  gHasPopupControl =((myErr == noErr) && (feature & (1 << gestaltPopupPresent)));
  if (gHasPopupControl)
    control= GetNewControl( kPopupControlID,
  FrontWindow() );
}

/****************** EventLoop ***********************/
void EventLoop( void){
  EventRecord event;
  gDone= false;

  while ( gDone == false ) {
    if ( WaitNextEvent( everyEvent, &event, kSleep, nil) ) DoEvent( &event);
    else
      HandleNull( &event);
  }
}

/****************** DoEvent ***********************/
void DoEvent( EventRecord *eventPtr ) {
  char theChar;

  switch ( eventPtr->what){
    case mouseDown:
      HandleMouseDown( eventPtr ); break;
    case keyDown:
    case autoKey:
      theChar = eventPtr->message & charCodeMask; 
      if ( (eventPtr->modifiers & cmdKey) != 0 )
        HandleMenuChoice( MenuKey( theChar) ); break;
    case updateEvt:
      DoUpdate( eventPtr ); 
      break;
  }
}



/****************** HandleNull ***********************/ 
void HandleNull( EventRecord *eventPtr){
  static long lastTime = 0;

  if ( (eventPtr->when / kTicksPerSecond) > lastTime ){
    InvalRect( &gClockRect );
    lastTime = eventPtr->when / kTicksPerSecond;
  }
}
/***************** HandleMouseDown **********************/
void HandleMouseDown( EventRecord *eventPtr ){
  WindowPtr whichWindow; GrafPtr oldPort; short thePart; long menuChoice; 
  ControlHandle control; short ignored;

  thePart= FindWindow( eventPtr->where, &whichWindow ); 
  switch ( thePart )
  {
    case inMenuBar:
      menuChoice = MenuSelect( eventPtr->where ); 
      HandleMenuChoice( menuChoice );
      break;
    case inSysWindow:
      SystemClick( eventPtr, whichWindow ); break;
    case inContent:
      SetPort( whichWindow ); GlobalToLocal( &eventPtr->where );
      if (FindControl( eventPtr->where, whichWindow, &control ) ){
        ignored= TrackControl( control, eventPtr->where, kUseDefaultProc );
        gCurrentZoneID =GetCtlValue( control);
      }
      break; 
    case inDrag:
      DragWindow( whichWindow, eventPtr->where, &screenBits.bounds );
      break; 
    case inZoomIn:
    case inZoomOut:
      if ( TrackBox( whichWindow, eventPtr->where, thePart ) ){
        SetUpZoomPosition( whichWindow, thePart ); 
        ZoomWindow( whichWindow, thePart, kLeaveWhereitis );
    }
    break;
  }
}


/****************** SetUpZoomPosition ****************/
void SetUpZoomPosition( WindowPtr window, short zoomInOrOut ){
  WindowPeek wPeek; WStateData *wStatePtr; Rect windowRect; Boolean isBig;
  short deltaPixels;

  wPeek =(WindowPeek) window;
  wStatePtr = (WStateData *) *(wPeek->dataHandle);
  windowRect = window->portRect; LocalToGlobal( &TopLeft( windowRect) );   
  LocalToGlobal( &BottomRight( windowRect ));
  wStatePtr->stdState = windowRect; wStatePtr->userState = wStatePtr->stdState;
  if ( gHasPopupControl )
  {

    isBig = (windowRect.bottom - windowRect.top) > (gClockRect.bottom - gClockRect.top);
    if ( isBig )
      deltaPixels -kExtraPopupPixels;
    else
      deltaPixels = kExtraPopupPixels;
    if ( zoomInOrOut == inZoomIn ) 
      wStatePtr->userState.bottom += deltaPixels;
    else
      wStatePtr->stdState.bottom += deltaPixels;
  } else
    SysBeep( 20 );
}


/***************** HandleMenuChoice *********************/
void HandleMenuChoice( long menuChoice ){
  short menu; short item;

  if ( menuChoice != 0 ){
    menu = HiWord( menuChoice ); 
    item = LoWord( menuChoice );
    switch (menu ){
      case mApple: HandleAppleChoice(item); break;
      case mFile: HandleFileChoice(item); break;
      case mFont: HandleFontChoice(item); break;
      case mStyle: HandleStyleChoice(item); break;
    }

    HiliteMenu( 0 ); 
  }
}

/**************** HandleAppleChoice *******************~*/ 
void HandleAppleChoice( short item ){
  MenuHandle appleMenu; 
  Str255 accName; 
  short accNumber;

  switch (item ){
    case iAbout: /* We'll put up an about box next chapter.*/ 
      SysBeep( 20 );
      break; 
    default:
      appleMenu = GetMHandle( mApple ); 
      GetItem( appleMenu, item, accName );  
      accNumber = OpenDeskAcc( accName ); 
      break;
  }
}
/***************** HandleFileCho1ce ****************** **/
void HandleFileChoice( short item ){
  switch ( item ) {
    case iQuit:
      gDone = true;
      break;
  }
}
/***************** HandleFontChoice ***************** ***/ 
void HandleFontChoice( short item ){
  short fontNumber; 
  Str255 fontName; 
  MenuHandle menuHandle;

  menuHandle = GetMHandle( mFont );
  CheckItem( menuHandle, gLastFont, kRemoveCheckMark ); 
  CheckItem( menuHandle, item, kAddCheckMark );
  gLastFont = item;
  GetItem( menuHandle, item, fontName );
  GetFNum( fontName, &fontNumber ); 
  TextFont( fontNumber );
}
/***************** HandleStyleChoice ********************/
void HandleStyleChoice( short item ) {
  MenuHandle menuHandle;

  switch( item ) {
    case iPlain:
      gCurrentStyle =kPlainStyle; break;
    case iBold:
      if ( gCurrentStyle & bold )
        gCurrentStyle -= bold; 
      else
        gCurrentStyle |= bold; 
      break;
    case iItalic:
      if ( gCurrentStyle & italic )
        gCurrentStyle -=italic; 
      else
        gCurrentStyle |= italic; 
      break;
    case iUnderline:
      if ( gCurrentStyle & underline )
        gCurrentStyle -= underline;
      else



        gCurrentStyle |= underline; 
      break;
    case iOutline:
      if ( gCurrentStyle & outline )
        gCurrentStyle -=outline; 
      else
        gCurrentStyle |= outline; break;
    case iShadow:
      if ( gCurrentStyle & shadow )
        gCurrentStyle -=shadow; 
      else
        gCurrentStyle |= shadow; 
      break;
  }
  menuHandle = GetMHandle( mStyle );
  CheckItem( menuHandle, iPlain, gCurrentStyle ==kPlainStyle ) ;
  CheckItem( menuHandle, iBold, gCurrentStyle & bold ) ; 
  CheckItem( menuHandle, iItalic, gCurrentStyle & italic ) ;
  CheckItem( menuHandle, iUnderline, gCurrentStyle & underline ) ;
  CheckItem( menuHandle, iOutline, gCurrentStyle & outline ) ;
  CheckItem( menuHandle, iShadow, gCurrentStyle & shadow ) ;
  TextFace( gCurrentStyle );
}

/****************** DoUpdate ***********************/ 
void DoUpdate( EventRecord *eventPtr ){
  WindowPtr window;
  Str255 timeString; unsigned long curTimeinSecs;

  window= (WindowPtr)eventPtr->message;

  BeginUpdate( window);
  GetDateTime ( &curTimeinSecs ); 
  curTimeinSecs += GetZoneOffset();
  IUTimeString( (long)curTimeinSecs, kincludeSeconds, timeString );
  EraseRect( &gClockRect ); MoveTo( kClockLeft, kClockTop );
  DrawString( timeString );
  DrawControls( window); EndUpdate( window);
}


/****************** GetZoneOffset ***********************/ 
long GetZoneOffset( void)
{
  MachineLocation loc;
  long delta, defaultZoneOffset;

  ReadLocation( &loc ); 
  defaultZoneOffset = ClearHighByte( loc.gmtFlags.gmtDelta );
  if ( IsHighBitSet( defaultZoneOffset ))
    SetHighByte( defaultZoneOffset );
  switch ( gCurrentZoneID )
  {
    case kCurrentTimeZone:
      delta = defaultZoneOffset; 
      break;
    case kNewYorkTimeZone :
      delta = -5L * kSecondsPerHour; 
      break;
    case kMoscowTimeZone :
      delta = 3L * kSecondsPerHour; 
      break;
    case kUlanBatorTimeZone :
      delta = 8L * kSecondsPerHour;
      break;




  }
  delta -= defaultZoneOffset;
  return delta;
}
