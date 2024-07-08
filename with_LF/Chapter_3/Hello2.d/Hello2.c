#define kBaseResID 128
#define kMoveToFront (WindowPtr)-1L
#define kHorizontalPixel 30 
#define kVerticalPixel 50

/***************/ 
/* Functions */ 
/***************/

void ToolBoxInit( void); 
void WindowInit( void);

/****************** main ***************************/ 

void main( void){
  ToolBoxInit(); 
  WindowInit ();
  while ( !Button() );
}

/****************** ToolBoxinit *********************/

void ToolBoxInit( void){
  InitGraf( &thePort ); 
  InitFonts();
  InitWindows(); 
  InitMenus () ;
  TEInit () ; 
  InitDialogs( nil ); 
  InitCursor();
}

/****************** WindowInit ***********************/ 

void WindowInit( void){
  WindowPtr window;
  
  window= GetNewWindow( kBaseResID , nil , kMoveToFront );
  if ( window == nil){
    SysBeep( 10 ); /* Couldn't load the WIND resource ! ! ! */
    ExitToShell () ;
  }
  
  ShowWindow( window);
  SetPort( window);
  
  MoveTo( kHorizontalPixel , kVerticalPixel ); 
  DrawString("\pHello, world!");

}
