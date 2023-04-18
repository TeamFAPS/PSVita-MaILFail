# MaILFail

by CelesteBlue

A toolset to exploit the PS Vita MaILFail vulnerability, allowing arbitrary file read and write on the PS Vita storage.

## Teaser

![MaiLFail_file_dump](https://github.com/TeamFAPS/PSVita-MaILFail/blob/a4d37cd86a9b0f22991dd1be4a2263b8d2fd3b82/MaiLFail_file_dump.png)
![os0_anchor](https://github.com/TeamFAPS/PSVita-MaILFail/blob/a4d37cd86a9b0f22991dd1be4a2263b8d2fd3b82/os0_anchor.png)
![vs0_and_vs0_tarpatch_anchor](https://github.com/TeamFAPS/PSVita-MaILFail/blob/a4d37cd86a9b0f22991dd1be4a2263b8d2fd3b82/vs0_and_vs0_tarpatch_anchor.png)

## Tools

### mailfail-installer

The MaILFail installer is a PS Vita homebrew that injects a MaILFail file dumper HTML file into the Email application folder on the Memory Card.

The MaILFail file dumper HTML file can be created either using mailfail-anchor-lister or mailfail-keyboard.

Note that for injection into another PS Vita's Memory Card, you need to initialize the Email application on the target PS Vita then insert the Memory Card and run the installer application on the hacked PS Vita, and finally put the Memory Card back inside the target PS Vita.

### mailfail-anchor-lister

The MaILFail anchor lister is a bunch of scripts to create a MaILFail file dumper HTML file.

This HTML file contains the list of known file paths of the PS Vita filesystem.

### mailfail-keyboard

The MaILFail keyboard is a MaILFail file dumper HTML file.

This HTML file contains a virtual touchscreen-only keyboard that allows to type the path of a file to dump.

Warning: PS TV is likely not supported for now because it has no touchscreen and the current virtual keyboard can be pressed only by touch.

### mailfail-usermode

NOT RELEASED YET

The MaILFail usermode is a usermode exploit that dumps any file of the PS Vita filesystem to the Memory Card.

### mailfail-file-injector

NOT RELEASED YET

The MaILFail file injector is a tutorial explaining how to exploit the MaILFail arbitrary file write vulnerability.

## Impact

### MaILFail arbitrary file read

This works on System Software version 2.10-3.55. This might work on System Software version 2.00-2.06.

Any file of the filesystem can be copied to the PS Vita Memory Card in ux0:email/message/XX/.

### MaILFail arbitrary file write

This works on System Software version 2.10-3.55. This might work on System Software version 2.00-2.06.

Files can be written to partitions mounted as writeable: ux0, ur0, ud0, grw0 for example. The source file is an arbitrary file attached to an email.

"../" trick allows to write files to storage.

"../" works in Calendar email:send?attach= trick even on System Software version 3.74, but cannot bypass ux0:calendar/ sandbox

### email:send?attach= URI call from HTML anchor in Email application

This works on System Software version 2.10-3.52. This might work on System Software version 2.00-2.06.

Send to yourself an email with the URI as an anchor <a href> html markup.

### email:send?attach= URI call from Calendar application

This works on System Software version 3.30-3.74.

1) Insert a PS Vita Memory Card in your non-hacked PS Vita.
2) Open the Email application.
3) Setup the Email application with a dummy email account.
4) Open the Calendar application.
5) Create an event in the calendar with the description set to 'email:send?attach=ux0:/calendar/calendar.db'
6) Click on the 3 "dots" and press "Send via E-Mail". Email app opens.
7) Enter a dummy email address like nobody@none then click on "Send". The Email application should close automatically and the Calendar application should reopen automatically.
8) Open the Email application.
9) That message you just pseudo-sent should appear in the Outbox folder (not Sent, nor Draft), and should embed the chosen file (calendar.db in the example). Else retry.
10) Remove the memory card from your non-hacked PS Vita and insert it in your hacked PS Vita.
11) Open VitaShell on your hacked PS Vita.
12) Go to the partition on which the Memory Card is mounted (ux0 by default but can be another mount point if you use a storage plugin like StorageMgr).
13) Go to /email/message/XX/ and find there the file that you just dumped. It will be a file named "XX.Part.Y.dat" where XX and Y are numbers.
14) Send this file to your personal computer.

Once you have done all the steps, check validity your dumped calendar.db to confirm that it worked.
108KB is a classic calendar.db file size. It becomes bigger the more you use the Calendar application...
Then dump the files that interest you, not calendar.db which is just the calendar database.
For example: email:send?attach=os0:/kd/acmgr.skprx

https://hackinformer.com/2016/01/24/psvita-dump-files-3-55/

### email:send?attach= URI call from usermode code execution

This works on System Software version 2.10-3.74. This might work on System Software version 2.00-2.06.

## Documentation

### Errors codes

* C2-14399-6 0x80106A41 = file does not exist, really does not exit, not just inaccessible, certainly "sceIoOpen failed: file does not exist".
* C2-14433-5 0x80106A91 when Email account does not contain an outbox
* C2-14389-5 sometimes when sending an email on System Software version 3.74

### About Memory Card files extraction

A less simple way to dump files from PS Vita is to use the write access provided by the mailfail trick to dump the files to the PS Vita memory card or user internal storage:
* A) To ux0:pspemu/PSP/SAVEDATA/. See https://wololo.net/talk/viewtopic.php?f=65&t=44935
* B) To ux0 music mail.db. Less simple but can dump files without size limitation other than ux0 available space. See https://wololo.net/2016/01/08/ps-vita-dumping-files-without-limitation/ or https://wololo.net/talk/viewtopic.php?p=404942#p404942.

QCMA trick: See https://hackinformer.com/2015/12/09/dumping-memory-card-ux0-without-using-pboot/.

### About URI calls
- psns: URI works on System Software version 1.03-3.74 from web browser and on System Software version 2.10-3.74 from Email application.
- email:, psgm: URI works from HTML anchor in Email application on System Software version 2.10-3.52. Fixed on System Software version 3.55.
- email:, psgm: URI does not work from web browser on System Software version 2.10-3.74.

### About Calendar application

Calendar application is only present since PS Vita System Software version 3.10.

Since PS Vita System Software version 3.30:
Users can now attach and send events created in [Calendar] to [Messages] and [Email]. Recipients can save those events in their own calendars.
Users can now add Friends and other players to events created in [Calendar].
The Calendar app’s LiveArea now supports the next six tagged events.

### About Email application

System Software version 2.00:
[Email] has been added as an application.

System Software version 2.10:
Enhancements to [Email] now allow users to view HTML messages, add multiple email addresses to contacts, and search messages.

System Software version 3.00:
Contacts can now be synchronized from Gmail and Yahoo! Mail using CardDAV.

On System Software version 2.00-2.06, most received emails do not open even when just embedding a simple HTML code however raw text works. One gets error C2-14393-0 when forwarding or opening a html-type email.

On System Software version 3.74, sometimes error C2-14389-5 when sending an email.

### Other

https://playstationdev.wiki/psvitadevwiki/index.php/FailMail

## Credits

* mr.gas and MajorTom for discovering the MaILFail vulnerability and exploiting it in many ways
* SilicaAndPina for doing many tests and discovering even more tricks after mr.gas and MajorTom
* CelesteBlue for summarizing the vulnerabilities, for writing tools to ease exploitation and to target more System Software versions
* xyz for writing the base of the MaILFail installer application, which is the HENkaku offline installer
