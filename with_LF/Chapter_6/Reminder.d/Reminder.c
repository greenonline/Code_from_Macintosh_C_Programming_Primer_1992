/* Reminder.c */

#include <Notification.h> 
#include <Processes.h> 
#include <Aliases.h>
#define kBaseResID 128
#define kMoveToFront (WindowPtr)-1L
#define kSleep 3600L
#define kLeaveWhereitis false
#define kUseDefaultProc (void *) -lL
#define kNotANormalMenu -1
#define mApple kBaseResID
#define iAbout 1
#define mFile kBaseResID+1
#define iSetReminder 1
#define iCancelReminder 2 
#define iQuit 4
#define mHours 100
#define mMinutes 101
#define mAMorPM 102
#define mReminders 103
#define kDialogResID kBaseResID+1
#define iHoursPopup 4
#define iMinutesPopup 5
#define iAMorPMPopup 6
#define iMessageText 8
#define iSoundCheckBox 9
#define iRotateCheckBox 10
#define iLaunchCheckBox 11
#define iAppNameText 12
#define kOn 1
#define kOff 0
#define kMarkApp 1

#define kAM 1
#define kPM 2
#define kMinTextPosition 0
#define kMaxTextPosition 32767
#define kDisableButton 255
#define kEnableButton 0

typedef struct {
  QElem queue;
  NMRec notify; FSSpec file;
  short hour;
  short minute; Boolean launch; Str255 alert;
  Str255 menuString; short menuItem; Boolean dispose; Boolean wasPosted; 
} ReminderRec, *ReminderPtr;
/***************/ 
/* Functions */ 
/***************/

void ToolBoxInit( void);
void MenuBarInit( void);
void EventLoop( void);
void DoEvent( EventRecord *eventPtr ); 
void HandleNull( void);
void HandleMouseDown( EventRecord *eventPtr ); 
void HandleMenuChoice( long menuChoice );
void HandleAppleChoice( short item );
void HandleFileChoice( short item );
ReminderPtr HandleDialog( void);
void GetFileName( StandardFileReply *replyPtr );

pascal void LaunchResponse( NMRecPtr notifyPtr );
pascal void NormalResponse( NMRecPtr notifyPtr );
void CopyDialogToReminder( DialogPtr dialog, ReminderPtr reminder);
ReminderPtr GetFirstReminder( void);
ReminderPtr  GetNextReminder( ReminderPtr reminder);
ReminderPtr GetReminderFromNotification( NMRecPtr notifyPtr );
ReminderPtr FindReminderOnMenu( short menuItem );
ReminderPtr FindReminderToPost( short hour, short minute); 
ReminderPtr FindReminderToDispose( void);

void SetupReminderMenu( void);
short CountRemindersOnMenu( void);
void RenumberTrailingReminders( ReminderPtr reminder); 
void InsertReminderintoMenu( ReminderPtr reminder);
void ScheduleReminder( ReminderPtr reminder);
void PostReminder( ReminderPtr reminder);
void  DeleteReminderFromMenu( ReminderPtr reminder ) ;
void DeleteReminder( ReminderPtr reminder);
ReminderPtr DisposeReminder( ReminderPtr reminder);

void ConcatString( Str255 strl, Str255 str2);

/* see tech note 304 */
pascal OSErr SetDialogDefaultItem(DialogPtr theDialog, short newItem) = { 0x303C, 0x0304, 0xAA68 };
pascal OSErr SetDialogCancelItem(DialogPtr theDialog, short newItem) = { 0x303C, 0x0305, 0xAA68 };
pascal OSErr SetDialogTracksCursor(DialogPtr theDialog, Boolean tracks) = { 0x303C, 0x0306, 0xAA68 };

/*************/ /* Globals */ /*************/
Boolean gDone;
QHdr gReminderQueue;
/******************************** main *********/ 
void main( void){
  ToolBoxInit(); 
  MenuBarInit();
  EventLoop();
}
/*********************************** ToolBoxinit */
void ToolBoxInit( void){
InitGraf( &qd.thePort ); 
InitFonts () ;
InitWindows ();
InitMenus () ;
TEInit (); 
InitDialogs( nil); 
InitCursor ();
}

