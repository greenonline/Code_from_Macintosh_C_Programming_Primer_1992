/* Mondrian.c */

#define kBaseResID 129
#define kMoveToFront (WindowPtr)-1L 
#define kRandomUpperLimit 32768

/*************/ 
/* Globals */ 
/*************/

long gFillColor = blackColor;

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
}

void MainLoop( void ){
  GetDateTime( (unsigned long*) ( &randSeed) );
  while ( ! Button() ) {
  DrawRandomRect();
  if ( gFillColor == blackColor)
    gFillColor = whiteColor;
  else
    gFillColor = blackColor;
  }
}

/****************** DrawRandomRect *****************/ 
void DrawRandomRect( void ){
  Rect randomRect;

  RandomRect( &randomRect ); 
  ForeColor( gFillColor ); 
  PaintOval( &randomRect );
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
