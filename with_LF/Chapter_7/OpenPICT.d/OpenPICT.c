#include <GestaltEqu.h>
#define kBaseResID 128
#define kMoveToFront (WindowPtr ) -1L
#define kEmptyString "\p"
#define kNilFilterProc nil
#define kErrorAlertID kBaseResID
#define kPICTHeaderSize 512

/***************/ 
/* Functions */ 
/***************/
void ToolBoxInit( void);
void GetFileName( StandardFileReply *replyPtr ); 
PicHandle LoadPICTFile( StandardFileReply *replyPtr ); 
void WindowInit( void);
void DrawMyPicture( PicHandle picture);
void CenterPict( PicHandle picture, Rect *destRectPtr );
void DoError( Str255 errorString, Boolean fatal );

/**************************** main **********************/
void main ( void ){
  PicHandle picture; 
  StandardFileReply reply;
  ToolBoxInit () ; 
  GetFileName( &reply); 
  if ( reply.sfGood ){
    picture= LoadPICTFile( &reply); 
    if ( picture != nil){
      WindowInit (); 
      DrawMyPicture( picture);
      while ( ! Button() ) ;
    }
  }
}


/****************** ToolBoxinit *********************/
void ToolBoxInit( void){
  InitGraf( &thePort ); 
  InitFonts(); 
  InitWindows () ; 
  InitMenus();
  TEInit (); 
  InitDialogs( nil ); 
  InitCursor () ;
}

  
/******************************** GetFileName *******/
void GetFileName ( StandardFileReply *replyPtr){
  SFTypeList typeList; short numTypes; long feature; OSErr err;

  err= Gestalt( gestaltStandardFileAttr, &feature);
  if ( err != noErr )
    DoError( "\pError returned by Gestalt!", true);
  if ( feature & (1 << gestaltStandardFile58)){ 
    typeList[ 0 ] = 'PICT';
    numTypes = 1;
    StandardGetFile( kNilFilterProc, numTypes, typeList, replyPtr );

  } else {
    DoError( "\pThe new Standard File routines \ are not supported by this OS!", true);
  }
}


/******************************** LoadPICTFile *******/
PicHandle LoadPICTFile( StandardFileReply *replyPtr ){
  short srcFile;
  PicHandle picture;
  char pictHeader[ kPICTHeaderSize ]; long pictSize, headerSize;
  long feature;
  OSErr err;

  err= Gestalt( gestaltFSAttr, &feature);
  if ( err != noErr )
    DoError( "\pError returned by Gestalt!", true);
  if ( feature & (1 << gestaltHasFSSpecCalls) ){
    if ( FSpOpenDF( &(replyPtr->sfFile), fsRdPerm, &srcFile ) != noErr){
      DoError( "\pCan't open file ... ", false); 
      return( nil );
    }
    if ( GetEOF( srcFile, &pictSize ) != noErr ){
      DoError ( "\pError returned by GetEOF()...", false );
      return( nil );
    }
    headerSize = kPICTHeaderSize;
    if (FSRead( srcFile, &headerSize, pictHeader ) != noErr){
      DoError( "\pError reading file header...", false);
      return( nil);
    }
    pictSize = kPICTHeaderSize;


    if ( ( picture= (PicHandle)NewHandle( pictSize )) == nil ){
      DoError("\pNot enough memory to read picture ... ", false);
      return( nil);
    }
    HLock( (Handle)picture );

    if ( FSRead ( srcFile, &pictSize, *picture ) != noErr ){
      DoError( "\pError returned by FSRead() ... ", false);
      return ( nil );
    }
    HUnlock( (Handle)picture ); FSClose( srcFile );
    return( picture); 
  }else{
    DoError( "\pThe new FSSpec File Manager routines \ are not supported by this OS!", true);
  }
}


/****************** Window!nit ***********************/
void WindowInit( void){
  WindowPtr window;

  window= GetNewWindow( kBaseResID, nil, kMoveToFront );
  if ( window == nil )
    DoError( "\pCan't load WIND resource...", true);
  ShowWindow( window); 
  SetPort( window);

}


/****************** DrawMyPicture ********************/
void DrawMyPicture( PicHandle picture ){
  Rect pictureRect; 
  WindowPtr window;

  window = FrontWindow(); 
  pictureRect = window->portRect;
  CenterPict( picture, &pictureRect ); 
  DrawPicture( picture, &pictureRect );
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
  ParamText( errorString, kEmptyString, kEmptyString, kEmptyString );
  StopAlert( kErrorAlertID, kNilFilterProc );
  if ( fatal) ExitToShell() ;
}
