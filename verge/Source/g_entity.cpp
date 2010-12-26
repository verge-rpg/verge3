/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


/******************************************************************
 * verge3: g_entity.cpp                                           *
 * copyright (c) 2002 vecna                                       *
 ******************************************************************/

#include "xerxes.h"

/****************************** data ******************************/

/****************************** code ******************************/

Entity::Entity(int x1, int y1, const char *chrfn)
{
	follower = 0;
	follow = 0;
	delay = 0;
	lucent = 0;
	wdelay = 75;
	setxy(x1, y1);
	setspeed(100);
	speedct = 0;
	chr = RequestCHR(chrfn);
	hotw = chr->hw;
	hoth = chr->hh;
	visible = true;
	active = true;
	specframe = 0;
	movecode = 0;
	moveofs = 0;
	framect = 0;
	frame = 0;
	face = SOUTH;
	hookrender = "";
	script = "";
	description = "";
	memset(movestr, 0, 256);
	obstructable = 0;
	obstruction = 0;
	for (int i=0; i<FOLLOWDISTANCE; i++)
		pathx[i] = x,
		pathy[i] = y,
		pathf[i] = SOUTH;
}

Entity::~Entity() { } // dont delete chr anymore due to chr cacher/garbage collection
int Entity::getx() { return x / 65536; }
int Entity::gety() { return y / 65536; }

void Entity::setface(int d) { 
  // bounds-check this since it's easy to set an invalid value in vc (janus)
  if ((d > 0) && (d <= 4)) {
    face = d; 
  } else {
    err("Entity::setface(%d), facing values must be within the range [1, 4]", d);
  }
}

void Entity::setxy(int x1, int y1)
{
	x = x1 * 65536;
	y = y1 * 65536;
	if (follower) follower->setxy(x1, y1);
	set_waypoint(x1, y1);
	for (int i=0; i<FOLLOWDISTANCE; i++)
		pathx[i] = x,
		pathy[i] = y,
		pathf[i] = SOUTH;
}

int Entity::getspeed() { return speed; }
void Entity::setspeed(int s)
{
	speed = s;
    // We don't reset the speedct here, because
    // 1) Is is keeping track of distance already moved but not acted on
    //    (ie any partial movement made but not turned into a tick)
    // 2) If we reset speedct, setting the speed frequently will slow
    //    the character down by discarding the partial bits

	if (follower) follower->setspeed(s);
}

int Entity::get_waypointx() { return waypointx; }
int Entity::get_waypointy() { return waypointy; }

void Entity::set_waypoint(int x1, int y1)
{
	waypointx = x1;
	waypointy = y1;

	switch (sgn(y1-gety()))
	{
		case -1: face = NORTH; break;
		case 0:  break;
		case 1:  face = SOUTH; break;
	}
	switch (sgn(x1-getx()))
	{
		case -1: face = WEST; break;
		case 0:  break;
		case 1:  face = EAST; break;
	}
}

void Entity::set_waypoint_relative(int x1, int y1, bool changeface)
{
	waypointx += x1;
	waypointy += y1;

	if(changeface) {
		switch (sgn(y1))
		{
			case -1: face = NORTH; break;
			case 0:  break;
			case 1:  face = SOUTH; break;
		}
		switch (sgn(x1))
		{
			case -1: face = WEST; break;
			case 0:  break;
			case 1:  face = EAST; break;
		}
	}
}

bool Entity::ready() { 
	
	return (x/65536 == waypointx && y/65536 == waypointy); 
}

bool Entity::leaderidle(){

	if (follow) return follow->leaderidle();
	return (x/65536 == waypointx && y/65536 == waypointy);
}

// called to sync up with leader's frame
// of course, if the two people have different-
// length walk cycles, they might have the same framect,
// but they won't sync visuall, which is OK
int Entity::get_leader_framect()
{
    if(follow) return follow->get_leader_framect();
    return framect;
}

void Entity::set_framect_follow(int f)
{
    if(follower) {
        follower->set_framect_follow(f);
    }
    framect = f;
}

