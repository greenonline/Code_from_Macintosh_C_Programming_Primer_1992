#include <Sound.h> 
#include <Soundinput.h> 
#include <GestaltEqu.h>
#define kBaseResID 128
#define kNilSoundChannel nil 
#define kSynchronous false
#define kEmptyString "\p" 
#define kNilFilterProc nil
#define kErrorAlertID kBaseResID

/***************/ /* Functions */ /***************/
void ToolBoxInit( void);
Handle RecordSound( void);
void PlaySound( Handle soundHandle );
void DoError( Str255 errorString, Boolean fatal);

/**************************** main **********************/
void main( void ){
  Handle soundHandle; long feature; OSErr err;

  MaxApplZone (); 
  ToolBoxInit();

  err= Gestalt( gestaltSoundAttr, &feature);
  if ( err != noErr )
    DoError( "\pError returned by Gestalt!", true);
  if ( feature & (1 << gestaltHasSoundInputDevice)){
    soundHandle = RecordSound(); 
    PlaySound( soundHandle ); 
    DisposHandle( soundHandle );
  }
  else
    DoError( "\pSound input device not available!!!", true);
}

/****************** ToolBoxinit *********************/
void ToolBoxInit( void ){
  InitGraf( &thePort ); 
  InitFonts () ; 
  InitWindows (); 
  InitMenus () ;
  TEInit () ; 
//  InitDialogs( nil );  // For Think v6
  InitDialogs(0L);       // For Think v7
  InitCursor();
}

/****************** RecordSound ***********************/ 
Handle RecordSound( void ){
  OSErr err;
  Point upperLeft; Handle soundHandle;
  SetPt( &upperLeft, 50, 50 );
  soundHandle = nil;
  err = SndRecord( nil, upperLeft, siBestQuality, &soundHandle ); /* Causes unexpected error */
                                                                  /* in BasiliskII           */
  if ( err == userCanceledErr )
    DoError( "\pRecording canceled... ", true);
  if ( err != 0 )
    DoError( "\pError returned by SndRecord() ... ", true); 
  return( soundHandle );
}

/****************** PlaySound ***********************/
void PlaySound( Handle soundHandle ) {
  OSErr err;

  err = SndPlay( kNilSoundChannel, soundHandle, kSynchronous );
  if ( err != noErr )
    DoError( "\pError returned by SndPlay() ... ", true);
}

/***************** DoError ********************/
void DoError( Str255 errorString, Boolean fatal){
  ParamText( errorString, kEmptyString, kEmptyString, kEmptyString );
  StopAlert( kErrorAlertID, kNilFilterProc );
  if ( fatal)
    ExitToShell () ;
}
