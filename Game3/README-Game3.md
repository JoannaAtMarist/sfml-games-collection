# Bhost Guster

The game is a top-down survival shooter where the player, Jillian Holtzmann, is a lone Ghostbuster trapped in a haunted arena, trying to survive wave after wave of ghosts using a proton blaster.

---
   
<br>

## Gameplay Loop  

1. Player spawns in the center of a rectangular haunted room.  
2. A wave of ghosts spawns around the outer edges of the room and moves toward the player.  
3. Player:
   - Moves with WASD
   - Aims with the mouse and shoots with left-click
   - Uses Dash (LShift) to quickly reposition
   - Uses limited Ghost Grenades (E) to blast multiple ghosts
   - Collects pickups (health, ammo, Shield, Freeze) to stay alive longer
4. When all ghosts in the wave are destroyed:
   - Game enters LEVELING UP state
   - Player chooses one upgrade
   - A new, larger arena is generated
   - A new, larger haunting (more ghosts) is spawned  
5. When the player’s health reaches 0, the run ends and the high score is saved.  

## Goal/Objective  

- **Primary objective:** Survive as many waves of ghosts as possible and achieve the highest score.  
- **Secondary objective:** Use abilities and pickups effectively to control the haunting:
  - Use Dash to escape dangerous clumps of ghosts.
  - Save grenades for big groups or emergency clears.
  - Time Shield and Freeze pickups for clutch saves.  

## List of Player Actions  

### Base Actions  

- **Smooth 8-directional movement**  
  - Controls: **WASD**  
  - Behavior: Player moves freely in all directions within the arena, clamped to room bounds.

- **Shoot basic projectiles (Proton Shots)**  
  - Controls: **Left Mouse Button**  
  - Behavior: Fires a straight-line projectile from the player’s position toward the mouse cursor.
  - Uses ammo; player must reload.  
>Expansion ToDo: Have two guns: Pewpew and Proton.  

- **Reload**  
  - Controls: **R**  
  - Behavior: Refills the clip from spare ammo if available; plays reload or reload-failed sound.  


### Time-Limited Action  

- **Dash**  
  - Control: **Left Shift**  
  - Behavior:
    - Temporarily multiplies movement speed (`m_DashSpeedMultiplier`) for a short duration (`m_DashDuration`).
    - Has a cooldown timer (`m_DashCooldown`); cannot be used again until the cooldown expires.
  - Use: Allows quick escapes through gaps in incoming ghost formations.  


### Quantity-Limited Action   

- **Ghost Grenade / Nuke**  
  - Control: **E key**  
  - Behavior:
    - When activated, if `nukesRemaining > 0`, the game:
      - Counts how many ghosts are currently active.
      - Selects roughly half of them.
      - Repeatedly applies `hit()` until those selected ghosts are destroyed.
      - Reduces `nukesRemaining` by 1.
      - Plays a power-up / explosion sound.
      - Logs a debug line showing how many ghosts were destroyed.
  - HUD:
    - `Nukes: X` is displayed at the bottom of the HUD and updates as grenades are used.
  - Use: Emergency room-clear tool, ideal for large clumps or when surrounded.  
>Expansion ToDo: Add Explosion effects...  


### Other Controls / Debug  

- **Pause / Unpause:** Enter (while playing)  
- **Start New Game from GAME_OVER:** Enter  
- **Toggle Debug Hitboxes:** H  
  - Draws:
    - Magenta circle around the player.
    - Cyan circle around each active ghost.   


## Pickups and Upgrades   

### Base Pickups

- **Health Kit**  
  - Behavior: When the player intersects it, restores health using `increaseHealthLevel()` and plays a pickup sound.  

- **Ammo Crate**  
  - Behavior: When collected, increases `bulletsSpare` by the pickup amount and plays a reload sound.  

Both base pickups use the original Zombie Shooter style `Pickup` class with health and ammo types.  


### Additional Pickups