/*********************************** MenuBarinit */ 
void MenuBarInit( void){
  Handle menuBar; MenuHandle menu;

  menuBar = GetNewMBar( kBaseResID ); 
  if ( menuBar == nil ) {
    SysBeep( 20 ); 
    ExitToShell ();
  }
  SetMenuBar( menuBar );
  menu= GetMenu( mReminders ); 
  InsertMenu( menu, kNotANormalMenu );
  menu= GetMHandle( mApple ); 
  AddResMenu( menu, 'DRVR' );
  DrawMenuBar();
}

/*********************************** EventLoop */
void EventLoop( void ){
  EventRecord event;
  gDone = false;

  while ( gDone == false ) {
    if ( WaitNextEvent( everyEvent, &event, GetCaretTime(), nil))
      DoEvent( &event ); 
    else
      HandleNull();
  }
}

/*********************************** DoEvent */ 
void DoEvent( EventRecord *eventPtr){
  char theChar;

  switch ( eventPtr->what){

    case mouseDown:
      HandleMouseDown( eventPtr ); break;
    case keyDown: case autoKey:
      theChar = eventPtr->message & charCodeMask; 
      if ( (eventPtr->modifiers & cmdKey) != 0 )
        HandleMenuChoice( MenuKey( theChar) ); 
      break;
  }
}

/****************** HandleNull **********************/
void HandleNull( void ){
  unsigned long time; 
  DateTimeRec dateTime; 
  ReminderPtr theReminder;

  GetDateTime( & time); 
  Secs2Date( time, &dateTime );
  theReminder = FindReminderToPost( dateTime.hour, dateTime.minute );
  while ( theReminder ){
    PostReminder( theReminder ); 
    DeleteReminderFromMenu( theReminder ); 
    theReminder = FindReminderToPost ( dateTime.hour, dateTime.minute );
  }
  theReminder = FindReminderToDispose(); 
  while ( theReminder )
  {
    DisposeReminder( theReminder ); 
    theReminder = FindReminderToDispose ();
  }
}

/****************** HandleMouseDown *******************/
void HandleMouseDown( EventRecord *eventPtr ){
  WindowPtr window; short thePart; long menuChoice;

  thePart = FindWindow( eventPtr->where, &window ) ; 
  switch ( thePart )
  {
    case inMenuBar:
      SetupReminderMenu();
      menuChoice = MenuSelect( eventPtr->where ); 
      HandleMenuChoice( menuChoice );
      break;
    case inSysWindow:
      SystemClick( eventPtr, window); 
      break;
  }
}
/****************** SetupReminderMenu *******************/
void SetupReminderMenu( void ){
  MenuHandle fileMenu; short items;

  fileMenu = GetMenu( mFile );
  items= CountRemindersOnMenu();
  if ( items ) 
    EnableItem( fileMenu, iCancelReminder); 
  else 
    DisableItem( fileMenu, iCancelReminder);
}
/****************** HandleMenuChoice *******************/ 
void HandleMenuChoice( long menuChoice ){

  short menu; short item; ReminderPtr reminder;
  if ( menuChoice != 0 ){
    menu = HiWord( menuChoice );
    item = LoWord( menuChoice );
    switch (menu){
      case mApple: 
        HandleAppleChoice(item);
        break; 
      case mFile:
        HandleFileChoice(item);
        break;
      case mReminders:

        reminder= FindReminderOnMenu( item );
        if ( reminder )
          DeleteReminder( reminder); 
        break;
    }
    HiliteMenu ( 0 ) ;
  }
}

/****************** HandleAppleChoice *******************/
void HandleAppleChoice( short item ){
  MenuHandle appleMenu; 
  Str255 accName; short accNumber;

  switch (item ){
    case iAbout:
      NoteAlert( kBaseResID, nil); 
      break;
    default:
      appleMenu = GetMHandle( mApple ); 
      GetItem( appleMenu, item, accName );

      accNumber = OpenDeskAcc( accName );
      break;
  }
}



