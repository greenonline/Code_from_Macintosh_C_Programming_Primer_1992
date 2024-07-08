#include <Values.h>
#define kBaseResID  128
#define kMoveToFront  (WindowPtr)-1L
#define kScrollBarWidth  16
#define kNilActionProc nil
#define kSleep MAXLONG


#define kVisible true
#define kStartValue 1
#define kMinValue 1
#define kNilRefCon 0L
#define kEmptyTitle "\p"
#define kEmptyString "\p"
#define kNilFilterProc nil
#define kErrorAlertID kBaseResID

/**************/ /* Globals */ /**************/
Boolean gDone;

/***************/ /* Functions */ /***************/
void ToolBoxInit( void);
void WindowInit( void);
void SetUpScrollBar( WindowPtr window);
pascal void ScrollProc( ControlHandle theControl, short partCode ); 
void EventLoop( void);
void DoEvent( EventRecord *eventPtr );
void HandleMouseDown( EventRecord *eventPtr );
void UpdateWindow( WindowPtr window);
void CenterPict( PicHandle picture, Rect *destRectPtr ); 
void DoError( Str255 errorString, Boolean fatal);


/**************************** main **********************/ 
void main( void){
  ToolBoxInit(); 
  WindowInit () ;
  EventLoop();
}


/****************** ToolBoxinit *********************/
void ToolBoxInit( void){

  InitGraf( &thePort ); 
  InitFonts(); 
  InitWindows () ; 
  InitMenus();
  TEInit () ; 
  InitDialogs( nil); 
  InitCursor();
}


/******************************** Windowinit ********/
void WindowInit( void){
  WindowPtr window;

  if ( ( window = GetNewWindow( kBaseResID, nil, kMoveToFront ))==nil) 
    DoError( "\pCan't Load WIND resource!", true);
  SetUpScrollBar( window);
  ShowWindow( window); 
  SetPort( window);
}


/********************************** SetUpScrollBar ******/
void SetUpScrollBar( WindowPtr window ){
  Rect vScrollRect; 
  short numPictures; 
  ControlHandle scrollBarH;

  if ( ( numPictures = CountResources( 'PICT' ) ) <= 0) 
    DoError( "\pNo PICT resources were found!", true);


  vScrollRect = window->portRect; 
  vScrollRect.top -= 1; 
  vScrollRect.bottom += 1; 
  vScrollRect.left = vScrollRect.right - kScrollBarWidth + 1; 
  vScrollRect.right += 1;
  scrollBarH = NewControl( window, &vScrollRect, kEmptyTitle, kVisible, kStartValue, kMinValue, numPictures, scrollBarProc, kNilRefCon );
}


/********************************** ScrollProc *******/
pascal void ScrollProc( ControlHandle theControl, short partCode ){

  short curCtlValue, maxCtlValue, minCtlValue; 
  WindowPtr window;

  maxCtlValue = GetCtlMax( theControl );
  curCtlValue = GetCtlValue( theControl );
  minCtlValue = GetCtlMin( theControl );
  
  window = (**theControl).contrlOwner; 
  switch (partCode ){
    case inPageDown: 
    case inDownButton:
      if ( curCtlValue < maxCtlValue){
        curCtlValue += 1;
        SetCtlValue( theControl, curCtlValue ); 
       }
       break; 
    case inPageUp:
    case inUpButton:
      if ( curCtlValue > minCtlValue)
      {
        curCtlValue -= 1;
        SetCtlValue( theControl, curCtlValue );
        UpdateWindow( window);
    }
  }
}


/****************** EventLoop ***********************/
void EventLoop( void ){
  EventRecord event;
  gDone =false;
  while ( gDone == false )

  {
    if ( WaitNextEvent( everyEvent, &event, kSleep, nil))
      DoEvent( &event);
  }
}


/****************** DoEvent ***********************/
void DoEvent(EventRecord *eventPtr){

  WindowPtr window;

  switch ( eventPtr->what){
    case mouseDown:
      HandleMouseDown( eventPtr ); break;
    case updateEvt:
      window = (WindowPtr)eventPtr->message;
      BeginUpdate( window); 
      DrawControls( window); 
      UpdateWindow( window); 
      EndUpdate( window); 
      break;
  }
}


/****************** HandleMouseDown *********************/
void HandleMouseDown( EventRecord *eventPtr ){

  WindowPtr window; short thePart; 
  Point thePoint; 
  ControlHandle theControl;

  thePart = FindWindow( eventPtr->where, &window); 
  switch ( thePart )
  {
    case inSysWindow:
      SystemClick( eventPtr, window); break;
    case inDrag :
      DragWindow( window, eventPtr->where, &screenBits.bounds );
      break;
    case inContent:
      thePoint = eventPtr->where; GlobalToLocal( &thePoint );
      thePart = FindControl( thePoint, window, &theControl );
      if(theControl == ((WindowPeek)window)->controlList){
        if ( thePart == inThumb ){
          thePart = TrackControl( theControl, thePoint, kNilActionProc );
          InvalRect( &(window->portRect) );
        } else
          thePart = TrackControl( theControl, thePoint, &ScrollProc );
        }
      break; 
  case inGoAway:
    gDone = true; break;
  }
}


/********************************** UpdateWindow *******/
void UpdateWindow( WindowPtr window){
  PicHandle currentPicture; Rect windowRect; RgnHandle tempRgn;

  tempRgn = NewRgn(); 
  GetClip( tempRgn );
  windowRect = window->portRect; 
  windowRect.right -= kScrollBarWidth;
  EraseRect( &windowRect );
  ClipRect( &windowRect );
  currentPicture = (PicHandle)GetIndResource( 'PICT', GetCtlValue( ((WindowPeek)window)->controlList ) );
  if ( currentPicture == nil)
    DoError( "\pCan't Load PICT resource!", true);
  CenterPict( currentPicture, &windowRect ); DrawPicture( currentPicture, &windowRect );
  SetClip( tempRgn ); DisposeRgn( tempRgn );
}


/****************** CenterPict ********************/
void CenterPict( PicHandle picture, Rect *destRectPtr){
Rect windRect, pictRect;

  windRect = *destRectPtr;
  pictRect = (**(picture)) .picFrame;
  OffsetRect( &pictRect, windRect.left - pictRect.left, windRect.top - pictRect.top); 
  OffsetRect( &pictRect, (windRect.right - pictRect.right)/2, (windRect.bottom - pictRect.bottom)/2);
  *destRectPtr = pictRect;
}


/***************** DoError ********************/
void DoError( Str255 errorString, Boolean fatal){
  ParamText ( errorString, kEmptyString, kEmptyString, kEmptyString ) ;
  StopAlert( kErrorAlertID , kNilFilterProc ) ;
  if ( fatal )
    ExitToShell ();
}
