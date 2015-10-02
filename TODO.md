* BUG: crash on backing from a GitBranchPage which is doing Pushing
* BUG: fix git not handling removed files (use smart git update on renamed/deleted diffs)
* BUG: some german letters not displaying (probably filtered by your unicode mapping function)
* allow git to reset the current branch to a specific commit
* inside GitRepoPage, add an action for deleting an untracked file 
* per-filetype settings on prediction and correction
* persistence for tabs
* project settings (move changing of project path into this)
* autoindent (dmych)
* tab key on the virtual keyboard / resolve Enter+ not working on virtual keyboard
* disable wrapping
* syntax files
    * AutoIt syntax file
    * matlab
* Term48 integration






## Blackberry bugs (waiting to report)

1. [No fix] sometimes when executing a find command, the selection is out of the view
2. [No fix] when there are two spaces at the beginning of a line, and the line is below another line, entering a non-space character screws it up
    * very very weird

## Long-term

1. stop updating all bounded views whenever one of the views changes
    - take advantage of the fact that there is only one view active, we only need to update the remaining views when it becomes active
    - should be fairly simple to implement
2. make optimization improvements to srchilite
