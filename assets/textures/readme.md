Place texture image files here (mario.png, goomba.png, etc.).
Draws fallback debug rectangles if missing.
Assets for game:
https://www.spriters-resource.com/snes/smassmb1/asset/83422/

# Positions for players, mobs and world

## Player
- Fill color in rectangle box: (224,104,136)
### Mario
- Idle: (2,24) -> (17,39)
- Walk: (26,24) -> (41,39) ; (43,24) -> (58,39)
- Jump: (109,24) -> (124,39)
- Pipe: (71,53) -> (56,68)
- Crouch : (19,53) -> (34,68)

Note:
there's no official crouch in the sheet so I'm using from climb

### Luigi

- Idle: (215,24) -> (230,39)
- Walk: (239,24) -> (254,39) ; (256,24) -> (271,39)
- Jump: (322,24) -> (337,39)
- Crouch: (235,53)-> (247,68)

## World
- Fill color: (255,255)

- Bush 1: (182,193) -> (248,208)
- Bush 2: (1431,190) -> (1482,208)

- Flying Mystery Box: (256,144)->(271,159)

- Flying Brick: (320,144)->(335,159)

- Normal brick: (2192,144) -> (2207,159)

- Ground floor: (0,208) -> (1103,239)
- No grass ground: (0,221) -> (1103,239)

### Note: 
- Normal bricks can be stacked to create blocks that lie on the platform
- if ground floor is not tall enough for the world, stack the "no grass ground"

## Monster
- Fill color: (0,64,64)

### Koopa 
- Move:(52,37) -> (67,60) ; (69,37)->(84,60)
- Shell