/****************** HandleFileChoice *******************/
void HandleFileChoice( short item ){
  ReminderPtr reminder;

  switch ( item )
  {
    case iSetReminder:
      reminder= HandleDialog(); if ( reminder )
      ScheduleReminder( reminder);
      break;
    case iQuit : 
      gDone = true;
      break;
  }
}

/****************** GetFirstReminder ********************/ 
ReminderPtr GetFirstReminder( void ){
  return( (ReminderPtr)gReminderQueue.qHead );
}
/******************* GetNextReminder ********************/ 
ReminderPtr GetNextReminder( ReminderPtr reminder){
  return( (ReminderPtr)reminder->queue.qLink );
}



/****************** FindReminderOnMenu ******************/ 
ReminderPtr FindReminderOnMenu( short menuItem ){
  ReminderPtr theReminder;

  theReminder = GetFirstReminder(); while ( theReminder )
  {
    if ( theReminder->menuItem == menuItem ) 
      break;
  theReminder = GetNextReminder( theReminder);
  } 
  return( theReminder );
}

/***************** FindReminderToPost *******************/ 
ReminderPtr FindReminderToPost( short hour, short minute ){
  ReminderPtr theReminder;

  theReminder = GetFirstReminder(); 
  while ( theReminder ){
    if ( (!theReminder->wasPosted)
       && (theReminder->hour <= hour)
       && (theReminder->minute <= minute))
      break;
    theReminder = GetNextReminder (theReminder);
  }
  return( theReminder );
}
/***************** FindReminderToDispose ****************/ 
ReminderPtr FindReminderToDispose( void){
  ReminderPtr theReminder;

  theReminder = GetFirstReminder (); 
  while ( theReminder ) {
    if ( theReminder->dispose)
      break;
    theReminder = GetNextReminder (theReminder); 
  }
  return( theReminder );
}

/***************** InsertReminderIntoMenu ***************/
void InsertReminderIntoMenu( ReminderPtr reminder ){
  short itemBefore; MenuHandle reminderMenu;

  reminderMenu = GetMenu( mReminders );
  itemBefore = CountRemindersOnMenu();
  InsMenuItem( reminderMenu, reminder->menuString, itemBefore );
  reminder->menuItem = itemBefore + 1;
}

/******************* CountRemindersOnMenu *************~*/ 
short CountRemindersOnMenu( void ){
  MenuHandle reminderMenu;

  reminderMenu = GetMenu( mReminders ); 
  return( CountMItems( reminderMenu) );
}


/******************* DeleteReminderFromMenu *************/ 
void DeleteReminderFromMenu( ReminderPtr reminder ){
  MenuHandle reminderMenu;

  reminderMenu = GetMenu( mReminders ); 
  RenumberTrailingReminders( reminder); 
  DelMenuItem( reminderMenu, reminder->menuItem ); 
  reminder->menuItem = 0;
}
/***************** RenumberTrailingReminders ************/
void RenumberTrailingReminders( ReminderPtr reminder ){ 
  short count;

  count = reminder->menuItem;

  reminder= GetNextReminder( reminder); 
  while ( reminder )
  {
    if (reminder->menuItem != 0) reminder->menuItem = count++;
    reminder= GetNextReminder( reminder);
  }
}

/****************** ScheduleReminder ********************/ 
void ScheduleReminder( ReminderPtr reminder){
  Enqueue( &reminder->queue, &gReminderQueue ); 
  InsertReminderIntoMenu( reminder);
}

/****************** PostReminder ************************/
void PostReminder( ReminderPtr reminder ){
  reminder->notify.nmRefCon = (long)reminder; 
  reminder->wasPosted = true;
  NMInstall( &reminder->notify );
}

/****************** DeleteReminder **********************/
void DeleteReminder( ReminderPtr reminder ){
  if ( reminder->menuItem ) DeleteReminderFromMenu( reminder);
  reminder->dispose = true;
}

