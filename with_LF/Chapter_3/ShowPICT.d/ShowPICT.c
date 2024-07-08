#define kBaseResID 128 
#define kMoveToFront (WindowPtr) -1L
/***************/ /* Functions */ /***************/


void ToolBoxInit( void);
void WindowInit( void);
void DrawMyPicture( void);
void CenterPict( PicHandle picture, Rect *destRectPtr );
/****************** main ***************************/ 
void main( void){
  ToolBoxInit(); 
  WindowInit();
  DrawMyPicture(); 
  while ( !Button ());
}
/****************** ToolBoxinit *********************/
void ToolBoxInit( void){
  InitGraf( &thePort ); 
  InitFonts () ;
  InitWindows(); 
  InitMenus();
  TEInit (); 
  InitDialogs( nil); 
  InitCursor();
}

/****************** Windowinit ***********************/
void WindowInit( void){
  WindowPtr window;
  window= GetNewWindow( kBaseResID, nil, kMoveToFront ); 
  if ( window == nil){
    SysBeep ( 10 ) ; /* Couldn't load the WIND resource! ! ! */
    ExitToShell ();
  }
  ShowWindow( window); SetPort( window);

}
/****************** DrawMyPicture ********************/
void DrawMyPicture( void ){
  Rect pictureRect; 
  WindowPtr window; 
  PicHandle picture;
  window= FrontWindow();
  pictureRect = window->portRect; 
  picture= GetPicture( kBaseResID ); 
  if ( picture == nil){
    SysBeep( 10 ); /* Couldn't load the PICT resource! ! ! */
    ExitToShell();
  }
  CenterPict( picture, &pictureRect ); 
  DrawPicture( picture, &pictureRect );
}

/****************** CenterPict ********************/
void CenterPict( PicHandle picture , Rect *destRectPtr){
  Rect windRect , pictRect;
  windRect = *destRectPtr;
  pictRect = (**(picture)) .picFrame;
  OffsetRect( &pictRect, windRect.left - pictRect.left,
  windRect.top - pictRect.top); OffsetRect ( &pictRect, (windRect.right -
  pictRect.right) /2 , (windRect.bottom - pictRect.bottom)/2) ; 
  *destRectPtr = pictRect;
}
