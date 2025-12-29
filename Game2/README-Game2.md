# Spooky Fetch  

Spooky Fetch is a two-player arcade sports game inspired by Pong, where a zombie and a ghost dog play fetch with the zombie’s own head. Each player has unique abilities and scoring conditions, and the match is set in a creepy graveyard-style court with spooky visuals and sounds.

---
   
<br>

## Gameplay Loop
1. Zombie tosses their head into play.  
2. Players move their bats (zombie or dog) to intercept the head.  
3. The zombie scores when the head passes the dog’s side.  
4. The dog scores when returning the head by hitting it.  
5. Special abilities can be activated to gain an edge.  
6. Loop repeats until one player wins.  

## Goal / Objective
Be the first player to reach the target score.  
- Zombie wins by successfully getting the head past the dog.  
- Dog wins by returning the head enough times to rack up points. 

## Player Actions & Abilities
- **Zombie (Player 1)**  
  - Moves with **W / S** keys (up and down).  
  - Scores when the ball (zombie head) passes the right edge.  
  - Ability: **🩸 Head Spin** — ball turns red and speeds up for 3 seconds.  

- **Dog (Player 2)**  
  - Moves with **Arrow Up / Arrow Down** keys.  
  - Scores by hitting the ball back (each successful return = point).  
  - Can catch and throw the ball by pressing **Spacebar**.
  - Ability: **👻 Ghost Buddy** — a semi-transparent clone appears and mirrors vertical movement for 3 seconds.  

## Obstacles / Boundaries
- Top and bottom screen edges act as boundaries: the ball bounces off.  
- Left and right edges act as goals: scoring conditions trigger.  
- Players are restricted to vertical movement on their side.  

## Scoring & Game Over
- **Zombie**: gains a point when the head passes the right edge.  
- **Dog**: gains a point whenever the head is caught/hit/returned.  
- **Game Over**: first to reach the score goal wins.  
- Centered winner text appears (“Zombie Wins!” or “Dog Wins!”).  

## Visual & Audio Feedback
- **Visuals**:  
  - Zombie, dog, and ball use custom sprites.  
  - HUD with translucent panel shows current scores.  
  - Head changes color when Zombie ability is active.  
  - Ghost Dog appears semi-transparent when summoned.  
- **Audio**:  
  - Background music loops during gameplay.  
  - Dog bark plays when Dog scores.  
  - Zombie groan plays when Zombie scores.  

---

<br><br>

# Credits / Assets  

### Original Content  
- **Zombie Sprites**: Matthew Picciano   
`ball.png` `zombie.png` `zombie-headless.png`  
 
- **Background Music**: Matthew Picciano  
`matt-picciano-game2-music.ogg`  
  
> ⚠️ Original Asset Notice:  
The background music and specific original sprites provided by **Matthew Picciano** are proprietary. 

### Font  
- **Font**: `DeadFontWalking.otf` by Chequered Ink  
https://www.1001fonts.com/dead-font-walking-font.html  

### Graphics
- **Dog Sprite**: `dog.png` “[LPC] Cats and Dogs” by bluecarrot16  
https://opengameart.org/content/lpc-cats-and-dogs   
- **Spooky Grass and Deco**: `spookytileset.png` Bee — itch.io  
https://beeplz.itch.io/halloween-tiles-and-walking-characters  

### Sounds  
- **Dog Bark Sound**: `dog-bark.wav` DRAGON-STUDIO — Pixabay  
  - https://pixabay.com/users/dragon-studio-38165424/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=382732
- **Zombie Sound**: `monster-snarl.wav` freesound_community — Pixabay  
  - https://pixabay.com/users/freesound_community-46691455/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=105348  


