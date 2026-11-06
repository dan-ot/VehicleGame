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
Soft Lock weapons have a 'target radius' based around the weapon's reticle (again, the reticle may lag or not reach camera center). The weapon has a target type preference list - this might be vehicle or object properties like "explosive" or "flying"
or it might be specific sub-parts like "weapon", "pilot", or "motivator". The Soft Lock weapon picks the highest-priority target (or targets, if the weapon supports multiple targets like a multi-missile launcher) and automatically locks on them while
they are in range of the reticle. This might apply outside the range you'd expect from an Undirected weapon - meaning a Soft Lock weapon on a restrictive turret might still be able to target a wide radius, including outside visible camera range. A
weapon which requires lock will fire with a soft lock. The player cannot control what is locked with a soft lock (other than clever manipulation of the camera).

The Soft Lock priority list is filtered by weapon limitations - i.e., if a weapon has a minimum or maximum range, it will not consider targets outside that range for locking. Within those limitations, it prefers closer targets (first by visual distance
to the reticle, then by physical distance from the turret). Buliding the target list depends on two vehicle subsystems - some level of Compute resource is required, and the more you have the smarter the lock gets; and some level of Sensor is required.
More sensor mostly allows a wider target list or helps to target in adverse conditions or specific sensory defenses. All Soft Lock weapons have these requirements; if your vehicle doesn't have a minimum amount of each subsystem available, the weapon
does not fire (unless it also supports Undirected fire, in which case it uses those firing rules). The effect of Compute and Sensor ratings on the weapons is definite per weapon - for example, it may allow a weapon to be Soft Lock instead of Undirected,
and then past another minimum Compute, allow the weapon to Soft Lock on multiple targets.

