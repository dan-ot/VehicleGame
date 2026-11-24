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

In the future, I'd like to differentiate what kind of tips are present on any given limbs - hands or claws could provide some ability to climb a vertical surface (while removing targeting on affected mounts).

#### Balance
As noted above (and I'll tease it out into this section as I get time), a vehicle must remain balanced as moving or when still.

For the first version, the point of this is to provide differentiation between vehicle designs; provide limitations on movement beyond marking some terrain as 'impassable' or 'no friction'; and make for some fun combat moments and interesting weapon
designs.

For future versions, I'd like to take into account whether a vehicle uses limbs (for motivation or manipulation), so more complex forms can retain and regain balance more effectively (but might lose targeting/aiming in the process, until balance is
regains).

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

An important case for Undirected fire is melee weapons and improvised weapons (held objects from the environment, given a limb for grabbing).

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

Design-wise, weapons are critical. They should feel different from each other in play; they should present mechanically interesting choices (a ballistics-heavy build will approach the same fight differently from a missile-heavy or beam-heavy one); they should
present aesthetically interesting choices (two ballistic weapons may rate very similarly in terms of 'average damage output' but one might make a resounding bang and the other is more of a thump). I don't want to go full loot-hound crazy like Borderlands or
get into weapon combinatorics like Diablo IV or Path of Exile I/II, but I do want interesting variety. This might be achieved by allowing 'weapon skins' instead of fully-distinct weapons (different tracer/smoke trail colors, different 'fire' and 'impact'
effects, but no numeric/mechanical changes).

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
support the idea of a minimum range on some missile systems - it takes a moment to arm some systems. In addition to self-propulsion, many Missile systems also have guidance on the missiles themselves - this is usually for something simple like 'locked target
when launched' or 'hottest signature detected'. Guidance mostly keeps the given target toward the center of the flight path - it does not avoid obstacles or have any other logics beyond an internal Sensor capability (if the missile is self-guided, not locked)
and 'rotate to face target' direction change. Guidance also does not change once the missile is launched - target loss, target cycle, or the like doesn't affect the missile once it exists independently in game logic. This means a target can utilize movement 
post-launch to guide the missile into an obstacle or another vehicle.

Guidance based on internal Sensors have only a Sensor score (no need to balance Energy, Power, or Fuel in this situation). The missile, once launched, periodically (sub-second, not every frame, will have to tune with gameplay) re-checks its Sensor list and
adjusts to the most-appropriate target - this will be filtered on range (short Sensor range for many missiles), Sensor cone (generally 45 to 90 degrees), and Sensor hit quality (with a minimum filter). From the listed targets, it will generally choose the
strongest hit and pursue that. Ties would be arbitrary, and a lack of Sensor inputs would mean the missile doesn't alter course - but missiles always move forward at their full speed, so it still might 'dumb-fire' hit a target.

Some Guided missiles inherit their target from their launching vehicle - those wouldn't lose that Target (unless it was destroyed). In the future we might complicate this for advanced target-shedding play. These missiles also don't need a Sensor ping - it's
assumed they have all the pertinent data from their launching vehicle (even if the vehicle loses that data - at least for the first version).

##### Autonomous
Some weapons systems have completely autonomous projectiles - not only are they their own in-game object, but they have their own behaviors and properties. These are things like infantry units which might get deployed, or drones that get launched and then
act autonomously. The vehicle only has influence via the Targeting mechanism - beyond that, the 'projectile' makes all its own decisions. For the first version, this will laregly be mission objectives - scrap-collecting drones, victim rescue drones, and the
like. Adding drones to full combat scenarios is an important thing to explore, but it can wait.

##### Other
This setup leaves out interesting new forms of weapon (flamethrowers, top of mind). We should be ready to expand this system, or repurpose it a bit.

### Armor
There are several ways of avoiding an impact - moving from the spot you were in when targeted, maneuvering so guided weapons collide with something before they collide with you, and as-yet-unmentioned Sensor/targeting tricks. However, getting hit isn't the
end of the story - the impact damage(s) apply to any layers of armor you have mounted, outside-in. Each layer sheds, absorbs, or passes some amount of the damage based on type (see Damage under Weapons - i.e., Kinetic, Particulate, Heat). Shed damage has no
effect (design-wise, this should be difficult or rare); absorbed damage subtracts integrity from that armor layer; and passed damage applies to the next layer mounted. Design-wise, we should generally have only one mitigation per layer (or two, for complex 
or expensive armors).

