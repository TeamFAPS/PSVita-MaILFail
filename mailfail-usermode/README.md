# MaILFail usermode

In order to dump PS Vita files, email:send?attach= URI can be called after getting usermode code execution.

This works on System Software version 2.10-3.74. This might work on System Software version 2.00-2.06.

## Entrypoints

* WebKit
* fSELF on activated Kits
* game savedata exploit, prohibited on non-activated Kits and in Manufacturing mode
* PSPemu privilege escalation exploit, prohibited on non-QAF Kits and in Manufacturing mode

## Implementations

I choosed to implement the WebKit exploit only because it is the most supported entrypoint. NOT RELEASED YET.