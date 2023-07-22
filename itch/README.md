# The itch.io page

Everything the store page needs, and what to do with it. Nothing in this
repository uploads anything — every command writes files into `dist/` and stops
(bar one: `make press` also refreshes `docs/screenshot.png`, the picture in
README.md), and you drag them into the dashboard yourself.

| What                 | Where it is                                                                                      |
| -------------------- | ------------------------------------------------------------------------------------------------ |
| Description          | `itch/page.html` — open it in a browser, select all, paste into the description editor           |
| Install instructions | `itch/install-instructions.md` — paste as plain text into that field                             |
| Pictures             | `make press` → `dist/press/` (screenshots, cover, GIFs, wallpaper, `MANIFEST.txt` with captions) |
| macOS build          | `make mac` → `dist/Breakup-1.0.0-macos.zip` (needs a Mac and an Apple ID)                        |
| Windows build        | `make win` → `dist/Breakup-1.0.0-windows-x64.zip` (works on the Mac)                             |
| Linux build          | `make linux` on a Linux, or the **payloads** button on the Actions tab                           |

`itch/page.md` is the same description in markdown, and it is the copy to edit;
`page.html` is what the editor can swallow, since it has no markdown and no
source view. If you change one, change the other.

## Creating the page

| Field                       | Value                                                                                                                     |
| --------------------------- | ------------------------------------------------------------------------------------------------------------------------- |
| Title                       | `Breakup`                                                                                                                 |
| Project URL                 | `breakup`                                                                                                                 |
| Short description / tagline | `A neon brick-breaker in twenty hand-built towers - no sprite sheet, no audio folder, every pixel and note made in code.` |
| Classification              | Games                                                                                                                     |
| Kind of project             | Downloadable                                                                                                              |
| Release status              | Released                                                                                                                  |
| Pricing                     | **Donate** — the download stays free and a support button appears                                                         |
| Genre                       | Action                                                                                                                    |
| Tags                        | `arcade`, `breakup`, `arkanoid`, `2d`, `retro`, `singleplayer`, `chiptune`, `difficult`                                   |
| Made with                   | SDL                                                                                                                       |
| Average session             | About an hour                                                                                                             |
| Languages                   | English                                                                                                                   |
| Inputs                      | Keyboard, Mouse                                                                                                           |
| Accessibility               | **nothing** — see below                                                                                                   |
| Community                   | Comments on                                                                                                               |
| Visibility                  | Draft until the builds are up, then Public                                                                                |

Two of those rows are worth a sentence each.

**Not `procedural-generation`,** however tempting: the _art_ is generated at
runtime and the levels are hand-drawn, and that tag means the opposite to
somebody browsing. Type the tags into the field rather than pasting them and take
itch's autocompletion where it offers it — a tag nobody else uses is a tag nobody
browses.

**Accessibility stays empty.** The controls are not rebindable, there is no
reduced-motion switch, and neither the screen shake nor the flash an explosion
throws can be turned down. Ticking _Configurable controls_ because the paddle
takes both a mouse and the arrow keys is the kind of thing somebody chooses a
game on and then cannot play.

## The pictures

`make press` builds all of them into `dist/press/`, captured from the running
game — see `tools/press_kit.sh`. Captions for every file are in
`dist/press/MANIFEST.txt`, each with the command that produced it.

- **Cover image**: `cover-630x500@2x.png`. itch shows it at 630×500 and as small
  as 315×250, and it is the only picture most people will ever see of this game.
  It is the Citadel screenshot cropped below the HUD, because a bar reading SCORE
  001500 is a screenshot's furniture and a cover has no use for it.
- **Screenshots**: the number in the filename is the order — `01` is the one that
  gets seen. The five to use are `01-citadel`, `02-honeycomb`, `03-vault`,
  `04-furnace` and `05-stormfront`: one per world plus the multiball, in that
  order. `06-levels` is the sixth if you want the twenty-level grid on the page.
  Use the `@2x` files: itch scales down cleanly and up badly.
- **Not `15-win`** — it is the last screen of the game. `13-nebula` is the only
  picture of a full five-times cascade and it is worth a devlog or a post, but at
  gallery size it is a wall of floating numbers, so it is not one of the five
  either.
- **A GIF** can go at the top of the gallery. `32-loop-furnace.gif` is the one
  that reads best small — a chain of explosions with capsules dropping out of it.
  They are 400×300 and around 2 MB each; anything much over 3 MB stops autoplaying
  on the page, which is why the sizes are printed as they are built — and why they
  are half-size, since at 800×600 these land past 4 MB.
- **Page background**: `wallpaper-1920x1080.png`, under Edit theme, if you want
  it.

The pictures are captures of a running game rather than files kept in the
repository, so after any change to the art, `make press` rebuilds every one of
them rather than leaving the page a version behind. The same commit always
photographs the same game: the run is seeded and stepped at a fixed rate, so two
runs produce byte-identical files.

## The uploads

One archive per platform. Drag each in, tick the platform, and leave _This file
will be downloaded_ selected — there is no web build.

`make mac` also cuts a DMG, for a GitHub release or a link in an email. Offering
a visitor a DMG _and_ a zip for macOS is a choice they have no way to make, so
upload one file per platform and no more.

## Before it goes public

- [ ] The macOS build is **notarized and stapled**, not just signed — otherwise
      every Mac but yours says "Apple could not verify". `make mac` does the whole
      thing in order, which is the point of it being one target: the zip is a
      snapshot of the bundle, so it has to be cut after the ticket is stapled in
      and not before.
- [ ] Each of the three downloads has been unpacked and started on the platform it
      is for — the Windows one especially, since it is cross-built and nothing on
      the build machine can run it.
- [ ] The version in `src/version.h` matches the archive names and what the page
      says.
- [ ] The cover reads at 315×250. Look at it that small before believing it.
- [ ] The font is somebody else's work, and this is the one item here that is
      still open. `assets/font.ttf` is PC Senior by codeman38, the one file in the
      binary this repository did not write. zone38.net states a licence for
      exactly one of his fonts and it is not this one; the "free for personal and
      commercial use, bundling allowed, no re-selling in a font collection" terms
      come from the font distributors rather than from him. Settle it before the
      page goes up — **NOTICE.md** has the three ways to, one of which is
      switching to Press Start 2P (same author, same kind of face, OFL, and a
      change to how the whole game looks). Everything else here is MIT and yours.

## Not in here: a trailer

`make press` photographs the game and it can only do that because the game will
play itself — `BREAKUP_AUTOPLAY` steers the paddle at the ball, which is what
makes the GIFs real play instead of an idling background. What it cannot do is
play _well_, and it never releases a caught ball on purpose or saves a laser for
an enemy. A real trailer wants a recording of somebody playing with the sound on,
which is twenty minutes with any screen recorder — and the sound is half of what
this game has.