Each kind of armor has a weight, a mitigation chart, an integrity, and a restoration factor; simpler armor needs to be repaired (may take place during Mission Play), while complex armor may be able to use Energy to restore itself over time. Any armor segment
reduced to 0 integrity is gone - damaged beyond repair. Visually, it should fly off the model, and we no longer count its weight or Energy draw.

Armor is represented in the HUD by bars drawn alongside chassis parts. Each layer is a bar, with a color indicating what kind of armor is present; the length of the bar indicates its integrity. The display should collapse if inner layers are destroyed first
(not leave visual holes).

### Chassis
The chassis gives the basic shape of the vehicle (how many legs? Where is the turret?), and expresses its resource limitations. Mechanically, it's divided into sub-parts - each sub-part is its own vehicle location, with its own equipment. The chassis determines
the basic size and shape of the vehicle. Each chassis part has a few factors - Mounts, Space, Frame, and Connection.

Chassis parts report their integrity through a 'paper doll' system (the parts may be visibly separate by a bit). If any specific parts are destroyed (Mobility, Connection, Power, Sensors) there are special indicators (letters or icons) - yellow for disabled, 
red for destroyed. Integrity is represented by a fill amount, with the outline of the chassis part being permanent. This might revise as I fill in the design, but the integrity of the chassis part is the sum of the integrity of all associated systems (some 
systems, like Connection, don't have an integrity number - they are either working or not). Mounts have their own integrity display, they're not included - only Connection and Space systems are indicated here.

The chassis and connected systems (Space, Mounts, Frame/Armor) are the inputs to the balance system (see Movement above). Fully-destroyed Armor, Mounts, and Space-taking subsystems (or limbs/turrets which are part of a Connection chain which have those things) 
also shed their associated weight, changing how the vehicle balances and behaves. Leg-based (I want to say 'hip and shoulder jointed') motivators have a lot of adaptability for rebalancing, while fixed-motivator systems - propellers, jets, wheels, tracks - are
less able to automatically adjust. In any case, the pilot (player or AI) will have to change their piloting style to adapt to new physics and balance limitations.

#### Connection
Each chassis part needs to maintain a connection with two central systems - Pilot and Power (is Compute on this list?). If Pilot connection is lost, the player cannot issue commands to change any mounted equipment's behavior (automated weapons may still fire 
but cannot be turned on or off). If Power connection is lost, no attached equipment or system can utilize power (some, like passive armor, might still function). Each chassis part has a Connection factor (don't need to track 'health' or 'damage' for it - it 
either works or doesn't), and lists the other parts it is connected to in order to reach Pilot and Power. If any chassis part on these chains loses connection, and there isn't another path, everything on the chain also loses connection. There is not a specific 
connection count for chassis parts - that's all down to vehicle design. The general intent is to publish chasses and let the player customize what goes on and in them.

I'm not sure about 'critical hits' or how to handle limb/turret loss - this system seems ideal for that, with some metric on whether connection was broken or the joint fully severed. We'll punt on this for now, since unconnected limbs might still provide armor
support or automated weapons, while lost limbs would not. It seems a lot to have a separate damage system for losing limbs.

In the future, we might carry the idea of Connection to handle weapon ammo and loading.

#### Space
Space refers to the systems inside the chassis. These are things like Compute, Sensors, Storage, and Power; limbs or other mobility-related systems have Mobility. This system takes up an amount of space determined by the chassis (not available for the player to change, 
in this version). When this system is destroyed, the limb or mobility cannot operate. This is general to any kind of movement - not specific to Movement as described in the section above. For example, if the Turret chassis loses Mobility, the player cannot rotate
the turret any more (which means no reticle movment - they'll have to spin the vehicle to aim).

Perhaps limb loss is when Mobility is fully destroyed, while 75% damage means the section no longer moves.

#### Frame
Frame is what holds Armor. At this point, I don't think Frame needs to limit how much armor - we have weight and Power for that; I also don't think we need to track damage for Frame, since Armor has its own tracking. The external Frame slot may also be used to mount
specialized Cargo storage; this only takes up the outermost layer, potentially allowing other layers of Armor underneath.

#### Mount
Mounts are points for equipment that affects the external environment (Weapons, Jets). I'm not sure the level of restriction just yet - I think keeping it maximum generic is ideal. The goal isn't to collect a huge variety of chassis which may or may not be compatible 
with the particular mission setup you want (MechWarrior) - it's to put just enough limits on concurrent equipment that you have to make interesting choices.

Mounts should be sized, though - specifically to prevent overly-ridiculous pairings, like a cannon on a bicycle. Weight and Power also put limitations on vehicle-weapon pairings, but Mounts
can help short-circuit the most egregious violations.

#### Mobility
Mobility Chassis parts allow the vehicle to move. Losing your mobility is not a guaranteed out - but the mission might be impossible, so you may switch to recovery or Bail the pilot. Mobility parts usually come in a couple flavors: Foot/Paw, Wheel/Tread/Hover, Rotary,
and Jets. There are two basic mounts for Mobility - one is on a central part with a lot of Space (tracked vehicle, VTOL), and the other is on the end of a limb with several Connections. This latter allows a lot of adaptability (especially in Balance and recovery) but
presents an enticing target.

In future versions I'd like to take Traction into account, on top of Balance; but we're a combat simulator, not a driving simulator, so we can cheese it for specific Mobility types (Hover and Rotary slide a lot; Wheel slides a little less; Tread and Foot/Paw slide the
least).

##### Foot/Paw
These Mobility types feature a flexible ending of various shapes that can 'wrap around' terrain for an excellent grip. This allows them the ability to climb. However, all this takes time, making them a bit slower, and presents complexity, making them a bit more fragile
and expensive. The main difference between Foot and Paw is shape (Pad forward of the leg for foot, allowing better balance and recovery; or pad surrounding/underneath the leg allowing better grip).

##### Wheel/Tread/Hover
Properly organized, it should be Tread/Wheel/Hover - presenting a sliding scale of grip to adaptability. Tread vehicles cannot strafe but have excellent grip on uneven or slanted surfaces; Wheeled vehicles can slide/drift at high speeds and are generally much faster;
and Hover vehicles can strafe, don't depend on surface properties (they work over liquids as well as over smooth or rough), but are prone to Traction loss.

##### Rotary
This Mobility allows for flight (given a certain weight limit isn't surpassed). We aren't a flight simulator, we're a combat simulator, so we limit player choices to rotary and VTOL-style; we want them to be able to hover, and we want a lower top speed overall -
battlefields which are appropriate for walking across would be much too small for flying over. The exchange for this expanded mobility is a general weakness - Rotary craft are subject to many more forces, and the loss of the ability to fly is frequently devastating.

##### Jets
This is a special case Mobility that generally gets added to an existing Mobility system to enable larger jumps, faster takeoffs, or smoother landings. Jets generally apply in a specific direction or range of directions (again - the controls problem, the lift they
provide is important but if we can get the controls right, a diagonal leap or a sideways scoot are very interesting for avoiding impacts). Unlike other Mobility, they don't tie into the core Propulsion force generation; they are self-contained in that respect. They
May consume the same fuel as Propulsion, may have their own fuel Cargo, or might even be self-charging (with built-in fuel). Their use is generally short-lived - a single burst, which might be repeatable, or a steady burn of 10 seconds or less.

#### Manipulation
Manipulation Chassis parts allow the vehicle to interact more directly with its environment. This usually takes the form of claws or hands. The most direct manipulation is a strike or punch, but both of these manipulators can also be used to pick up/drop, throw,
grapple, and climb. Hands are generally more effective, but much more complex (repesented by a higher cost to install or repair and lower integrity). This can all wait for a later version - there's a lot of control nuance here and I'm not sure I'm ready to tackle that
without practical experience with the demands we've put on the controls with all these other systems.

### Power
Many systems use Power - energy weapons, ballistics with high rates of fire, even more passive systems like Sensors. At its core, Power resolves to two function: Generation and Capacitance. Active Power-draw systems (Sensors, Compute) consume Generation. Any unconsumed
Generation goes to fill up Capacitance. Other systems (most Weapons) draw down Capacitance. If there is not enough Generation to cover the cost of active-draw systems, they shut down until there's enough (we need a predictable algorithm for this - highest-draw first?
Lowest-draw? The player can sort it out after). If there is not enough Capacitance to cover the cost of a system activation, the system does not activate (and no Capacitance is used). This system takes the place of Heat in MechWarrior.

#### Generation
Power generation is the act of turning fuel into Power. Each Generator consumes a specific fuel at a given rate, and produces Power (a single, generic resource) at a specific rate. For first release, Generators are either working or not; as a control subsystem, you can
either turn them all on or all off. Some Generators use ambient conditions - light for solar generation, flammable-gas extraction for some specific planets, thermodynamic for extreme heat environments, and so on (for first version, we might just put an 'Ambient'
generator type and supply the generated based on the environment). It's also possible to have external generators - we can assume an interface on any given vehicle, and portable generators or specialized support vehicles would provide Power on demand. The fuel for
Generators may be the same fuel for Propulsion.

Generators take up Space and provide Weight. They also require Connection to fuel storage. Some generators (fission, for example) have fuel built into the installation; they cannot be refilled during a Mission but also cannot run out of fuel during a mission. The
Generator and Propulsion might use the same fuel (or might both be internally-fueled), or Propulsion might be an Active Power system. Generators may have an Impact block (like a Weapon hit) for when they get destroyed.

When Sensor detect 'power signatures,' they detect Generator usage - the conversion of fuel to Power (among other things), not Power-drawing systems or Capacitance. Ambient generators do not convert fuel internally so also do not show up on 'power signature' Sensors.
External generators show up independently (you would detect the generator but not any connected vehicles).

#### Capacitance
Capacitance is stored Power - it is consumed by Power Draw systems and refilled by Generators (those installed in the vehicle, or externally-connected). We should probably just call it Batteries - sounds less official but most players will understand the system much
more clearly. We don't worry about charge rates or draw rates like real-world batteries - the charge rate is all spare Generator power, and the draw rate is any equipment which needs Active Draw power. Capacitance does not show up on Sensors for 'power signature.'

The main choices to make with Capacitance are balancing Space, weight, capacity, and integrity. Like generators, in the first version capacitance either works or doesn't (we won't deal with degraded due to damage or conditions). Capacitance system may have an Impact block
(like Weapons) for when they get destroyed.

In the future I'd like ambient interaction with capacitance (in particular, bleed at extreme heat or cold; attraction for lightning or plasma strike, etc).

### Propulsion
Propulsion is force applied to motivator systems. In order for throttle (from Movement) to turn into motion, it gets processed through Propulsion. This process requires some combination of Fuel and Power (specific mix, which might be 0 on either side, determined by the
Propulsion system in question). The specific chain is thus: throttle determines what percentage of motion the Propulsion produces. The Propulsion consumes fuel appropriate to that throttle. Then the Propulsion generates an amount of force. The force has to overcome
environmental effects (water resistance, strong winds, other forms of restraint). After all that, we get the actual forward speed.

### Sensors
Sensors are critical for success on the battlefield. They feed non-visual info and power all Targeting beyond Direct targeting. Sensors take up Power and Space; they are frequently grouped together in a single chassis part but that's not a requirement. In game engine terms,
the core camera/render flow is not a Sensor (we don't narratively address what it is). The influence the player feels when dealing with Sensors mostly appears in the HUD, and in operation of the Targeting system. To call out specifically, a mission-requirement scanner which
is used to search a building for a MacGuffin is not a Sensor (it would probably be a Weapon, by engine terms, since you'd target and trigger it manually). The specific thing Sensors do as a subsystem is feed information about the environment to other systems.

Each Sensor system collects a specific kind of data, over a specific set of ranges. All of this is added together to produce a 'Sensor' data kit of contacts and relative locations, which is then consumed by other systems (Targeting and HUD). This flow is important - there
are (planned, at least) weapons and systems that spoil this data at different points. Sensors work (mechanically, without having actually coded it yet) through ray-casting to detectable targets; the ray strength starts at a value set by the Sensor. Every intervening region
which affects the ray (solid objects for radar, fog for lasers, trees for visual, etc) subtracts from the sensor strength. If the final rating is below the Sensor's detection rating, the target is not part of sensor data. There is probably a heuristic way to do that.
Sensors only run this test for objects with their kind of data (thus, a heat sensor does not run this test against things that don't generate any heat) - this means objects have to mark themselves for what kind of Sensor data they generate.

Sensors are Active or Passive - when operating equipment, we might split these two. Active sensors must add energy to their environment to get data back - radar and sonar are good examples. In this process, they create strong signals which can be picked up by other Sensors.

### Compute
This may be an unneccessary resource - it's on the chopping block. The purpose of including it is to force specialization around high-end systems - smart weapons, Automated Targeting, drone clouds, and the like. Including one of these may work on a normal amount of Compute,
but including two or three starts to add up the extra Compute space required. As just mentioned, Compute would use up Space and Power. But, having written all the other systems out (which would be used directly, while Compute is only a limitation), it might make sense to
just increase the Space and Power requirements of the other systems. Perhaps Compute just acts like Ammo - it must connect to the consumer via a healthy Connection (see Chassis), letting you put Sensors in one limb and Compute in the body (or vice versa), and letting you
destroy Compute without destroying the Sensors (but disabling them, because they needed the Compute).

### Cargo
There are various game systems (a superset of vehicle systems) which require you to carry or store things. Cargo is reserved Space which starts with a minimal weight and gets heavier depending on what's being stored. Each kind of Cargo storage holds a specific set of things
- for instance, a liquid container for stable liquids, or an energy cell for collected plasma, or safety-aware quarters for VIPs you're transporting. Probably the most common are Ammo storage for Weapons systems, Fuel storage for Power and Propulsion systems, and scrap
storage for specialized scrap-collecting vehicles (remember from Mission - you have to extract any scrap you want to keep).

There are a couple main things Cargo does. First, it reserves Space; then it converts that Space into its own unit of measurment. For example, it might reserve 3 slots, and be able to store 100 gallons of liquid in each slot; or reserve 3 slots, and store 1 VIP per slot.
This gives it a 'fullness' measure, from 0% to 100%. The material itself will have a 'weight per unit' which can be used for weight calculations (balance, speed resistance). Maybe in the future we get fancy for 'shiftable' - liquid or semi-solid - to provide entertaining
balance challenges, but for now it's the same regardless of material. A specialized kind of Cargo is handled by Frame instead of Space (I need to annotate this under Frame as well); it still has similar properties as other Cargo, like weight distribution and integrity,
but it can store material which doesn't actually fit inside a Chassis Space (like a small cargo mech carrying broken limbs and Chassis parts off the battlefield).

Second, it protects the contents; it has its own 'armor'/Impact-handling, and its own integrity. Like other systems, at low integrity it is broken and may leak material (this could be critical depending on the material - spraying jet fuel or plasma all over yourself can
have big consequences in a fight); at 0 integrity it is destroyed, the weight is all gone, and so is the material (also critical for many missions - goodbye, VIPs). For missions requiring collection or delivery of a material contained in Cargo, there will usually be levels
of sucess depending on how much actually makes it out. Specifically for broken Cargo containers - the side effects of the break, and how it reacts to further damage, all depend on the matrial being stored (not sure if I will get that far in this doc, but it doesn't belong
here in any case).

Cargo would be collected and deployed by a Weapons-compliant system - this represents the loading or unloading process. For some missions (delivery especially), you may merely be measuring how much you extract with - in which case you can't collect or deploy the stored
Cargo. Collection and Deployment come into play when considering scrap/resource collection as a side mission, or the environmental effects of deploying certain kinds of cargo (instant smokescreen? flammable patch? and so on). Collection and Deployment also become more
interesting when we get hands/grabbing/carrying and environmental tool usage - you might carry a shipping container to throw or drop it, then crack it open to make a hazard area. We'll skip that for the first version.