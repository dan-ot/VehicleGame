# Cav and Arty
This is a game about piloting heavy vehicles in a futuristic battlefield. The core plot is you command a unit of vehicles (via piloting one of the unit at a time, the rest are allied AI) and must complete a variety of missions. Completing a mission
gets you money, salvage (whole gear and parts), and reputation (level or rank). In the initial design, there is only one team (the 'Good Guys') to get rep with.

Mission selection is randomized - this puts the game closer to roguelike territory. In the Core mode, you select some parameters for your desired mission. These selections tune the environment, challenge, and rewards (in the future, I see 
a card/collectible minigame here). You are given objectives when you enter the field and must figure out how to complete them (in the future, I see lots of 'GM Intrusions' to make the mission unexpectedly difficult or require a change of tactics).
At any time, but hopefully after completing or failing all objectives, you can exit the field via the Extraction Zone (which stays marked). In Play mode, you can be more specific about what mission to play and resources aren't tracked outside of
mission play (money, resources, etc). In Hardcore, you have to pay for fuel and ammunition, and if your vehicle is destroyed you have to bail as an individual pilot and either jump into another vehicle or make it to Extraction on foot. When killed
as a pilot, your save is completed and you must start a new game. In Core and HArdcore, you are penalized for not completing primary objectives (penalties are clear before accepting the mission), and you don't receive rewards for 
objectives you don't complete. In Play there are no resources so it's moot. Unlike MechWarrior, salvage doesn't happen automatically. You have to take the time to collect and store the salvage (which might include specialized vehicles on your team),
and it has to make it back to Extraction as well.

You choose your starting vehicle kit - several medium tanks, a variety of small tanks and aircraft and a medium mech, or two small mechs. You're not limited by actual garage vehicle storage in Play mode - you simply pick out a team from among unlocked
vehicles. The root of the gameplay is mech combat, but it should support ground, water, and air vehicles (specifically hover/VTOL/helicopter for playable). There will be a balance/point system in place so these are equivalent; missions will also have
a point system to determine bonus or reduced rewards if you've over- or under-stocked your team.  There is no current plan for pilot management - the emphasis is on driveable vehicles shooting each other.

After each mission, you get a debrief about how much fuel, ammo (if applicable to your game mode), and damage you've sustained; and you'll get a report of the objective results. Finally, you'll get a list of any extra resources you've collected during
the mission. This won't happen every mission. Then you'll go back to your garage - before you can complete another mission, you'll need to pay for repairs, and potentially re-fit or upgrade your vehicles. Depending on the situation (not sure what level
of detail to get into - for first version, just assume full access), you could also purchase gear, chassis, and equipment. The limit is your reputation level (for purchased equipment - salvage is limited only by what you can get away with). Once all this
is resolved, you're back to mission select. In Core mode, you get one vehicle as automatic salvage if you lose all your vehicles in combat; in Hardcore, no such luck. In Play, there are no permanent costs (or resources to begin with) so the point is moot.

In the future, there is room here for a metagame around salvage processing and capacity, or extraction systems - not for the initial version, though. This would also include things like sensor/satellite support, airstrikes, and reinforcements. Also in
the future is more context for missions - planet types, political/military status, mission faction, and the like. Mission results could affect the flow of this, which would in turn change the nature of the missions available. Unlike MechWarrior, there 
are no set dates or events or narrative beats that are required to hit - just play and see what happens to the world.

## Mission Play
The following describe the basics of gameplay - how it is meant to feel, how controls map to that feeling, and some of the balance aspects involved in actual mission play.

### Movement
The game is about vehicle control, so movement is pretty important. You should feel the speed and agility of smaller or wheeled vehicles, versus the stability and ponderous unstoppability of large vehicles. There should be some basic balance system (in
terms of physics) so going up too steep a slope, turning too quickly, or getting hit hard over your center of gravity, should be able to knock you over. Depending on your vehicle's recovery capability (very good for flight and floating, decent for mechs 
who have arms, poor for ground vechicles), you may be forced to abandon your vehicle if it topples. In Play mode, there would be a 'reset' to put you back upright. In other modes, if you have a recovery capability, it would be a standard button remap 
(jump/launch comes to mind).