- **Proton Shield: Invincibility (temporary effect)**  
  - Behavior:
    - Spawns once per wave at a random location in the arena.
    - When the player touches it:
      - `shieldActive = true`
      - `shieldTimer` is set (e.g., 5 seconds)
      - Player becomes fully immune to ghost collision damage while timer > 0.
      - Pickup is removed (`shieldPickupSpawned = false`).
      - Plays the power-up sound and logs a debug message.
    - While `shieldActive`:
      - Player collisions with ghosts are ignored, and a debug message prints instead of applying damage.  

- **Containment Pulse: Freeze (temporarily stop enemy movement)**  
  - Behavior:
    - Spawns once per wave at a random location in the arena.
    - When the player touches it:
      - `freezeActive = true`
      - `freezeTimer` is set (e.g., 4 seconds)
      - All ghosts remain fully drawn and can still be damaged, but their `update()` movement logic is skipped.
      - Pickup is removed.
      - Plays the power-up sound and logs a debug message.
    - While `freezeActive`:
      - The ghost update loop runs, but movement is skipped, effectively freezing the haunting in place.  

Note: These pickups currently spawn once per wave. Their exact duration and spawn rate can be tuned later for difficulty balancing.

## Level-Up Upgrades  

When a wave is cleared, the game enters the **LEVELING UP** state and shows four upgrade options. The player chooses one by pressing **1–4**.

### Base Upgrades (from Zombie Shooter)  

1. **Increased Fire Rate**
   - Key: **1**
   - Effect: Increases `fireRate`, allowing the player to fire more shots per second.

2. **Increased Run Speed**
   - Key: **2**
   - Effect: Calls `player.upgradeSpeed()`, increasing the player’s movement speed permanently.

### Additional Upgrades   

3. **Damage Resistance**
   - Key: **3**
   - Effect:
     - Multiplies `damageScale` by 0.75 each time (to a minimum of 0.25).
     - All damage sources (ghost contact and green Slimer explosions) are multiplied by `damageScale` before being applied.
     - For example, at `damageScale = 0.5`, the player takes half damage from all ghost hits.
   - Notes:
     - Debug logs print the new `damageScale` value when this upgrade is chosen.

4. **Ghost Grenade Refill**
   - Key: **4**
   - Effect:
     - Increases `nukesRemaining` by 1, up to a cap stored in `maxNukes` (e.g., 3).
     - If already at the cap, a debug message is printed and the count does not increase.
   - HUD:
     - The `Nukes: X` text updates so the player can see current capacity.


## Enemy Types  

Requirement: **4 distinct enemy types**, differing in health, speed, damage dealt, and spawn probability, with at least 1 having dynamic behavior.  

### Enemy Type A – White Ghost (Common)

- **Behavior:** Basic chaser  
  - Slowly floats directly toward the player using the original chase logic.  
- **Health:** Low (baseline health).  
- **Speed:** Baseline speed.  
- **Damage:** Baseline contact damage.  
- **Spawn Probability:** Most common (default type).

### Enemy Type B – Green Ghost (Slimer)

- **Behavior:** Tough, slightly slower pursuer with an **explosive death effect**.  
- **Health:** Higher health than white ghosts.  
- **Speed:** Slower than baseline.  
- **Damage:** Slightly gentler contact damage than red, but:  
  - **On death**, if the player is within a radius (`GREEN_EXPLOSION_RADIUS`), a Slimer-style explosion is triggered that deals **area damage**.
  - This explosion damage is also affected by the player’s `damageScale` and blocked by Shield.  
- **Spawn Probability:** Uncommon (tank/utility ghost).  
>Expansion ToDo: Add exploding Slimer sound and slimey graphic.  

### Enemy Type C – Blue Ghost (Speedster)

- **Behavior:** Fast chaser that closes distance quickly.  
- **Health:** Low health (easy to kill).  
- **Speed:** Higher than baseline – “speedy ghost.”  
- **Damage:** Baseline contact damage (modified by resistance).  
- **Spawn Probability:** Rare, but more threatening because of speed.

### Enemy Type D – Red Ghost (Meanie)

- **Behavior:** **Evil Red Ghost – High-damage pursuer**
  - Moves toward the player with more threatening stats.
  - Designed to be one of the scariest contacts due to higher damage.
  - Can optionally support “enrage” behavior when close or low on health.
