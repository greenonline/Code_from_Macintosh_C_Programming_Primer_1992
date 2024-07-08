#define kBaseResID 128 #define kMoveToFront (WindowPtr) - 1L 
#define kEmptyString "\p"
#define kNilFilterProc nil
#define kErrorAlertID kBaseResID

/***************/ 
/* Functions */ 
/***************/
 
void ToolBoxInit( void);
void WindowInit( void);
void MainLoop( void);
void CenterPict( PicHandle picture, Rect *destRectPtr ); 
void DoError( Str255 errorString, Boolean fatal);

/****************** main ***************************/ 
void main( void){
  ToolBoxInit(); 
  WindowInit(); 
  MainLoop();

}

/****************** ToolBoxinit *********************/
void ToolBoxInit( void){
  InitGraf( &thePort ); InitFonts();
  InitWindows(); 
  InitMenus () ;
  TEInit (); 
  InitDialogs( nil); 
  InitCursor();
}

/****************** Windowlnit ***********************/
void WindowInit( void){
  WindowPtr window;
  window= GetNewWindow( kBaseResID, nil, kMoveToFront );
  if ( window == nil ) 
    DoError( "\pCan't load the WIND resource!", true );
  ShowWindow( window); SetPort( window);
}

/******************************** MainLoop *********/
void MainLoop( void ){
  Rect pictureRect; Handle clipHandle; 
  long length, offset; 
  WindowPtr window;

  clipHandle= NewHandle( 0 ); 
  window= FrontWindow();
  if ( ( length= GetScrap( clipHandle, 'TEXT', &offs~t ))<0){
    if ( GetScrap( clipHandle, 'PICT', &offset ) < 0 ) 
      DoError("\pThere's no PICT and no text in the scrap..." , true};
    else{
      pictureRect = window->portRect; CenterPict( (PicHandle}clipHandle,
&pictureRect ); 
      DrawPicture( (PicHandle)clipHandle, &pictureRect );
    }
  } else{
    HLock( clipHandle };
    TextBox( *clipHandle, length, &(window->portRect), teJustLeft);
    HUnlock( clipHandle };
  }
  while ( !Button(} )
}

/****************** CenterPict ********************/
void CenterPict( PicHandle picture, Rect *destRectPtr){
  Rect windRect, pictRect;

  windRect = *destRectPtr;
  pictRect = (**(picture }) .picFrame;
  OffsetRect( &pictRect, windRect.left - pictRect.left,
  windRect.top - pictRect.top); OffsetRect( &pictRect, (windRect.right - pictRect.right)/2, (windRect.bottom - pictRect.bottom)/2); 
  *destRectPtr =pictRect;
}

/***************** DoError ********************/
void DoError( Str255 errorString, Boolean fatal){
  ParamText( errorString, kEmptyString, kEmptyString, kEmptyString );
  StopAlert( kErrorAlertID, kNilFilterProc );
  if ( fatal)
     ExitToShell ();
}
