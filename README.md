FaceBuilder
===========

FaceBuilder is a little toy application that lets you construct faces
by putting together eyes, nose, mouth, head, hair and some additional
items. You can also move, scale and rotate each of those face-parts as
you like. The results can be saved to XML files and it currently
provides ~100 faceparts in total.


Running
-------

> Getting this to run as of 2015 is a little hard, as Ubuntu no longer
> ships the necesarry libraries.

You need Ruby and the Ruby bindings for Gtk and GnomeCanvas. Once you
have those just run:

    ruby ./facebuilder.rb


Controls
--------

    PgUp, PgDown: scale facepart
    Home, End:    rotate facepart
    Cursorkeys:   move facepart

The face parts itself can be selected via the GUI


Customizations
--------------

If you want to customize this programm just add new face parts to
`data/$FACEPART/`, the programm should be able to find them
automatically then.