void Entity::stalk(Entity *e)
{
	follow = e;
	e->follower = this;
	for (int i=0; i<FOLLOWDISTANCE; i++)
		pathx[i] = follow->pathx[FOLLOWDISTANCE-1],
		pathy[i] = follow->pathy[FOLLOWDISTANCE-1],
		pathf[i] = SOUTH;
	x = follow->pathx[FOLLOWDISTANCE-1];
	y = follow->pathy[FOLLOWDISTANCE-1];
	set_waypoint(x/65536, y/65536);
    movecode = 0;
	obstruction = false;
	obstructable = false;
    // clear delay info from wandering
    delay = 0;
    // sync our (and followers') framect with the leader
    set_framect_follow(get_leader_framect());
}

// This is called when we are going to change
// to a kind of movement that isn't stalking to
// ensure we are not trying to stalk at the same time
void Entity::clear_stalk()
{
    if(follow) {
        follow->follower = 0;
        follow = 0;
    }
}

void Entity::move_tick()
{
	int dx = waypointx - getx();
	int dy = waypointy - gety();

	if (this != myself && !follow && obstructable)
	{
		// do obstruction checking */

		switch (face)
		{
			case NORTH: if (ObstructDirTick(NORTH)) return; break;
			case SOUTH: if (ObstructDirTick(SOUTH)) return; break;
			case WEST: if (ObstructDirTick(WEST)) return; break;
			case EAST: if (ObstructDirTick(EAST)) return; break;
			default: err("Entity::move_tick() - bad face value!!");
		}
	}
	framect++;

	// update pathxy for following
	for (int i=FOLLOWDISTANCE-2; i>=0; i--)
		pathx[i+1] = pathx[i],
		pathy[i+1] = pathy[i],
		pathf[i+1] = pathf[i];
	pathx[0] = x;
	pathy[0] = y;
	pathf[0] = face;

	// if following, grab new position from leader
    // We now keep track of our own framect, (rather
    // than using the leader's framect)
    // which is synced with the leader in stalk(),
    // but then runs free after that so animations
    // of different lengths are ok in a stalking chain.
	if (follow)
	{
		x = follow->pathx[FOLLOWDISTANCE-1];
		y = follow->pathy[FOLLOWDISTANCE-1];
		face = follow->pathf[FOLLOWDISTANCE-1];
		set_waypoint(x/65536, y/65536);
		if (follower)
			follower->move_tick();
		return;
	}

	// else move
	if (dx)
		x += sgn(dx) * 65536;

	if (dy)
		y += sgn(dy) * 65536;

/*
	if (dx &&!dy)
		x += sgn(dx) * 65536;
	if (!dx && dy)
		y += sgn(dy) * 65536;

	if (dx && dy)
	{
		x += sgn(dx) * 46340;
		y += sgn(dy) * 46340;
	}
*/
	if (follower)
		follower->move_tick();
}

void Entity::think()
{
	int num_ticks;
	if (!active) return;

	if (delay>systemtime)
	{
		framect = 0;
		return;
	}

	speedct += speed;
	num_ticks = speedct / 100;
	speedct %= 100;

	while (num_ticks)
	{
		num_ticks--;

		if (ready())
		{
			switch (movecode)
			{
				case 0: if (this == myself && !invc) ProcessControls(); break;
				case 1: do_wanderzone(); break;
				case 2: do_wanderbox(); break;
				case 3: do_movescript(); break;
				default: err("Entity::think(), unknown movecode value");
			}
		}
		if (!ready())
			move_tick();
	}
}

bool Entity::ObstructDirTick(int d)
{
	__grue_actor_index = this->index;

	int x, y;
	int ex = getx();
	int ey = gety();

	if (!obstructable) return false;
	switch (d)
	{
		case NORTH:
			for (x=ex; x<ex+hotw; x++)
				if (ObstructAt(x, ey-1)) return true;
			break;
		case SOUTH:
			for (x=ex; x<ex+hotw; x++)
				if (ObstructAt(x, ey+hoth)) return true;
			break;
		case WEST:
			for (y=ey; y<ey+hoth; y++)
				if (ObstructAt(ex-1, y)) return true;
			break;
		case EAST:
			for (y=ey; y<ey+hoth; y++)
				if (ObstructAt(ex+hotw, y)) return true;
			break;
	}
	return false;
}

