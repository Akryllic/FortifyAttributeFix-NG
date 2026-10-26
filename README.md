# Fortify Attribute Fix - NG

An SKSE plugin. A fortify effect wearing off can no longer kill you.

Skyrim stores current health as `base + permanent + temporary + damage`, where damage is negative.
A fortify raises the temporary number and your current health rises with it. When it lapses the
maximum drops, nothing restates the damage, and the sum can land below zero. This restates it.

Health, magicka and stamina. No esp, no scripts, safe to add or remove mid-playthrough.

## Requirements

* [CMake](https://cmake.org/)
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Environment variable `VCPKG_ROOT` pointing at your clone
* [CommonLibSSE-NG](https://github.com/alandtse/CommonLibSSE-NG)
	* Environment variable `COMMONLIB_SSE_FOLDER` pointing at your clone
* Visual Studio 2022 with Desktop development in C++

## Building

```
cmake --preset release
cmake --build build/release
```

For the debug build, which logs per-site and caller counters and ships a PDB:

```
cmake --preset debugpanel
cmake --build build/debugpanel
```

The counters are compiled out of the normal build rather than switched off.

## License

GPL-3.0. See [LICENSE](LICENSE).

Portions derive from MIT-licensed work, whose notices stand: `virt-x/FortifyAttributeFix` for the
published call-site addresses, `shazdeh/Fortify-Attribute-Refill-Fix`, and CommonLibSSE-NG.

The menu compiles in FLICK's API header (`Fuzzlesz/FUCK_API`, GPL-3.0) and Dear ImGui's header (`powerof3/imgui`, MIT),
pinned in `cmake/ports/flick-api`.

## Credits

* [FatGerald](https://www.nexusmods.com/skyrimspecialedition/mods/175462) for Fortify Attribute Fix
* [shazdeh](https://github.com/shazdeh/Fortify-Attribute-Refill-Fix) for Fortify Attribute Refill Fix,
  prior art on the same bug. Restating the damage slot against the new maximum, and the fact that a
  dual value modifier carries a second actor value that has to be handled on its own, both came from
  reading it
* [CharmedBaryon](https://github.com/CharmedBaryon/CommonLibSSE-NG) and
  [alandtse](https://github.com/alandtse/CommonLibSSE-NG) for CommonLibSSE-NG
* [Fuzzlesz](https://github.com/Fuzzlesz/FUCK_API) for FLICK
