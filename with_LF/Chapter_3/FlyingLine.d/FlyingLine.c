/* FLyingLine.c */
#define kNumLines 50 /* Try100or150 */
#define kMoveToFront (WindowPtr)-1L
#define kRandomUpperLimit 32768
#define kEmptyString "\p"
#define kEmptyTitle  kEmptyString
#define kVisible true
#define kNoGoAway false
#define kNilRefCon (long) nil

/*************/ /* Globals */ /*************/
 
    
Rect gLines[ kNumLines ];

short gDeltaTop=3, gDeltaBottom=3; /* These four
                                      are the key 
                                      to FlyingLine!*/ 

short gDeltaLeft=2, gDeltaRight=6;
short gOldMBarHeight;

/***************/ /* Functions */ /***************/
void ToolBoxInit( void);
void WindowInit( void);
void LinesInit( void);
void MainLoop( void);
void RandomRect( Rect *rectPtr ); 
short Randomize( short range); 
void RecalcLine( short i );
void DrawLine( short i );
/****************** main ***************************/ 
void main( void){
  ToolBoxInit () ; 
  WindowInit(); 
  LinesInit(); 
  MainLoop();
}
/****************** ToolBoxinit *********************/ 
void ToolBoxInit( void){
  InitGraf( &thePort ); 
  InitFonts () ; 
  InitWindows () ; 
  InitMenus () ;
  TEInit (); 
  InitDialogs( nil); 
  InitCursor();
}
/****************** W indowinit ***********************/
void WindowInit( void){
  Rect totalRect, mBarRect; 
  RgnHandle mBarRgn;
  WindowPtr window;
  gOldMBarHeight = MBarHeight; 
  MBarHeight = 0;

  window= NewWindow( nil, &(screenBits.bounds), kEmptyTitle, kVisible, plainDBox, kMoveToFront, kNoGoAway, kNilRefCon );
  SetRect( &mBarRect, screenBits.bounds.left, screenBits.bounds.top,
                      screenBits.bounds.right,
                      screenBits.bounds.top+gOldMBarHeight) ;
  mBarRgn = NewRgn();
  RectRgn( mBarRgn, &mBarRect );
  UnionRgn( window->visRgn, mBarRgn, window->visRgn );
   DisposeRgn( mBarRgn );
  SetPort( window);
  FillRect( &(window->portRect), black);
  /* Change black to ltGray, */
  PenMode( patXor ); /*<-- and comment out this line*/
}


/****************** Linesinit **********************/
void LinesInit( void){ 
  short i;

  HideCursor();
  GetDateTime( (unsigned long*) ( &randSeed) ); 
  RandomRect( &(gLines[ 0 ]) );
  DrawLine( 0 );
  for ( i=1; i<kNumLines; i++ ){
    gLines[ i ] = gLines[ i-1 ]; RecalcLine( i );
    DrawLine( i );
  }
}
/****************** MainLoop ***********************/
void MainLoop( void ){
  short i;
  while ( ! Button() ){
    DrawLine( kNumLines - 1 ); 
    for ( i=kNumLines-1; i>0; i--)
      gLines [ i ] = gLines[ i-1 ]; RecalcLine( 0 );
    DrawLine( 0 );
  }
  MBarHeight = gOldMBarHeight;
}
/****************** RandomRect *********************/

void RandomRect( Rect *rectPtr ){
  WindowPtr window;
  window= FrontWindow();

  rectPtr->left =Randomize( window->portRect.right - window->portRect.left );
  rectPtr->right = Randomize( window->portRect.right - window->portRect.left );
  rectPtr->top =Randomize( window->portRect.bottom - window->portRect.top );
  rectPtr->bottom = Randomize( window->portRect.bottom - window->portRect.top);
}
/****************** Randomize **********************/ 
short Randomize( short range){
  long randomNumber;
  randomNumber =Random();
  if ( randomNumber < 0 ) randomNumber *= -1;
  return( (randomNumber *range) / kRandomUpperLimit );
}
/****************** RecalcLine *********************/
void RecalcLine( short i ) {
  WindowPtr window; window= FrontWindow();
  gLines[ i ].top += gDeltaTop;
  if ( ( gLines[ i ].top < window->portRect.top ) ||
       ( gLines[ i ].top> window->portRect.bottom) ){
    gDeltaTop *= -1;
    gLines[ i ] .top += 2*gDeltaTop;

  }
  gLines[ i ].bottom+= gDeltaBottom;
  if ( ( gLines[ i ].bottom< window->portRect.top ) ||
       ( gLines[ i ].bottom> window->portRect.bottom )){

    gDeltaBottom *= -1;
    gLines[ i ].bottom+= 2*gDeltaBottom;
  }
  gLines[ i ].left += gDeltaLeft;
  if ( ( gLines[ i ].left < window->portRect.left ) ||
       ( gLines[ i ].left > window->portRect.right ) ){
    gDeltaLeft *= -1;
    gLines[ i ].left += 2*gDeltaLeft;
  }
  gLines[ i ].right += gDeltaRight;
  if ( ( gLines[ i ].right < window->portRect.left ) ||
       ( gLines[ i ].right > window->portRect.right ) ) {
    gDeltaRight *= -1;
    gLines[ i ].right += 2*gDeltaRight;
  }
}

/****************** DrawLine ***********************/ 
void DrawLine( short i ){

  MoveTo( gLines[ i ] .left, gLines[ i] .top); LineTo( gLines[ i ].right, gLines[ i ].bottom);
}
