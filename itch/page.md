# Breakup

**Twenty towers of neon brick. No art files.**

Breakup is a brick-breaker in the arcade tradition — a paddle, a ball, a wall
that comes apart — built as one C binary that ships with no pictures and no
sound files in it. Every brick, every glow, every nebula behind the field is
drawn by code in the second the game starts, and everything you hear, from the
click of a brick to the two music tracks each world runs on, is synthesised by a
small chiptune engine in that same second. There is no sprite sheet in the
download because there is no sprite sheet anywhere. The only file in here
somebody else made is the font it prints in.

What that buys is not a smaller download. It is that the twenty levels are the
only thing anybody had to build by hand — and all twenty were built a character
at a time.

## What you actually do

A level is a tower rather than a screen: fifteen bricks wide, thirty to sixty
rows tall, with the camera climbing it as the ball does. You are not clearing
every brick. You are after the crystals, and everything else in the tower is
either in the way or a tool.

- **Seven kinds of brick, and not all of them break.** Basic goes in one hit for
  100, durable in two for 200, tough in three for 300. Steel never goes at all
  and pays nothing — it is the shape of the room you are playing in, and learning
  to bank off it is most of a level like Bastion or Foundry. Gold is 500 for a
  single hit. Explosive takes its neighbours with it a seventh of a second later,
  and they take theirs, so one ball into the right cell of Furnace brings down
  more of that tower than you had planned. The crystals are what you are actually
  here for, at 1000 each: shatter every one and the level ends, whatever else is
  still standing.
- **Four kinds of enemy, and none of them can kill you.** A drifter wanders the
  field. A diver comes down at the paddle in a serpentine line. A splitter breaks
  into two smaller ones when it is hit. A saucer shoots, and its shot costs no
  life — it shrinks the paddle, which is worse for longer. What enemies actually
  do is stand in the way and knock your ball off the line you had. All four can
  be dealt with four ways and the game pays differently for each: 400 for the
  lasers, 300 for the ball or for catching one in an explosion, and 50 for
  driving the paddle into it, which is the cheap one and the satisfying one.
- **Nine power-ups, and the commonest one is the bad one.** About one brick in
  five drops a capsule: expand, laser, catch, extra life, multiball, slow,
  fireball, a barrier across the floor — and shrink, out of the same bricks in the
  same capsule, dark red with an exclamation mark on it, and at sixteen percent of
  all drops the single most likely thing to fall. Deciding _not_ to catch one is a
  real decision, and it has to be made while it falls.
- **A combo that pays you for not touching the ball.** Every brick broken before
  the ball comes back to the paddle raises a multiplier, a step every five
  bricks, up to five times. That is the whole scoring game: a tower taken in long
  chains pays several times what the same tower pays played safe.
- **Three lives, and a bonus for the ones you did not spend.** Finish a level
  with lives in hand and they are paid out in points.

Twenty levels across four worlds — Cyan Dawn, Emerald Drift, Ember Fields,
Violet Void — and each world has its own palette, its own sky and its own pair of
music tracks, which the levels in it alternate between. They run from a
single-screen classic (First Steps) to a sixty-row tower with three saucers in it
(The Core).

## What it remembers

Progress, settings and scores are kept in one small file in the usual place for
your platform, and nowhere else.

The level select unlocks as you go, and anything you have reached can be started
again from it directly. The high-score table keeps the best five runs with the
level each one ended on. The two volume sliders and the fullscreen flag persist.
Deleting that one file starts the whole game over.

## Controls

- Paddle on the **mouse** or on the **arrow keys**, whichever you reach for — the
  game follows whichever one moved last.
- **Space** or a **click** serves the ball, fires the lasers and lets go of a
  ball the paddle has caught.
- **Esc** or **P** pauses; the pause menu can resume, restart the level or leave
  for the main menu.
- **F** goes fullscreen and back, at any moment, and it is remembered. The
  picture is letterboxed rather than stretched, so nothing ends up the wrong
  shape on a widescreen display.
- Music and sound effects have separate volumes in the options.

## Under it

One binary, no asset folder. The graphics are generated at startup — bricks,
paddle, ball, the glow on all three, the four world skies and their parallax
starfields — and the audio is a chiptune engine that synthesises every effect and
all nine tracks into raw PCM before the title screen appears. The font and the
twenty level files are compiled into the executable, so what you unpack is the
game and the SDL it was built against and nothing else — there is no asset
folder to keep beside it and nothing to install.

The levels themselves are plain text, fifteen characters to a row, one letter per
brick. Editing one is editing a text file; a twenty-first is a file and a `make`.

It is C17 against SDL3, it opens in an 800×600 window, and it does not touch the
network.

The source is public and MIT licensed:
**https://github.com/RobertLib/breakup**

It is free. If it was worth an evening to you the donate button is there, and
thank you — but the download is not waiting on it.
