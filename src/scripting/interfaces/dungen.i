%module dungen

%{
    #include "stdafx.h"
%}

#if SWIG_VERSION < 0x040001
    #error Minimum SWIG version is 4.0.1
#endif

%include typemaps.i
%include vectors.i

%include images.i
%include rendering.i
%include text.i
%include util.i

%include hlvm.i
%include world.i
%include attributes.i
%include game.i
%include pathfind.i
%include ui.i
