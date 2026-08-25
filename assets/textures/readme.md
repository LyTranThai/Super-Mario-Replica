# Asset and Position Reference

## 1. Assets

Place texture image files here (`mario.png`, `goomba.png`, etc.).

Draws fallback debug rectangles if missing.

### Assets for game
https://www.spriters-resource.com/snes/smassmb1/asset/83422/

---

# 2. Positions for Players, Mobs, and World

## 2.1 Player

**Fill color in rectangle box:** `(224,104,136)`

### Mario

| Animation | Position |
|---|---|
| Idle | `(2,24) -> (17,39)` |
| Walk | `(26,24) -> (41,39)` ; `(43,24) -> (58,39)` |
| Jump | `(109,24) -> (124,39)` |
| Pipe | `(71,53) -> (56,68)` |
| Crouch | `(19,53) -> (34,68)` |

**Note:**  
There's no official crouch in the sheet so I'm using from climb.

### Mario Power State
Fill color: (255,255,255)

| Animation | Position |
|---|---|
| Idle | `(2,88) -> (33,119)` |
| Walk | `(76,88) -> (107,119)` ; `(109,88) -> (140,119)` |
| Jump | `(2,132) -> (33,163)` |
| Pipe | `(2,176) -> (33,207)` |
| Crouch | `(33,88) -> (70,119)` |


### Luigi

| Animation | Position |
|---|---|
| Idle | `(215,24) -> (230,39)` |
| Walk | `(239,24) -> (254,39)` ; `(256,24) -> (271,39)` |
| Jump | `(322,24) -> (337,39)` |
| Crouch | `(235,53) -> (247,68)` |

---

## 2.2 World

**Fill color:** `(255,255,255)`

| Object | Position | Map Code (.txt) |
|---|---|---|
| Bush 1 | `(182,193) -> (248,208)` | N/A |
| Bush 2 | `(1431,190) -> (1482,208)` | N/A |
| Flying Mystery Box | `(256,144) -> (271,159)` | `?` `M` `F` `S` |
| Flying Brick | `(320,144) -> (335,159)` | `B` |
| Normal brick | `(2192,144) -> (2207,159)` | `X` |
| Ground floor | `(0,208) -> (1103,239)` | `#` |
| No grass ground | `(0,221) -> (1103,239)` | `#` |
| Short Pipe | `(448,175) -> (479,208)`| `I` |
| Medium Pipe | `(608,160) -> (639,208)`| `J` |
| Long Pipe | `(736,144) -> (767,208)`| `K` |


### Notes

- If ground floor is not tall enough for the world, stack the "no grass ground".

---

## 2.3 Monster

**Fill color:** `(0,64,64)`

### Koopa (Map Code: `C`)

- **Move:** `(52,37) -> (67,60)` ; `(69,37) -> (84,60)`
- **Shell**

### Goomba (Map Code: `G`)

- **Move:** `(1,45) -> (16,60)` ; `(18,45) -> (33,60)`
- **Crushed:** `(35,45) -> (50,60)`

### Falling Piranha (Map Code: `N`)

- **Open Mouth:** `(239,37) -> (254,60)`
- **Closed mouth:** `(256,37) -> (271,60)`

### Falling block (Map Code: `T`)

- **Open:** `(273,37) -> (288,60)`
- **Falling:** `(290,37) -> (305,60)`

---

## 2.4 Items (Rewards from Mystery Blocks)

**Texture file loaded:** `coinblockreward.png` (used for all item rewards)

**Fill:** `(256,256,256)`

**Note:**  
These items appear after Mario hits the flying mystery block.

### Coin (Loaded from coinblockreward.png)

- **Position:** `(243,66) -> (252,80)`

### Mushroom / Little white monster (Loaded from coinblockreward.png)

- **Position:** `(0,176) -> (15,191)` ; `(16,176) -> (31,191)`

---

# 3. Background

- **Load from:** `background.png`
- **Full box:** `(18,18) -> (3089,256)`
- **Note:** Shrink size to fit world if needed.
- **Fill lower color:** `(192,248,248)`

---

# 4. Texture Background

**Fill color:** `(255,255,255)`

| Element | Position |
|---|---|
| Hill | `(48,160) -> (168,254)` |
| Mountain | `(12,139) -> (224,231)` |
| Cloud (no background) | `(388,15) -> (496,92)` ; `(830,16) -> (913,82)` |

### Note

- Hill and mountain can be reused to create multiple versons, thereby enhancing the background.