# Third-party material

Everything in this repository is covered by [LICENSE](LICENSE) (MIT) **except the
file listed below**, which somebody else wrote and which this project only
redistributes. Every sprite, background, sound effect and music track is
generated in code at runtime — the font is the one asset on disk, and it is the
one thing here that is not ours to license.

## assets/font.ttf — PC Senior

- **Author:** codeman38 (Zone38) — <https://www.zone38.net/font/>
- **What it is:** a TrueType conversion of the 8×8 pixel font from IBM CGA
  hardware, released in 2004. The name table in the file carries
  "TrueType conversion © 2001 codeman38".
- **How this project uses it:** embedded verbatim into the binary by
  `embed_assets.sh`, unmodified, and rendered by SDL_ttf.

### Licence status — read this before shipping

The author's own page states an explicit licence for exactly one of his fonts
(Press Start 2P, under the SIL Open Font License) and **states none for PC
Senior**. The terms circulated by the font distributors — that it is free for
personal and commercial use, may be bundled in free or commercial projects, may
not be re-sold as part of a commercial font collection, and asks for credit to
codeman38 / zone38.net on modified versions — come from those distributors
rather than from a licence file in this repository.

That is second-hand, and a released game is the wrong place to find out it was
wrong. Before a public release, do one of:

1. Get the original download from zone38.net and keep its readme/licence file
   next to the font in this repository, so the terms travel with the asset; or
2. Ask codeman38 directly and record the answer here; or
3. Switch to a font with an unambiguous licence — **Press Start 2P** is by the
   same author, is the same kind of pixel face, and is OFL. That is a change to
   how the whole game looks, so it is a design decision and not a formality.

Credit is given regardless of which of those it turns out to be: it costs
nothing and the font deserves it.