Vehicles fit into two basic categories - strafing and turning. Many strafing vehicles change into turning vehicles with a 'boost' or 'run' mode. Vehicles will have a turn radius, which will be unique to each mode. Vehicles also have a top speed, which
is also unique to each mode (if they support both). Vehicle physics aren't meant to be racing-accurate or a driving simulator - they are meant to enable the following behaviors:

#### Strafing
Strafing controls map movement directly to the 'forward' movement of the vehicle (forward, in this case, means 'direction throttle is applied', not related to the turret/camera facing or targeting). This means as you push a stick or press a key with a
direction, the vehicle moves in that direction relative to the camera; the final 'forward' indicated is the normal of all inputs (so up and left is diagonal). As the player indicates a different direction (chooses a new control angle), the vehicle's
motivation system (wheels, tracks, legs, rotors) interpolates through the intervening angles until it reaches the target direction. Inputs are taken relative to the camera *at the point they were first received*, so if the player faces West and
indicates rightward movement, they'll go North. They'll continue North as long as they don't change the normal on the controls, even if they rotate and face North or East. Once the movement input normal changes, it's calculated relative to the current
camera again. For controller (or similar 0.0 to 1.0 inputs), there should be a 'grace zone' before we count an actual change in movement. This might apply only in the case where the camera/facing has significantly changed, or maybe the scale of the zone
is related to the scale of facing change.

If the player indicates an unsafe movement (especially if the 'forward' movement is far from the center of the camera), they get a system warning sound. This mostly occurs for hazardous terrain - walking/driving/flying into the water, over a cliff, or
onto a slope that threatens balance. The movement is still allowed (maybe have a slight throttle down in combination with the warning sound so the player has a bit more reaction time). The indicator does not take into account crashes or non-terrain
proximity (it doesn't warn you before you step on a mine or crash through a fuel tank).

#### Turning
Turning controls map more closely to those of a car - forward and back are throttle up, throttle down/brake, and eventually reverse/throttle up. Sideways controls indicate the strength of a turn - the direction is related to forward movement, unrelated
to the camera angle. Turn radius refers to how far to each side the motivators can move - a 90-degree turn radius means the motivators can move directly sideways, spinning on the corresponding rear motivator (or spinning in place). Turning vehicles
(or vehicles in turning mode) generally have a much larger speed range. As the vehicle approaches higher speeds, the turn radius becomes less safe. This interacts with the balance system - depending on the vehicle's balance, there are 'zones' for the
center of gravity. If it's on an interior zone (interior to the vehicle's footprint), the vechicle is stable; if it's in a border zone (a tolerance on either side of the footprint), the vehicle is drifting - moving in a different direction than the
motivators, allowing faster cornering at the risk of stability; if it's outside the border zone, the vehicle is very likely to topple. Many turning vehicles 'lean into' a turn in order to keep the center over the footprint (the footprint may shift
a bit to account for this). If the vehicle takes hits (particularly from the sides) in this process, it may be pushed into a different part of the border zone, including toppling. It might even be pushed from leaning into a fall if it's leaning inward
and takes hits from the outside.

Turning vehicles cannot move to the side if they have no forward speed or momentum. Turning-only vehicles support speeds from 0 to their maximum; Strafing-Turning vehicles, and flying Turning-only vehicles, maintain a minimum turning speed at no
input to maintain their mode.

Relative controls should apply their degree of relativity (a stick 0.5 to the left should apply wheel rotation 50% to the left). Absolute controls should apply over time - a rapid amount of time, but not so fast that the player can't control the
vehicle at higher speeds. This would allow a player to hold the key for a rapid turn at lower speeds, or 'feather' (press-and-release rapidly) for slighter, safer turns at higher speed. We might even want to adjust the rate of turn on the wheels
depending on the vehicle's speed (not the balance - that can be affected by external sources, like taking hits).

