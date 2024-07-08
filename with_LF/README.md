### Readable in OSX/macOS

The text files with line feed (LF), which are readable in TextEdit on OS X.

The line feeds will need converting to carriage return (CR) to be usable on Macintosh. This can be achieved by:

- Copy and paste, if using an emulator, such as BasiliskII, or SheepShaver, as this automatically converts LF to CR
- Search and replace, from within ThinkC/Symantec C++
- Search and replace, from within Macintosh Programmers Workshop (MPW)

#### Converting using ThinkC

This can be performed using Symantec C++ version 7. The LF now show up and it is possible to replace them using the Find/Replace All method: 

- Open the **Find** dialog (Menu **Search** > **Find...**, or <kbd>Command</kbd> + <kbd>F</kbd>).
- It is possible to copy the LF character (that was generated in textEdit on OSX) in ThinkC and search for it.
- Paste the character into the **Search For:** text box.
- In the **Replace with:** text box, generate the CR character by pressing <kbd>Command</kbd> + <kbd>Return</kbd> (not <kbd>Enter</kbd>)

  From [page 101](https://archive.org/details/THINKCUsersManual1989/page/n113/mode/2up) of the [ThinkC user’s manual](https://archive.org/details/THINKCUsersManual1989)

   > **Finding Non-printing Characters**
   >
   > To look for tab and return characters, hold down the Command key as you type them into the Search for and Replace with fields. To insert other non-printing characters, use the Copy command to copy them into the Clipboard, then Paste them into the Search for and Replace with fields. 

- Check the **Wrap Around** checkbox
  - or ensure that your cursor was at the start of the text fle before opening the **Find** dialog 
- Then hit the **Find** button to find the next occurence of the LF character
- Finally, select **Replace All** from the **Search** menu (there is no keyboard shortcut for **Replace All**)
- On subsequent files, once opened, you can just select **Replace All** - there is no need to reopen the **Find** dialog again.

Symantec C++ 8.6 appears to recognise the LF from OSX, without needing the Find and Replace with CR, as the source files are displayed correctly. However, there seems to be an issue when compiling, if the LF are not replaced - weird errors occur.

#### Converting using MPW

In Macintosh Programmers Workshop, `CR` is `∂n`. Use the following command

```none
Replace -c ∞ /LF/ "∂n"
```
Where `LF` is the LF character (that was generated in textEdit on OSX). Use copy and paste to insert the LF character into the command (as there doesn't seem to be a way to generate that character using the keyboard). 