/*************** DisposeReminder ************************/ 
ReminderPtr DisposeReminder ( ReminderPtr reminder ){
  ReminderPtr next;
  if (reminder->menuItem) 
    DeleteReminderFromMenu( reminder);
  next= (ReminderPtr)reminder->queue.qLink; Dequeue( &reminder->queue, &gReminderQueue ); 
  DisposePtr( (Ptr)reminder );
  return( next);
}

/******************************** GetFileName *******/ 
void GetFileName( StandardFileReply *replyPtr){
  SFTypeList typeList; 
  short numTypes;



  typeList[ 0 ] = 'APPL';
  numTypes = 1;
  StandardGetFile( nil, numTypes, typeList, replyPtr );
}

/******************************** HandleDialog *********/
ReminderPtr HandleDialog( void ) {
  DialogPtr dialog;
  Boolean dialogDone = false; short itemHit, itemType; Handle textItemHandle; 
  Handle itemHandle;
  Handle okItemHandle; Handle launchItemHandle; Rect itemRect;
  Str255 itemText; StandardFileReply reply;
  ReminderPtr reminder;

  dialog = GetNewDialog( kDialogResID, nil, kMoveToFront );
  ShowWindow( dialog); SetPort( dialog);
  reminder= (ReminderPtr)NewPtr( sizeof( ReminderRec) );
  reminder->menuItem = 0; reminder->dispose = false; 
  reminder->wasPosted = false;
  SetDialogDefaultItem( dialog, ok ); SetDialogCancelItem( dialog, cancel); 
  SetDialogTracksCursor( dialog, true);
  GetDItem( dialog, iMessageText, &itemType, &textItemHandle, &itemRect );
  GetDItem( dialog, ok, &itemType, &okItemHandle, &itemRect );
  GetDItem( dialog, iLaunchCheckBox, &itemType, &launchItemHandle, &itemRect );



  SelIText( dialog, iMessageText, kMinTextPosition, kMaxTextPosition );
  while ( ! dialogDone ){
    GetIText( textItemHandle, itemText );
    if ( itemText[ 0 ] == 0 && !GetCtlValue( (ControlHandle) launchItemHandle ) ) 
      HiliteControl( (ControlHandle)okItemHandle,kDisableButton );
    else
      HiliteControl( (ControlHandle)okItemHandle,kEnableButton );

  ModalDialog( nil, &itemHit );
  switch ( itemHit){
    case ok: case cancel:
      dialogDone = true;
      break;
    case iSoundCheckBox: case iRotateCheckBox:
      GetDItem( dialog, itemHit, &itemType, &itemHandle, &itemRect );
      SetCtlValue( (ControlHandle)itemHandle, ! GetCtlValue((ControlHandle) itemHandle) );
      break;
    case iLaunchCheckBox: case iAppNameText:
      if (! GetCtlValue((ControlHandle) launchItemHandle ) ){
        GetFileName( & reply); 
        if ( reply.sfGood ){
          SetCtlValue( (ControlHandle) launchItemHandle, kOn );  
          reminder->file = reply.sfFile;



          GetDItem( dialog, iAppNameText, &itemType,&itemHandle, &itemRect ); 
          SetIText( itemHandle, reminder->file.name );

        }
      } else {

        SetCtlValue( (ControlHandle) launchItemHandle, kOff );
        GetDItem( dialog, iAppNameText, &itemType,&itemHandle, &itemRect ); 
        SetIText( itemHandle, "\p<Not Selected>" );

      }
      break;
  }
}


  if ( itemHit == cancel ){
    DisposePtr( (Ptr)reminder );
    reminder = nil;
  } 
  else
    CopyDialogToReminder( dialog, reminder);
    DisposDialog( dialog);
  return( reminder);
}