I'd also like some adjustment of the system to account for uneven terrain - driving on a slope, or even up onto a wall. That might require an additional 'downforce' modeling that might not be worth it, especially if it doesn't apply to Strafing
movement. It also brings up the question of when the angle of the driving surface changes - when the vehicle is driving on one side of a crest, and crosses to the other without triggering a jump - the vehicle is immediately in 'topple' territory or
must rapidly adjust its speed and turning to stabilize (which I like, but how is that modeled?).

The unsafe movement warning applies to vehicles in the border zone. No adjustment to controls or movement is made in addition; it's mostly a feedback mechanism so the player knows if they're going to topple an unfamiliar vehicle.

#### Vertical
Many vehicles (especially flight) have vertical movement as well. Some have 'jump jets' of some kind - this should apply upward force, but still retain a forward component. When the jets stop applying, the vehicle loses its upward force and begins
falling. This upward force should not be absolute - it should react to gravity and fall speed, so it can be used to boost up into the air or soften a landing. Other vehicles - specifically flight, or submarine vehicles fully submerged - have 'upward'
and 'downward' movement. The animation detail of this movement might be different depending on the Strafing/Turning mode - flying vehicles in Turning mode may change their attitude to point more upward or downward, while flying vehicles in Strafing
mode may merely move up and down (with the assumption that there are subtle adjustments to rotors not visible to the player). Jump jets or other vertical movement is mapped to the bumpers (above the triggers on a controller) or associated keys.

This is not a flight simulator - we may take some efforts to make 'flight' feel like flying, but the physics need to support the listed gameplay, not actual flight physics. Similarly for underwater.

Some vehicles may be equipped with 'bracing' or other crouch/stability modes - these use the opposite bumper from jump jets, or the same bumper as 'downward' movement for vertical vehicles.

### Targeting
All vehicles have some level of 'turret' or targeting system that deviates from 'forward' in movement terms. This is centered on 'forward' movement - that is, the assumption that (0, 0) rotation means the turret or torso is level with the mobility
base or legs and on movement centerline. Every vehicle has 'turret lock,' meaning a limit on how far from this center on each axis it can rotate; attempting to push beyond this limit is a no-op. It also has a rotation speed, which for most vehicles
should be relatively fast. Operating the look controls - mouse or right stick - changes the current turret/torso rotation accordingly. These turret locks and speeds are dependent on installed equipment - they're not hardcoded to the vehicle type. 
Vehicles with a variety of weapon systems on the turret move and lock with the most permissive (or fastest), with a separate reticle coding for any weapons that lag behind. This means battlefield awareness (camera view, specifically) is also limited
by these factors - that's acceptable. This is in effect in 3rd-person view (I'm not sure at this point we want 1st person in the first version).

There are 4 levels of targeting: Undirected, Soft Lock, Hard Lock, and Automated. Some weapons only support a subset of the targeting levels. Some targeting levels have equipment/vehicle setup requirements. Those concerns will be handled in an
Equipment section; this section is about how those results combine to produce gameplay. At this point, we only note that a weapon which doesn't support a targeting level doesn't fire at that level - all requirements have to be met to fire.

#### Undirected
Undirected targeting fires the weapon centered on the reticle. Remember from the previous section that the reticle may be lagging behind camera center, if the turret it's on is slow; it might also be off-center because the most permissive turret can
go further off centerline. Firing the weapon 'centered' is still not a guarantee of accuracy; many weapons have multiple barrels, are subject to bullet drop or other indirect fire, or might just not travel in a straight line once fired; the actual
gameplay guarantee we're making with the reticles is that this is where the barrel/launcher is pointed and it's up to the player to adapt and actually hit a target in real conditions.

Undirected weapons will generally fire if the weapon isn't destoryed and has ammo. It doesn't care much about the situation outside the vehicle.

#### Soft Lock

#### Hard Lock

#### Automated