Soft Locking is per weapon - thus, a vehicle with 3 soft lock weapons may have 3 (or more) soft-locked targets. The HUD should indicate a soft-locked target (I'm thinking a blue outline) with an icon or numeric indicator as to which weapon has the lock
(these icons/indicators are part of the weapon list and are pickable during vehicle setup, so the pairing is predictable and in the player's control). Offscreen locks have a directional arrow of the appropriate color and style and with the icon/indicator
to let the player know the weapon has a soft lock, even if they can't see the target.

Soft Locking has a quiet chime/click/beep on the system sound channel. This plays any time a Soft Lock target is acquired - whether a new target from having none, or a change in target (even a change in sub-target on the same vehicle, like switching from
a limb to a leg on a mech).

#### Hard Lock
Hard Lock weapons require a choice of target based on player input. They are limited in similar ways to Soft Lock weapons - they don't fire without a target, they require Compute and Sensors to obtain the lock. However, they are not reticle-limited when
picking targets. Hitting the Target Cycle button (D-pad left, Left Arrow) scrolls through available targets which Sensors can currently detect; hitting the Target Select button initiates the Hard Lock. If the currently-locked target goes out of Sensor
detection (behind a mountain, under the water, far away), the Hard Lock is broken and the next available target is selected; if the same happens to the currently-selected target, the next selection is made. If no target is available there is no selection
and no Hard Lock possible. Depending on available Compute, Sensors, and the weapon itself, the pilot may have to hold a selection for a time to obtain a Hard Lock. For example, for every Compute available to the Hard Lock system, multiply it by the Sensor
quality of the selection (faster for clear sensors, slower for obscured) and again by per-frame timing to achieve lock-per-second; accumulate this until the total is 10 for some weapons, and 3 for others. These Compute demands are only made when the player
hits the Target Select, and are released when Target Select is used again (abandoning the Hard Lock) or the Hard Lock is otherwise broken (target destroyed, sensors foiled).

Selected but not locked targets have one color of outline or off-screen indicator (yellow or orange comes to mind), and Hard Locked targets have another (red comes to mind). Targets are notified of an acquired Hard Lock (there's a 'Weapon Lock' alert on
on the system sound channel for players; enemy AI receives a message which it can use to modify behavior). Each step in the process (Select, Locking, Hard Locked, Hard Lock Lost) has a unique system sound - small, short, but different from Soft Lock - so a
player can subconsciously track the progress.

There is one Hard Lock per vehicle, regardless of the number of systems which support or require Hard Lock. All locks acquire at the rate of the slowest/most difficult lock, but also all locks consume the same set of Compute and Sensor (meaning, adding less
demanding weapon systems doesn't complicate the Hard Lock - it stays at the most difficult to acquire). If multiple systems support Hard Lock, the entire kit of icons/indicators appear with the Hard Lock indicator; weapons still trigger with their
appropriate groups (discussed later). Hard Lock is camera/reticle-independent; instead, it is directly related to Sensors. 

#### Automated
Automated weapons consume larger amounts of Compute (and rely on Sensors) but require no input from the player. They scan available Sensor data and select targets based on internal metrics (generally, physically closest for offensive weapons, highest-threat
for defensive ones). When any target is detected, they sound a system ping; depending on the weapon in question, they may wait for a trigger or they may fire automatically. They present a specific color (white?) for their selection and icon/indicators.

### Weapons
The category of equipment labeled Weapons is grouped mainly due to a combination of UX factors:
	- Weapons affect the environment outside of the vehicle
	- Weapons draw on resources internal to the vehicle for that impact (ammo or energy). These resources can be exhausted, though they may also recharge
	- Weapons use one of the Targeting modes to determine which part of the environment they'll impact
	- Weapons have a range outside of which they are not effective

#### Impact
The core point of having weapons systems is to affect the environment. 

##### Damage
The primary effect is to cause damage. Damage-dealing weapons deal damage in some combination of three categories: Kinetic, Particulate, and Heat (or explosive). Objects in the game world (terrain, vehicles, buildings, people) react to these types of damage
in different ways. Damage for a weapon system is expressed in the buildout phase as 'Damage Per Second'; however, each weapon has individual blasts which must either hit or miss (for example, a bullet from an automatic cannon, a laser from a blaster, or a
single missile from a swarm missile launch). When a weapon is fired, it deals this damage to the first object it contacts, applying all damages. Weapons potentially also have a secondary or splash effect on nearby targets. Splash effects do not apply to the
same object the impact damage (in this sense, different locations on a mech or large vehicle are different objects). Splash effects can apply to an internal system if the impact removed all the armor or defenses from that same object. Damage dealt to internal
systems is randomly spread among those systems (more detail under Systems).

##### Collection
Collection systems draw external resources into the vehicle. They generally include, as part of their size requirements (more under Chassis), some amount of storage. Taking resources into a vehicle can change its weight and balance profile (see Propulsion and
Movement, respectively). Once drawn into the vehicle, the collected resources may react to future damage (such as filling an internal tank with explosive gases, which are then triggered by a bullet strike). 

Collection mission objectives are successful only if the material makes it to Extraction, so even if it's non-reactive (VIPs or refugees, for example), no credit is granted unless they make it to Extraction intact. Collection is also critical for salvage - if
the player wants any materials from the battlefield to use in future missions, they must use a Collection-equipped vehicle to take those resources off the field via Extraction. Delivery missions may start with a Collection systems pre-loaded.

#### Resources
Weapons use two basic kinds of resources - first of all, they require ammo or fuel to fire. Storage for this is a spearate system (see Systems); multiple weapons which use the same ammo or fuel type can draw from the same storage (regardless of actual storage
location - i.e., ammo stored in the legs can be used by weapons mounted on the head and arms). Weapons have a secondary storage (or magazine) of this resource which is independent of the main storage - if the main storage is damaged this magazine can still
operate the weapon. The process of replenishing the magazine is 'reload rate' - it can be expressed as an average, but it's either a rate per second (frame) or a bundle of ammo all at once, every number of seconds (frames). Regardless if the ammo is liquid and
infintesimally divisible, it is only tracked in whole units. Additionally, the ammo storage is expressed in the smallest unit available, but magazines are expressed in firing units for the weapon they support (consider a semi-auto launcher and a swarm launcher
drawn from the same missile storage. The magazine for the single-shot launcher might hold 8 missiles, which reports as 8 shots ready to fire; while the magazine for the swarm missile launcher holds 8 missiles, which is 1 swarm.

The second resource might be Energy (more on that in Power) - a weapon must draw from the vehicle's shared pool of Energy to activate. This draw might be very heavy for pure particle weapons, moderate for weapons with pumps or spinning barrels, and very slight
for weapons with onboard power sources. The draw takes place during firing, not during loading or while idle.

For Collection-type weapons, the system might be thought to work in reverse - the magazine is loaded and then moved into Storage. The general mechanics work the same way.

#### Range
Each weapon system has a specific range within which it is effective. The minimum range is optional, and assumed to be '0' or 'adjacent', though there are some weapons that are ineffective at close range (some missiles or drones might need time to arm, for
example, or some weapons might be too explosive to safely use close-up). The weapon's range is accounted for in two places - first, a weapon which impacts an object outside of its range has no effect (mechanically - there might still be some level of visual
feedback). Second, it's passed on to the Targeting system the weapon is using. If the weapon is out of range it does not fire. An out-of-range weapon's icon/indicator is rendered with lower Value (darker) to show it cannot fire, and attempts to trigger
firing (for player input weapons only) make a system channel sound - a short alert of different pitches for under- or over-range.

#### Types
While the systems above are very flexible, meant to express a huge variety of weapons, the first version will be constrained. There will be a selection of three or four each from the following categories, and we'll grow from there. There's not meant to be an
exhaustive cross-product of weapon types, or filled categories (like MechWarrior). I'd prefer interesting weapons systems with unique choices - I'd rather the weapons had personality than they made complete sense, or mirrored real-world weapon development.

That said, there are four Types of effect a weapon might have: Ballistic, Beam, Missile, and Autonomous. They do no map to the types of damage - each weapon will have an impact damage and a splash damage, or the types of ammo.

##### Ballistic
Ballistic weapons fire projectiles with an arc trajectory - they rise a bit, then fall. The projectile is either a bullet (which means, in game terms, too small or too fast to be rendered - just trace the trajectory to calculate a hit and maybe have some
'tracer' projectiles to let the player see the path) or a shell (slow enough or large enough to need to render - check impact point based on collision. We may need a high-speed collider system, or a midpoint system where the visual is rendered but we still
do a 'bent ray' cast to determine impact.

These trajectories are usually either flattish (like a bullet) or a high arc (like artillery). Especially for arcs, 'minimum' and 'maximum' ranges refer to impact distance from the vehicle, not travel distance of the projectile. This means we'll need a
specific division in behavior for these two - there are no weapons that will use both. This trajectory is the defining characteristic of a Ballistic weapon. For shorter-range ballistics, where the flattened trajectory means the 'maximum range' is
basically a straight line, I'd like the 'tracer' effect to only show within range (its purpose is to assist the player in shooting); but the full trajectory should be tracked, whether that's a visible shell or a traced bullet, so the impact effects can
still be rendered (sparks from bullets hitting metal, for example).

Future versions may support external ballistic effects - wind, viscosity, air-to-water transfer, planet-specific gravity, and the like - but the first version should stop at 'bent ray tracing' or a specific arc path for a game object to travel.

##### Beam
Beam weapons transmit damage from weapon to target instantly. They use a simple ray trace to detect impact, and include either an endpoint marker (like a red dot for a 'painted target' system) or a particle effect tracing the whole path. Even with an
invisible (non-rendered) beam, we should have some way of marking the attacker for the defender specifically - maybe you get a 'lens flash' from the attacker only if they are successful in their targeting. Beam weapons may go through 'stages' of fire - an
early acquisition phase, a 'building damage' phase, and a 'critical damage' phase on a heat ray, for example. The particle effect should denote which phase is present, and each phase may have its own impact characteristics (this is part of the weapon
definition, not a requirement for all beam weapons).

Future versions may support 'intervening media' properties - refraction as a beam passes through liquids, diffusion as it passes through vapors - but this will require not only advanced modeling on the part of the ray cast but regional modeling of these 
effects. There may be an intermediate step where these regions are specifically defined ('this smoke cloud scatters beams 30%').

##### Missile
Missile weapons, despite the academic meaning of the phrase being very similar to Ballistic, are those which are self-powered and potentially self-guided as well. Their defining characteristics in game terms are that they travel fast but not instantaneously
- depending on range, it would be possible to avoid their impact entirely, or react to their approach. They are modeled specifically as collidable objects, and they usually have two impact definitions - one for 'disarmed' and another for 'armed.' This is to
support the idea of a minimum range on some missile systems - it takes a moment to arm some systems.