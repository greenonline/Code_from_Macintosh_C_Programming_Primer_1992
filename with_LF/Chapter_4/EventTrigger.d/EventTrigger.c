#include <AppleEvents.h> 
#include <GestaltEqu.h>
#define kGestaltMask 1L
/***************/
/* Functions */ 
/***************/
void ToolBoxinit( void);
void Eventsinit( void);
void SendEvent( AEEventID theAEEventID );
/******************************** main *********/ 
void main( void){
  ToolBoxinit () ; 
  Eventsinit () ;
  SendEvent( kAEOpenApplication ); 
  SendEvent( kAEOpenDocuments );
  SendEvent( kAEPrintDocuments ); 
  SendEvent( kAEQuitApplication );
}
/*********************************** ToolBoxinit */
void ToolBoxinit( void){
  InitGraf( &thePort ); 
  InitFonts () ; 
  InitWindows(); 
  InitMenus ();
  TEInit () ; 
  InitDialogs( nil); 
  InitCursor();
}
/*********************************** Eventsinit */ 
void Eventsinit( void){
  long feature; OSErr err;

  err= Gestalt( gestaltAppleEventsAttr, &feature); 
  if ( err != noErr ){
    SysBeep( 10 ); /* Error calling Gestalt!!! */ 
    ExitToShell () ;
  }
  if ( ! ( feature & ( kGestaltMask << gestaltAppleEventsPresent ) ) )
  {
    SysBeep( 10 ); /* AppleEvents not supported!!! */ 
    ExitToShell() ;
  }
}


/******************************** SendEvent *********/
void SendEvent( AEEventID theAEEventID )
{
  OSErr err;
  AEAddressDesc address;
  OSType appSig;
  AppleEvent appleEvent, reply;

  appSig = 'Prmr';
  err= AECreateDesc( typeApplSignature, (Ptr) (&appSig),(Size)sizeof( appSig ), &address);
  err= AECreateAppleEvent( kCoreEventClass, theAEEventID, &address,
kAutoGenerateReturnID, 1L, &appleEvent );
err =AESend( &appleEvent, &reply, kAENoReply + kAECanInteract, kAENormalPriority, kAEDefaultTimeout, nil, nil);

}
