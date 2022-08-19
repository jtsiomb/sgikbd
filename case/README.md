3D printable case for the sgikbd board
======================================

![shot](http://nuclear.mutantstargoat.com/hw/sgikbd/img/sgikbd_case_cad-thumb.png)

This case was designed with OpenSCAD. In release archives, this directory
contains STL files exported from OpenSCAD, otherwise if you got this from the
git repository, you will only find the OpenSCAD source file, which you will have
to export yourself. To do so, simply load the `sgikbd.scad` file in OpenSCAD,
press F6 to generate the final geometry for the model, and then press F7 to
export to STL format.

File list
---------
  - `sgikbd.scad`: source file, open it with OpenSCAD to export STL files.
  - `sgikbd-bot.stl`: STL file containing the bottom part of the case in the
    default configuration.
  - `sgikbd-top.stl`: STL file containing the top part of the case in the default
    configuration.
  - `sgikbd-top-allports.stl`: STL file containing the top part of the case, with
    all SGI ports open (no break-away panels).

Options
-------
If you open the source file in openscad and export to STL manually, you can
tweak a number of options about the case. You will find these options at the top
of the file. The default configuration includes break-away panels for all SGI
ports, which you'll have to remove for every connector you have actually
populated on the board. However you have the option to either make any of the
ports completely solid, if you're sure you're not going to populate that
connector in the future, or leave any of the ports completely open.

  - `breaktabs`: set to false to leave ports open, instead of having the default
    break-away panel.
  - `no_db15`: set to true to completely block the DB15 port with solid wall.
  - `no_db9`: set to true to completely block the DB9 port (IRIS 4D series) with
    solid wall.
  - `no-minidin`: set to true to completely block the MiniDIN port (Indigo/Onyx)
    with solid wall.
  - `have_top`: set to false to omit the top part of the case.
  - `have_bottom`: set to false to omit the bottom part of the case.

License
-------
Creator: John Tsiombikas <nuclear@mutantstargoat.com>

This case design is public domain, feel free to copy, modify, and use, as you
will, without restriction.
