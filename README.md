# 🧻 Paper Folder
Paper folder provides a way to work with FOLD (Flexible Origami List Datastructure).<br>
Draw crease patterns and simulate them, freeze the intermediate results as key frames.<br>
Export precomputed folding animation as an easy to parse series of meshes.<br>

## Current features
* FOLD specification (version 1.2) validation.
* Convert from and to FOLD, SVG, OBJ files.

## Building the project
Run the command inside a terminal.<br>
```Bash
make && \
./paper-folder validate \
"assets/rabbit-ear/kissing-squares.fold"
```
```Go
⛔INVALID: "assets/rabbit-ear/kissing-squares.fold"
⚠️frame:0: faces_vertices and faces_edges mismatching winding
⚠️frame:0: faces_vertices and faces_faces mismatching winding
⚠️frame:0: faces_edges and faces_faces mismatching winding
```
