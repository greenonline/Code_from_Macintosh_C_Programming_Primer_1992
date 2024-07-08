#include <PrintTraps.h>
#define kBaseResID 128
#define kDontScaleOutput nil
#define kEmptyString "\p"
#define kNilFilterProc nil
#define kErrorAlertID kBaseResID


/***************/ /* Functions */ /***************/
void ToolBoxInit( void);
PicHandle LoadPICT( void);
THPrint PrintInit( void);
Boolean DoDialogs( THPrint printRecordH );
void PrintPicture( PicHandle picture, THPrint printRecordH );
void CenterPict( PicHandle picture, Rect *destRectPtr );
void DoError( Str255 errorString, Boolean fatal);

/**************************** main **********************/
void main( void ){
  PicHandle picture; THPrint printRecordH;
  ToolBoxInit(); 
  picture= LoadPICT();
  printRecordH = PrintInit();
  if ( DoDialogs( printRecordH ) )
    PrintPicture( picture, printRecordH );
}

/****************** ToolBox!nit *********************/
void ToolBoxInit( void){
  InitGraf( &thePort ); 
  InitFonts();
  InitWindows(); 
  InitMenus(); 
  TEInit(); 
  InitDialogs( nil); 
  InitCursor ();

}

/******************************** LoadPICT *********/ PicHandle LoadPICT( void){
  PicHandle picture;

  picture= GetPicture( kBaseResID );
  if ( picture == nil )
    DoError ( "\pCan 't load PICT resource...", true ) ;
  return( picture);
}

/******************************** P r i n t i n i t *********/ 
THPrint PrintInit( void){
  THPrint printRecordH;

  printRecordH = (THPrint)NewHandle( sizeof( TPrint ) );
  if ( printRecordH == nil ) 
    DoError("\pNot enough memory to allocate print record" , true);
  PrOpen ();
  PrintDefault( printRecordH );
  return( printRecordH );
}

/******************************** DoDialogs *******/
Boolean DoDialogs( THPrint printRecordH ){
  Boolean confirmed = true;

  confirmed= PrStlDialog( printRecordH );
  if ( confirmed )
    confirmed= PrJobDialog( printRecordH );

  return( confirmed);
}

/******************************** PrintPicture *******/
void PrintPicture( PicHandle picture, THPrint printRecordH){
  TPPrPort printPort; Rect pictureRect; TPrStatus printStatus;
  printPort = PrOpenDoc( printRecordH, nil, nil);
  PrOpenPage( printPort, kDontScaleOutput );
  if ( PrError() != noErr )
    DoError( "\pError returned by PrOpenPage() ... ", true);
  pictureRect = (**printRecordH) .prInfo.rPage;
  CenterPict( picture, &pictureRect ); 
  DrawPicture( picture, &pictureRect );
  PrClosePage( printPort ); 
  PrCloseDoc( printPort );
  if ( (**printRecordH) .prJob.bJDocLoop == bSpoolLoop )
    PrPicFile( printRecordH, nil, nil, nil, &printStatus );
  PrClose();
  DisposHandle( (Handle)printRecordH );
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
  ParamText( errorString, kEmptyString, kEmptyString, kEmptyString ) ;
  StopAlert( kErrorAlertID, kNilFilterProc );
  if ( fatal)
    ExitToShell();
}
