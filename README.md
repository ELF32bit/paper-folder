# 🧻 Paper Folder
Paper folder provides a way to work with FOLD (Flexible Origami List Datastructure).<br>
Draw crease patterns and simulate them, freeze the intermediate results as key frames.<br>
Export precomputed folding animation as an easy to parse series of meshes.<br>

> My research curiosity here is to classify locally flat-foldable crease graphs,<br>
with the condition that their dual graphs also retain interior flat-foldability.<br>
One example is a [hyperbolic paraboloid origami](https://github.com/ELF32bit/origami-hyperbolic-paraboloid) without a central square.

## Current features
* FOLD specification (version 1.2) validation.
* Convert from and to FOLD, SVG, OBJ files.
* Generate locally flat-foldable SVG crease patterns.

> Such crease patterns satisfy [Maekawa's](https://en.wikipedia.org/wiki/Maekawa%27s_theorem) and [Kawasaki's](https://en.wikipedia.org/wiki/Kawasaki%27s_theorem) theorems.<br>
Try proving that a triangle **dual** can never satisfy |M - V| = 2 condition.

State | Module | Commentary
-- | ---------------------- | ----------------------------------------------------------- |
🟨 | collections            | Feels a bit undercooked.<br>
✅ | definitions            | Modern, Rust-like syntax sugar.<br>
🟨 | fold_deserialization   | No support for SVG triangulation yet.<br>Math module needs BVH2 and Bezier2.<br>
✅ | fold_generation        | Desperate for clever math optimizations.<br>
❌ | fold_graph             | Work in progress! Basic methods only.<br>Math module needs 3x3 covariance matrix.<br>`fold_graph_VV_from_FV` 2D example.<br>`fold_graph_VF_from_VV` complex walk.<br>`fold_graph_EF_from_FV` more hard math.<br>`fold_graph_FV_*` family not implemented.<br>
🟨 | fold_serialization     | OBJ writing is very primitive.<br>Unique vertices are not tracked properly.<br>SVG could support more visual modes.<br>
❌ | fold_simulation        | Considering a different approach.<br>More research is necessary.<br>
✅ | fold_specification     | Pretty much complete.<br>Supports generic mesh arrays.<br>
✅ | fold_validation        | 100% bullet-proof, very fast.<br>
✅ | lua_modules            | Lua bindings, see the [example](https://github.com/ELF32bit/c-lua-template/blob/main/sources/lua_modules/example.h).<br>
✅ | lua_scripts            | Command line parsing routines.<br>
🟨 | math                   | Needs full BVH2, BVH3 accelerators.<br>Matrix, Plane, Bezier curves are missing.<br>Triangle/Polygon helpers would be nice.<br>

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
