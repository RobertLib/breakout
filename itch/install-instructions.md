# Install instructions

Paste this into the page's **Install instructions** field (Edit game → below the
uploads). It is a separate field from the description and it is the one people
read right after downloading, which is the moment a Gatekeeper dialog or a
missing DLL turns a free game into a shrug.

Keep it short. This is not the place for the features.

---

**macOS** — Unzip and drag `Breakup.app` wherever you keep applications. It is
signed and notarized, so it opens by double-clicking; nothing else is needed and
nothing is installed. Needs macOS 11 or newer, Intel or Apple silicon.

**Windows** — Unzip the folder somewhere and run `breakup.exe`. Keep the three
SDL3 DLLs beside it. Needs 64-bit Windows 10 or newer. The build is not signed
with an Authenticode certificate, so Windows may say the publisher is unknown —
_More info_ → _Run anyway_, or build it yourself from the source.

**Linux** — Unpack the tarball and run `./breakup`. The `lib/` folder next to it
holds the SDL3, SDL3_ttf and SDL3_mixer this build was made against, and the
binary looks for them there, so keep the two together and install nothing. Built
on Ubuntu 24.04 for x86_64 (glibc 2.39); on an older distribution, building from
source is a `make` away.

**Everywhere** — Settings, progress and high scores live in one folder and
nowhere else: `~/Library/Application Support/RobertLib/Breakup` on macOS,
`%APPDATA%\RobertLib\Breakup` on Windows,
`~/.local/share/RobertLib/Breakup` on Linux. Deleting it starts the twenty
levels again from the first. The game does not touch the network.

**Controls** — mouse or the arrow keys move the paddle, Space (or a click)
serves the ball and fires the lasers, Esc or P pauses, F goes fullscreen. They
are not rebindable in this version.
