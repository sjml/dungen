%module dungen

%{
    #include "stdafx.h"

    // disabling specific warnings in generated file
    #ifdef _MSC_VER
        // C4244: "possible loss of data" when converting between numeric types
        #pragma warning( disable : 4244 )
    #endif // _MSC_VER
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