- **Health:** Highest of all ghosts.  
- **Speed:** Starts slower than blue but can be tuned to increase when enraged.  
- **Damage:** Highest contact damage of all ghost types, especially noticeable if the player has not taken Resistance upgrades.  
- **Spawn Probability:** Rare special threat.   

### Damage Differentiation

- Each ghost type uses a different base damage value (white, green, blue, red).  
- Damage is scaled by `damageScale` (Resistance upgrade) and blocked entirely while Shield is active.  
- Green ghosts add an extra layer of danger via Slimer explosions on death.  



## Levels & Obstacles  

### Room Layout  
- The game uses a **single haunted rectangular room** per wave:
  - Room dimensions: `ROOM_WIDTH x ROOM_HEIGHT`.  
  - The room is tiled using `background_sheet2.png` with:
    - Floor tiles in the interior.
    - Wall tiles forming the border (top row, bottom row, left column, right column).
- The camera follows the player within this arena.  

### Obstacles (Gravestones)
- **Collision:**
  - Gravestones **block player movement**:
    - Player movement is applied, then checked against gravestone bounding boxes.
    - If the player intersects any gravestone, their position is snapped back to the last safe location.
  - Bullets **do NOT collide** with gravestones:
    - Projectiles fly over/through gravestones so they can be used as cover.
  - Ghosts currently pass through gravestones (they are incorporeal).
- **Spawn logic:**
  - Gravestones are placed away from the walls so they don’t overlap the wall tiles.
  - They serve as static cover and pathing constraints, forcing the player to navigate the room more carefully.


## Known Issues
- **Visuals:** Proton projectiles may intermittently fail to render at high velocities.  
- **Physics:** Player/Enemy collision with gravestones can occasionally cause "snagging" (sticky movement) due to bounding box overlaps.  


---  

<BR><BR>  

# Credits / Assets  

## Inspiration & Source Material 👻  
- **Inspired by the *Ghostbusters* franchise (Columbia Pictures).**
- **World & Concept:** Based on the original *Ghostbusters* (1984) created by Dan Aykroyd and Harold Ramis.
- **Player Character:** Jillian Holtzmann is based on the character from *Ghostbusters: Answer the Call* (2016) played by Kate McKinnon.  
- **Music Motif:** The background tracks are inspired by the *Ghostbusters* theme song originally written and performed by **Ray Parker Jr.**  
- *Note: This is a non-commercial fan project created for educational purposes as part of a game programming course.*  


## Original Content  
- **Original Background Music:** Matthew Picciano  
`mjpicciano-bhost-guster-jam.ogg` `mjpicciano-ghoul-shooter-ALT-jam.ogg`  
> ⚠️ Original Asset Notice:  
The background music provided by **Matthew Picciano** are proprietary.  


## Other Assets   

### Font  
- Ghostbusters by The Flea Pit `GHOSTBUS.TTF`    
https://www.dafont.com/ghostbusters.font  

### Graphics  

- **Ghosts:**  sanctumpixel  
https://www.gamedevmarket.net/asset/ghost-top-down-pixel-art-character-assets
- **Floor & Wall textures:** Screaming Brain Studios  
https://opengameart.org/content/top-down-dungeon-pack  
- **Graveyard graphics:** gniiial  
https://opengameart.org/content/graveyard-pack-01-02-v-099-0  
- **Crosshair and misc:** Kenney  
https://kenney.nl/assets  
- **Proton Beam parts:** BDragon1727  
https://bdragon1727.itch.io/retro-impact-effect-pack-5  
- **Icons:**  
https://craftpix.net/freebies/free-protective-suit-against-radiation-and-things-pixel-art-icons/  
https://fightswithbears.itch.io/  
- **Background wallpaper:**  
https://free-game-assets.itch.io/parallax-halloween-2d-game-backgrounds  


### Sounds  
- HProps Ghostbusters Sounds!  
http://hprops.com/sounds/  
- Effects  
https://pixabay.com/  

