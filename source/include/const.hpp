#pragma once
enum GameCommand
{
    GC_PAUSE, /* Pause the execution of the script */
    GC_START, /* Click Start Game button */
    GC_PLAY_NORMAL, /* Play game in normally */
    GC_PLAY_EXTENDED, /* Play game in extended mode, specified by user */
    GC_CONFIRM, /* Confirm a round */
    GC_ESCAPE, /* Press ESC button */
};