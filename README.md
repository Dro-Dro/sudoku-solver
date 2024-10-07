# Known Issues and Other Notes

The main known issue is the segmentation fault caused for the 9x9 test puzzle.
I have found that this is caused when adding in the line on sudoku.c at line 202.
This is line 202:
"pthread_create(&th[i], NULL, checkSquare, (void*) sqParams[i]);"
I think this is caused by the sqParams array, but I have no idea why it doesn't effect the other puzzles.
Even testing the 9x9 puzzle by itself gives the same seg dump result.
Not sure why this is the case.
For some reason it can work; resulting in solving the invalid puzzle.
When the invald puzzle gets listed out as invalid; the puzzle can get solved and listed out afterwards.
I think the issue is related to having the threads running at the same time. Causing issues with the next puzzle to be tested.
I found that commenting the line out makes everything else work.
I would appreciate any insight into this issue that you could possibly give me.
Thanks!