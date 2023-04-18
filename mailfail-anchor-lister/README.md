# mailfail-anchor-lister

by CelesteBlue

A tool to create an html file that contains URI exploiting the MaILFail vulnerability in PS Vita Email application on System Software version 2.10-3.52.

## Support

System Software version older than 2.10 do not support HTML files in the Email application.

System Software version in the 2.10-3.52 range supports email: URI call from the Email application.

System Software version more recent than 3.52 has URI call from HTML anchor blocked in the Email application.

## MSDOS

Step 1:

For each partition of the PS Vita filesystem, drag and drop the partition folder to the corresponding BAT file, or run the BAT with the partition folder as argument.

It will produce ``mailfail_<partition>.html``.

Example: mailfail_os0_lister.bat D:/os0_folder

Step 2:

Run mailfail_html_maker.bat.

It will produce ``index.html``.

Step 3:

Inject ``index.html`` on the PS Vita Memory Card by using mailfail-installer or by sending an Email containing this HTML file to the PS Vita Email account.

## UNIX

UNIX is not supported yet.