bool Entity::ObstructDir(int d)
{
	__grue_actor_index = this->index;

	int i, x, y;
	int ex = getx();
	int ey = gety();

	if (!obstructable) return false;
	switch (d)
	{
		case NORTH:
			for (i=0; i<hoth; i++)
				for (x=ex; x<ex+hotw; x++)
					if (ObstructAt(x, ey-i-1)) return true;
			break;
		case SOUTH:
			for (i=0; i<hoth; i++)
				for (x=ex; x<ex+hotw; x++)
					if (ObstructAt(x, ey+i+hoth)) return true;
			break;
		case WEST:
			for (i=0; i<hotw; i++)
				for (y=ey; y<ey+hoth; y++)
					if (ObstructAt(ex-i-1, y)) return true;
			break;
		case EAST:
			for (i=0; i<hotw; i++)
				for (y=ey; y<ey+hoth; y++)
					if (ObstructAt(ex+hotw+i, y)) return true;
			break;
	}
	return false;
}

void Entity::do_wanderzone()
{
	bool ub=false, db=false, lb=false, rb=false;
	int ex = getx()/16;
	int ey = gety()/16;
	int myzone = current_map->zone(ex, ey);

	if (ObstructDir(EAST) || current_map->zone(ex+1, ey) != myzone) rb=true;
	if (ObstructDir(WEST) || current_map->zone(ex-1, ey) != myzone) lb=true;
	if (ObstructDir(SOUTH) || current_map->zone(ex, ey+1) != myzone) db=true;
	if (ObstructDir(NORTH) || current_map->zone(ex, ey-1) != myzone) ub=true;

	if (rb && lb && db && ub) return; // Can't move in any direction

	delay = systemtime + wdelay;
	while (1)
	{
		int i = rnd(0,3);
		switch (i)
		{
			case 0:
				if (rb) break;
				set_waypoint_relative(16, 0);
				return;
			case 1:
				if (lb) break;
				set_waypoint_relative(-16, 0);
				return;
			case 2:
				if (db) break;
				set_waypoint_relative(0, 16);
				return;
			case 3:
				if (ub) break;
				set_waypoint_relative(0, -16);
				return;
		}
	}
}

void Entity::do_wanderbox()
{
	bool ub=false, db=false, lb=false, rb=false;
	int ex = getx()/16;
	int ey = gety()/16;

	if (ObstructDir(EAST) || ex+1 > wx2) rb=true;
	if (ObstructDir(WEST) || ex-1 < wx1) lb=true;
	if (ObstructDir(SOUTH) || ey+1 > wy2) db=true;
	if (ObstructDir(NORTH) || ey-1 < wy1) ub=true;

	if (rb && lb && db && ub) return; // Can't move in any direction

	delay = systemtime + wdelay;
	while (1)
	{
		int i = rnd(0,3);
		switch (i)
		{
			case 0:
				if (rb) break;
				set_waypoint_relative(16, 0);
				return;
			case 1:
				if (lb) break;
				set_waypoint_relative(-16, 0);
				return;
			case 2:
				if (db) break;
				set_waypoint_relative(0, 16);
				return;
			case 3:
				if (ub) break;
				set_waypoint_relative(0, -16);
				return;
		}
	}
}

