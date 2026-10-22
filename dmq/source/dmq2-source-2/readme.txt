DMQ 2 SOURCE CODE - By : PIX
Based on DMC Pro by : TrenD*KilleR

Memorial for PIX, the legend that was a god in coding, that did most of the coding for DMQ2 alone, but sadly PIX had 
inner demons, and the demons won :( Everytime Quake is on my mind I think about you old friend! 
Rest in peace PIX!
 -Billy



credits :

open source OZ DM
open source HLSCTF
Kyle@hl-elite.com
Divebomber - Linux compile and CTF
Punkbitch - Web site programmer


Per PIX :

add the int for addedCommands in \engine\progdefs.h in the globalvars_t structure.

Changelog:

Pre 10-Nov-05:
Pix did loads of work including the following options:
"fasttps" 		enables fast teleports so that the player doesnt lose speed when travelling through
"outclassed" 		will stop a 1on1 match when there is a kill-difference of 15
"trueshaft" 		allows server to draw shaft beams for all clients

10-Nov-05 - TrenD*KilleR:
"discharge"		0 will disable underwater damage from shaft to enemy players

05-Dec-05 - mfrx:
"requirematchvoted"	0 ensures match modes start as soon as enough players have joined (negates startmatchvoted)
"regenerate"		1 will enable regeneration of health up to 250/200
"shaftpush"		a float which adjusts the amount of push by the shaft (0.5 seems about right)
Midair Mode - resets scores after each round
Midair Mode - player must be 95 units from the floor to take damage - was just 42 units
Team Mode - allows selfdamage as an option
added "#define DMQ_RAMPJUMP 1" to enable ramp jumps at compilation

05-Feb-06 - mfrx
"respawntime_666"	time in seconds that the pent will respawn after being taken (default: 300)
"respawntime_quad"	time in seconds that the quad will respawn after being taken (default: 60)
"respawntime_eyes"	time in seconds that the eyes will respawn after being taken (default: 300)
"duration_666"		time in seconds that the pent will last for (default: 30)
"duration_eyes"		time in seconds that the eyes will last for (default: 30)
"duration_quad"		time in seconds that the quad will last for (default: 30)
"ssg_shots"		how many shots the SSG fires - dont go too high (server may suffer) (default: 14)
Also added some spam to the console regarding respawn times and hook.
Ramp jumps are sweet now thanks to pix
Pix added:
"shaftpushground"	How much push by the shaft when the enemy is on the ground.
"shaftpushair"		How much push by the shaft when the enemy is in the air.
Pix removed "shaftpush" as it is replaced by the vars above.

28-Feb-06 - mfrx
Ive redone all of the mode configuration files to include new cvars and make changable values a little clearer.  Goto http://mfrx.kicks-ass.net/files/7/h4xx0rs-mode-configs/default.aspx to get my files.
Runes work in all modes - use the following cvars:
"runes_enable"		1 to enable runes, 0 to disable
"runes_num_boots"	How many "Boots of Speed" runes to spawn.  Player moves at approx double speed.
"runes_num_cycle"	How many "Cycle" runes to spawn.  Doubles the rate of fire for all weapons.
"runes_num_frag"	How many "Frag" runes to spawn.  Gives the player a extra frag for every kill.
"runes_num_lowgrav"	How many "Low Gravity" runes to spawn.  Halfs gravity.
"runes_num_protect"	How many "Protection" runes to spawn.  Halfs any taken damage.
"runes_num_regen"	How many "Regeneration" runes to spawn.  Constantly adds health and armour.
"runes_num_trap"	How many "Booby Trap" runes to spawn.  Player will blow up when killed - damaging or killing nearby players.
"runes_num_vamp"	How many "Vampire" runes to spawn.  Player will gain half of the value of any damgage inflicted.
"defaultmode"		What mode should the server change to if all humans disconnect.  Must be a valid mode like "arena".
"respawntime_ammo"	time in seconds that ammo will respawn after being taken (default: 30)
"spawn_protection"	Should players be invincible when they spawn. (1 = Yes)
"protection_time"	How long, in seconds, should players be invincible for if spawn_protection = 1.
"rlflare"		1 enables the pretty flare behind rockets.  Unfortunatly its only pretty because I already had a custom animglow01.spr - I will put that on my site.  If clients dont have a custom sprite, it will look rubbish.
The server will also send a message when clients connect informing them of respawn time and things.

TODO?

Mode change from clanarena or team currently requires a changelevel command to force the mode change
Make it compile in Linux

