# Velocity Snapping

Velocity snapping and snap zones are well established terms in Quake 3 Defrag, see for instance [Jelvan1/q3df-doc](https://github.com/Jelvan1/q3df-doc).
By default Holomatch inherits exactly this behaviour from Quake 3, but when using one of the non-default modes of the Quake Virtual Machine (QVM) there is a subtle but significant change.

## Summary

The TL;DR is this: In Holomatch, if the game module is executed with the QVM bytecode just-in-time compiled, then velocity snapping behaves exactly like in vanilla Quake 3.
But if the game module is executed by the QVM interpreter or as native DLL, then the velocity components will always be rounded down towards zero rather than towards the nearest integer as in Quake 3.
So in this case, accelerating becomes more difficult, especially when already at high speeds.
This effect becomes even worse at higher framerates (`com_maxfps`), as the player physics are computed more frequently, meaning that the velocity snapping is applied more frequently too.

To influence which mode the QVM is operated in, the cvar `vm_game` can be used:

| `vm_game` | QVM                        | rounding (Holomatch) | rounding (Quake 3) |
| --------- | -------------------------- | -------------------: | -----------------: |
| `0`       | DLL **or** compiled        | zero **or** nearest  | nearest            |
| `1`       | interpreted                | zero                 | nearest            |
| `2`       | compiled                   | nearest              | nearest            |

Note that if `vm_game` is `0`, then the behaviour depends on if a native DLL of the game module is found.
If yes, then it is used and velocity snapping results in rounding to zero.
Otherwise, the compiled QVM bytecode will be used as fallback instead which results in rounding to nearest.

By default, Holomatch does not ship with a native DLL version of the game module and `vm_game` is set to 0, so you get the vanilla Quake 3 behaviour.

## Code Difference

In Quake 3, velocity snapping happens in the function `PmoveSingle` by calling `trap_SnapVector` ([Quake-III-Arena/code/game/bg_pmove.c#L2015](https://github.com/id-Software/Quake-III-Arena/blob/dbe4ddb10315479fc00086f08e25d968b4b43c49/code/game/bg_pmove.c#L2015)).
Function `PmoveSingle` is included in the game module which runs in the QVM.
So from inside the QVM, `trap_SnapVector` is called, which leads out of the QVM ([Quake-III-Arena/code/game/g_syscalls.c#L227](https://github.com/id-Software/Quake-III-Arena/blob/dbe4ddb10315479fc00086f08e25d968b4b43c49/code/game/g_syscalls.c#L227)) and ends up in native engine code, which finally calls the function `Sys_SnapVector` ([Quake-III-Arena/code/server/sv_game.c#L454](https://github.com/id-Software/Quake-III-Arena/blob/dbe4ddb10315479fc00086f08e25d968b4b43c49/code/server/sv_game.c#L454)).
The implementation of `Sys_SnapVector` basically boils down to all floating-point components of the velocity vector being rounded to the nearest integer (for details see [Rounding Implementation Details](#rounding-implementation-details)).

In Holomatch on the other hand, the snapping happens in a "call" to `SnapVector` ([holomatch/Code-DM/game/bg_pmove.cpp#L2624](https://github.com/kugelrund/stvoyHM_strafe_tools/blob/fab9bebea7bfc5f0187bd4a02cd80858926f33d7/Code-DM/game/bg_pmove.c#L2242))
which is a simple macro defined as ([holomatch/Code-DM/game/q_shared.h#L464](https://github.com/kugelrund/stvoyHM_strafe_tools/blob/fab9bebea7bfc5f0187bd4a02cd80858926f33d7/Code-DM/game/q_shared.h#L529)):

```cpp
#define SnapVector(v) {v[0]=(int)v[0];v[1]=(int)v[1];v[2]=(int)v[2];}
```

So here the `float` values are just cast to `int` directly in the QVM, without delegating to the native engine code.
Note that the following implicit conversion back to `float` does not have any relevant effects, as it can be performed losslessly for any reasonable velocity values.
That means that the behaviour of `SnapVector` only depends on how the QVM executes the float-to-int conversion.
Since there is some flexibility to how the QVM is used, this can happen in a few different ways.
While we do not have the engine source code for Holomatch, it does seem to behave just like vanilla Quake 3 in this case, so we can follow its engine source code.
From ([Quake-III-Arena/code/qcommon/vm.c](https://github.com/id-Software/Quake-III-Arena/blob/dbe4ddb10315479fc00086f08e25d968b4b43c49/code/qcommon/vm.c)) we can learn that the QVM code is executed in either of three ways:

1. `VMI_NATIVE`: The game module is loaded as native DLL and executed directly.

   In this case, the native DLL has been generated with a normal C compiler, so we just get the standard C behaviour for casting a `float` to an `int`.
   That means the value is truncated, i.e. it is rounded towards zero.

2. `VMI_BYTECODE`: The game module is loaded as QVM file and the bytecode is run by the interpreter.

   In this case, LCC will have generated `OP_CVFI` for the conversion from `float` to `int` in the QVM bytecode.
   The interpreter runs this opcode by simply casting the `float` to `int` ([Quake-III-Arena/code/qcommon/vm_interpreted.c#L867](https://github.com/id-Software/Quake-III-Arena/blob/dbe4ddb10315479fc00086f08e25d968b4b43c49/code/qcommon/vm_interpreted.c#L867)).
   Since the interpreter is in natively compiled engine code, this also follows standard C behaviour.
   So in this case the value is truncated too.

3. `VMI_COMPILED`: The game module is loaded as QVM file and just-in-time compiled to native instructions which are executed.

   Just like in the previous case, the QVM bytecode will contain `OP_CVFI`.
   But now this operation is compiled into native machine code, where the relevant architecture for Holomatch is x86.
   The emited instructions happen to boil down to the floating-point value being rounded to the nearest possible integer (for details see [Rounding Implementation Details](#rounding-implementation-details)).

So in summary, in Holomatch velocity snapping only rounds to the nearest integer like Quake 3 does when the game module is executed by loading and just-in-time compiling the QVM bytecode to native x86 instructions.
Otherwise, velocity snapping performs rounding towards zero.

### Reason for the Difference

According to a changelog in the Quake 3 source release ([Quake-III-Arena/code/unix/ChangeLog#L1432](https://github.com/id-Software/Quake-III-Arena/blob/dbe4ddb10315479fc00086f08e25d968b4b43c49/code/unix/ChangeLog#L1432)), the `trap_SnapVector` function may have been introduced only as late as December of 2000.
The surrounding changelog entries and comments in the corresponding code seem to indicate that float-to-int rounding had been causing quite some trouble and inconsistencies.
In particular it looks like in some early version, the just-in-time compilation of QVM code to x86 would emit code that rounds to the nearest integer ([Quake-III-Arena/code/qcommon/vm_x86.c#L996](https://github.com/id-Software/Quake-III-Arena/blob/dbe4ddb10315479fc00086f08e25d968b4b43c49/code/qcommon/vm_x86.c#L996-L1000)) for `OP_CVFI` which is the intermediate representation of a cast from `float` to `int` generated by LCC.
And so to ensure compatibility with that behaviour, important physics code like the velocity snapping would have had to make sure to use rounding to nearest in all options between native DLL, compiled QVM and interpreted QVM code.

Unfortunately, the history of the actual changes in the Quake 3 code does not seem to be available, so we can only speculate what the early versions might have looked like exactly.
But it does not seem unreasonable to assume that early on Quake 3 had been using the same `SnapVector` macro for the velocity snapping in `bg_pmove.c` that Holomatch uses.
In fact that macro does still exist in the released version of the Quake 3 source code ([Quake-III-Arena/game/q_shared.h#L646](https://github.com/id-Software/Quake-III-Arena/blob/dbe4ddb10315479fc00086f08e25d968b4b43c49/code/game/q_shared.h#L646)), but it is only used in other contexts.

So with Elite Force being released in September of 2000, it seems likely that the version of the Quake 3 engine that Raven was working with did not have the `trap_SnapVector` version yet and instead used the simple `SnapVector` macro.

### Rounding Implementation Details

To confirm the previous claim that both the `Sys_SnapVector` implementation and the emited x86 instructions for `OP_CVFI` come down to floating-point values being rounded to the nearest integer, there are a few cases to look at.

For `Sys_SnapVector`, the Windows implementation is using inline assembly ([Quake-III-Arena/code/win32/win_shared.c#L68](https://github.com/id-Software/Quake-III-Arena/blob/dbe4ddb10315479fc00086f08e25d968b4b43c49/code/win32/win_shared.c#L68)), while the Unix implementation uses pure assembly ([Quake-III-Arena/code/unix/snapvector.nasm#L41](https://github.com/id-Software/Quake-III-Arena/blob/dbe4ddb10315479fc00086f08e25d968b4b43c49/code/unix/snapvector.nasm#L41)), but both essentially use the x87 instruction `fistp`.
This instruction rounds a floating-point value according to the current rounding mode and stores it as an integer to some destination ([Intel® 64 and IA-32 Architectures Software Developer’s Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)).
So in practice this will basically always mean rounding to nearest, as that is the default rounding mode.
In both cases this integer is then coerced back into a floating-point value, which for all reasonable velocity values is a lossless conversion.

There is also a pure C implementation of `Sys_SnapVector` used for Apple/PowerPC and up to some point for Linux ([Quake-III-Arena/code/unix/unix_shared.c#L151](https://github.com/id-Software/Quake-III-Arena/blob/dbe4ddb10315479fc00086f08e25d968b4b43c49/code/unix/unix_shared.c#L151)), which uses the C standard library function `rint` ([cppreference](https://en.cppreference.com/w/c/numeric/math/rint.html)).
This function also performs rounding to an integer value using the current rounding mode, which in practice means rounding to nearest too.

As for the instructions emited for `OP_CVFI`, tn the Quake 3 source code they depend on the macro `FTOL_PTR` ([Quake-III-Arena/code/qcommon/vm_x86.c#L996](https://github.com/id-Software/Quake-III-Arena/blob/dbe4ddb10315479fc00086f08e25d968b4b43c49/code/qcommon/vm_x86.c#L996)).

- If `FTOL_PTR` is not set, then `fistp` is used directly, much like we saw in `Sys_SnapVector` before.

  So in this case we get the default rounding mode again, which in pratice will be rounding to the nearest integer.

- If  `FTOL_PTR` is set, then on Unix the function `qftol0F7F` is called, which sets the rounding mode to rounding towards zero and then executes `fistp` ([Quake-III-Arena/code/unix/ftol.nasm#L100](https://github.com/id-Software/Quake-III-Arena/blob/dbe4ddb10315479fc00086f08e25d968b4b43c49/code/unix/ftol.nasm#L100)).
  On Windows, the function `_ftol` is called, which seems to be an old MSVC-specific compiler intrinsic ([MSVC _ftol](https://learn.microsoft.com/en-us/cpp/build/reference/qifist-suppress-ftol)) that does the same thing.

  So in this case, we get rounding towards zero.

In the released version of the Quake 3 source code, `FTOL_PTR` **is** defined, leading to rounding towards zero.
This is at odds with the previous claim that `OP_CVFI` gets compiled to rounding to nearest in Holomatch.
But observations definitely show rounding to nearest in this case.
Perhaps the Holomatch engine is still using the branch in which `FTOL_PTR` is not set.
That branch is marked with the comment "was used in 1.17", so perhaps Raven was working with that version of the Quake 3 source code.

On the other hand, there is also a note ([Quake-III-Arena/code/unix/ChangeLog#L1426](https://github.com/id-Software/Quake-III-Arena/blob/dbe4ddb10315479fc00086f08e25d968b4b43c49/code/unix/ChangeLog#L1426)) in the changelog that indicates the float-to-int rounding inconsistencies which suspects that the Quake 3 engine may have been compiled with `/QIfist` ([MSVC /QIfist](https://learn.microsoft.com/en-us/cpp/build/reference/qifist-suppress-ftol)). This would cause the Windows compiler to not issue calls to `_ftol` but instead emit bare `fist[p]` instructions, meaning that the conversion would fall back to the default rounding mode again.
If Raven was using that compiler option too, that would be an alternative explanation.