/********************************* CopyDialogToReminder */
void CopyDialogToReminder( DialogPtr dialog, ReminderPtr reminder){
  short itemType; Rect itemRect; Handle itemHandle; Str255 string; 
  MenuHandle menu;
  short val; long tmp;



  GetDItem( dialog, iMessageText, &itemType, &itemHandle, &itemRect );
  GetIText( itemHandle, reminder->alert ); reminder->notify.nmStr = (StringPtr)&reminder->alert;
  GetDItem( dialog, iSoundCheckBox, &itemType, &itemHandle, &itemRect );
  if ( GetCtlValue( (ControlHandle)itemHandle ) )
    reminder->notify.nmSound = (Handle) -1L;
  else
    reminder->notify.nmSound = nil;
  GetDItem( dialog, iRotateCheckBox, &itemType, &itemHandle, &itemRect );
  if (GetCtlValue( (ControlHandle)itemHandle ) )
    reminder->notify.nmIcon = GetResource( 'SICN', kBaseResID);
  else
    reminder->notify.nmIcon = nil;
  GetDItem( dialog, iLaunchCheckBox, &itemType, &itemHandle, &itemRect );
  if ( reminder->launch = GetCtlValue( (ControlHandle)itemHandle ) )
    reminder->notify.nmResp = &LaunchResponse; 
  else
    reminder->notify.nmResp = &NormalResponse;
  GetDItem( dialog, iHoursPopup, &itemType, &itemHandle, &itemRect );
  val= GetCtlValue( (ControlHandle)itemHandle ); NumToString( (long) val, string);
  StringToNum (string, &tmp );
  reminder->hour = tmp;
  reminder->menuString[0] = 0;
  ConcatString( reminder->menuString, string); 
  ConcatString( reminder->menuString, "\p:" );
  GetDItem( dialog, iMinutesPopup, &itemType, &itemHandle, &itemRect );
  val= GetCtlValue( (ControlHandle)itemHandle ); 
  menu= GetMHandle( mMinutes );
  GetItem( menu, val, string);


  StringToNum (string, &tmp ); reminder->minute = tmp;
  ConcatString( reminder->menuString, string); ConcatString( reminder->menuString, "\p ");
  GetDItem( dialog, iAMorPMPopup, &itemType, &itemHandle, &itemRect );
  val= GetCtlValue( (ControlHandle)itemHandle );
  if( val == kPM ) 
    reminder->hour += 12;
  menu= GetMenu ( mAMorPM );
  GetItem( menu, val, string);
  ConcatString( reminder->menuString, string);
  reminder->notify.qType = nmType; 
  reminder->notify.nmMark = kMarkApp;

}

/************************** ConcatString ************/ 
void ConcatString( Str255 str1, Str255 str2){
  short i;

  for (i=str1[0];i<str2[0]+str1[0];i++)
    str1[i+1]=str2[i-str1[0]+1]; 
   str1[0]=i;
}

/************************** NormalResponse ************/
pascal void NormalResponse( NMRecPtr notifyPtr ) {
  ReminderPtr reminder; 
  OSErr err;

  reminder = GetReminderFromNotification( notifyPtr ); 
  err= NMRemove( notifyPtr );

  reminder->dispose = true;
}

/************************** LaunchResponse *******/
pascal void LaunchResponse( NMRecPtr notifyPtr ){
  LaunchParamBlockRec launchParams; 
  OSErr err;
  FSSpec fileSpec; 
  ReminderPtr reminder; 
  Boolean isFolder; 
  Boolean wasAlias;

  reminder = GetReminderFromNotification( notifyPtr ); 
  fileSpec = reminder->file;

  err = ResolveAliasFile( &fileSpec, true, &isFolder, &wasAlias );
  launchParams.launchBlockID = extendedBlock; 
  launchParams.launchEPBLength = extendedBlockLen; 
  launchParams.launchFileFlags = 0; 
  launchParams.launchControlFlags = launchContinue + launchNoFileFlags; 
  launchParams.launchAppSpec = &fileSpec;
  launchParams.launchAppParameters = nil;
  if ( LaunchApplication( &launchParams ) ) 
    SysBeep( 20 ); 
  err= NMRemove( notifyPtr );
  reminder->dispose = true;
}
/********************** GetReminderFromNotification *****/
ReminderPtr GetReminderFromNotification( NMRecPtr notifyPtr){
  return (ReminderPtr) notifyPtr->nmRefCon;
}
