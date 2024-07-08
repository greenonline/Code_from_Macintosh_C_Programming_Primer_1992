/* Mondrian.c */

#define kBaseResID 128
#define kMoveToFront (WindowPtr)-1L 
#define kRandomUpperLimit 32768

/* Variants */
#define kOvalWidth 20 
#define kOvalHeight 20 
/* For FrameRoundRect, PaintRoundRect and InvertRoundRect */
#define kPenWidth 10 
#define kPenHeight 2 
/* For FrameArc, PaintArc and InvertArc */
#define kStartDegrees 0
#define kArcDegrees 270


/*************/ 
/* Globals */ 
/*************/

//long gFillColor = blackColor;
long gFillColor = redColor;

/* blackColor, whiteColor, redColor, yellowColor, 
greenColor, blueColor, cyanColor, and magentaColor */


/***************/ 
/* Functions */ 
/***************/
void ToolBoxInit( void);
void WindowInit( void);
void MainLoop( void);
void DrawRandomRect( void ) ;
void RandomRect( Rect *rectPtr ); 
short Randomize( short range);

/****************** main ***************************/ 
void main( void){
  ToolBoxInit(); 
  WindowInit(); 
  MainLoop();
}

/****************** ToolBoxinit *********************/
void ToolBoxInit( void){
  InitGraf( &thePort ); 
  InitFonts ();
  InitWindows(); 
  InitMenus();
  TEInit (); 
  InitDialogs( nil ); 
  InitCursor () ;
}

/****************** Window!nit ***********************/ 
void WindowInit( void){
  WindowPtr window;

  window = GetNewWindow( kBaseResID , nil, kMoveToFront);
  if ( window == nil)
  {
    SysBeep( 10 ); /* Couldn't load the WIND resource!!! */
    ExitToShell ();
  }
  ShowWindow( window); 
  SetPort( window);
  
  /* Variants */
  /* For Paint, Frame and InvertRoundRect */
  PenSize( kPenWidth, kPenHeight ); 
  PenPat( gray); 

}

void MainLoop( void ){
  GetDateTime( (unsigned long*) ( &randSeed) );
  while ( ! Button() ) {
    DrawRandomRect();
    if ( gFillColor == redColor ) 
      gFillColor =yellowColor; 
    else
      gFillColor =redColor; 

//    if ( gFillColor == blackColor)
//      gFillColor = whiteColor;
//    else
//      gFillColor = blackColor;
  }
}

/****************** DrawRandomRect *****************/ 
void DrawRandomRect( void ){
  Rect randomRect;

  RandomRect( &randomRect ); 
  ForeColor( gFillColor ); 
  /* Variants */
//  PaintOval( &randomRect );
//  PaintRoundRect( &randomRect , kOvalWidth, kOvalHeight ); 
//  FrameRoundRect( &randomRect, kOvalWidth, kOvalHeight ); 
//  InvertRoundRect ( &randomRect, kOvalWidth, kOvalHeight ); 

//  FrameArc( &randomRect, kStartDegrees, kArcDegrees ); 
  PaintArc( &randomRect, kStartDegrees, kArcDegrees ); 
//  InvertArc( &randomRect, kStartDegrees, kArcDegrees ); 


}

/****************** RandomRect *********************/
void RandomRect( Rect *rectPtr ) {
  WindowPtr window;

  window= FrontWindow();
  rectPtr->left = Randomize( window->portRect.right - window->portRect.left );
  rectPtr->right =Randomize( window->portRect.right - window->portRect.left );
  rectPtr->top =Randomize( window->portRect.bottom - window->portRect.top );
  rectPtr->bottom = Randomize( window->portRect.bottom - window->portRect.top );
}

/****************** Randomize **********************/ 
short Randomize( short range ) {
  long randomNumber;

  randomNumber =Random();
  if ( randomNumber < 0 ) randomNumber *= -1;
  return( (randomNumber *range) / kRandomUpperLimit );
}