void Entity::do_movescript()
{
	static char vc2me[] = { 2, 1, 3, 4 };
	int arg;

	// movements factors
	// These are set to -1,0 or 1 to signify in
	// which directions movement should occur
	int vertfac = 0, horizfac = 0;


    // reset to tile-based at the start of a movestring
    if(moveofs == 0) {
        movemult = 16;
    }

	while ((movestr[moveofs] >= '0' && movestr[moveofs] <= '9') || movestr[moveofs] == ' ' || movestr[moveofs] == '-')
		moveofs++;

	int done = 0;
	int found_move = 0; // number of LRUD letters we found
	while(!done && found_move < 2) {
		switch(toupper(movestr[moveofs]))
		{
			case 'L':
				if(!found_move && face != WEST) setface(WEST);
				moveofs++;
				horizfac = -1;
				found_move++;
				break;
			case 'R':
				if(!found_move && face != EAST) setface(EAST);
				moveofs++;
				horizfac = 1;
				found_move++;
				break;
			case 'U':
				if(!found_move && face != NORTH) setface(NORTH);
				moveofs++;
				vertfac = -1;
				found_move++;
				break;
			case 'D':
				if(!found_move && face != SOUTH) setface(SOUTH);
				moveofs++;
				vertfac = 1;
				found_move++;
				break;
			default:
				done = 1;
		}
	}

	if(found_move) {
		arg = atoi(&movestr[moveofs]);
		// we've already set facing, don't do it again
		set_waypoint_relative(horizfac*arg*movemult, vertfac*arg*movemult, false);
	} else {
		// no directions, check other possible letters:
		switch(toupper(movestr[moveofs])) {
			case 'S': moveofs++;
				setspeed(atoi(&movestr[moveofs]));
				break;
			case 'W': moveofs++;
				delay = systemtime + atoi(&movestr[moveofs]);
				break;
			case 'F': moveofs++;
				setface(vc2me[atoi(&movestr[moveofs])]);
				break;
			case 'B': moveofs = 0; break;
			case 'X': moveofs++;
				arg = atoi(&movestr[moveofs]);
				set_waypoint(arg*16, gety());
				break;
			case 'Y': moveofs++;
				arg = atoi(&movestr[moveofs]);
				set_waypoint(getx(), arg*16);
				break;
			case 'Z': moveofs++;
				specframe = atoi(&movestr[moveofs]);
				break;
			case 'P': movemult = 1;
				moveofs++;
				break;
			case 'T': movemult = 16;
				moveofs++;
				break;
			case 'H':
			case 0:  
				movemult = 0; moveofs = 0; movecode = 0; framect = 0; 
				return;
			default: err("Entity::do_movescript(), unidentify movescript command");
		}
	}
}

void Entity::set_chr(const std::string& fname)
{
    chr = RequestCHR(fname.c_str());
	specframe = 0;
	framect = 0;
	frame = 0;
}

void Entity::draw()
{
	if (!visible) return;

    // if we're idle, reset the framect
	if ((!follow && ready()) || (follow && leaderidle()))
		framect = 0;

	if (specframe)
		frame = specframe;
	else
	{
		if (!follow)
		{
			if (ready()) frame = chr->idle[face];
			else frame = chr->GetFrame(face, framect);
		}
		else
		{
			if (leaderidle()) frame = chr->idle[face];
			else frame = chr->GetFrame(face, framect);
		}
	}

	int zx = getx() - xwin,
		zy = gety() - ywin;

	if (hookrender.length())
	{
		event_entity = index;
		se->ExecuteFunctionString(hookrender.c_str());
		return;
	}

	if (chr)
		chr->render(zx, zy, frame, screen);
	else
		DrawRect(zx, zy, zx + 15, zy + 15, MakeColor(255,255,255), screen);
}

void Entity::SetWanderZone()
{
    clear_stalk();
	set_waypoint(getx(), gety());
	movecode = 1;
}

void Entity::SetWanderBox(int x1, int y1, int x2, int y2)
{
    clear_stalk();
	set_waypoint(getx(), gety());
	wx1 = x1;
	wy1 = y1;
	wx2 = x2;
	wy2 = y2;
	movecode = 2;
}

void Entity::SetMoveScript(const char *s)
{
    clear_stalk();
	set_waypoint(getx(), gety());
	strcpy(movestr, s);
	moveofs = 0;
	movecode = 3;
}

void Entity::SetWanderDelay(int n)
{
	wdelay = n;
}

void Entity::SetMotionless()
{
    clear_stalk();
	set_waypoint(getx(), gety());
	movecode = 0;
	delay = 0;
}
