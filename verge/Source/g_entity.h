/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#define NORTH				1
#define SOUTH				2
#define WEST				3
#define EAST				4
#define NW					5
#define NE					6
#define SW					7
#define SE					8

#define ENT_AUTOFACE		1
#define ENT_OBSTRUCTS		2
#define ENT_OBSTRUCTED		4

#define ENT_MOTIONLESS		0
#define ENT_MOVESCRIPT		1
#define ENT_WANDERZONE		2
#define ENT_WANDERBOX		3

class Entity
{
public:
	Entity(int x, int y, const char *chrfn);
	~Entity();
	int getx();
	int gety();
	void setxy(int x1, int y1);
	int getspeed();
	void setspeed(int s);
	int get_waypointx();
	int get_waypointy();
	void set_waypoint(int x1, int y1);
	void set_waypoint_relative(int x1, int y1, bool changeface = true);
	void setface(int d);
	bool ready();
	bool leaderidle();
	void move_tick();
	void think();
	void draw();
	void stalk(Entity *e);
	void clear_stalk(); // stop following people, if we are
    int get_leader_framect();
    void set_framect_follow(int f); // set our and our follower's framect
	void set_chr(CStringRef fname);

	void SetWanderZone();
	void SetWanderBox(int x1, int y1, int x2, int y2);
	void SetMoveScript(const char *script);
	void SetMotionless();
	void SetWanderDelay(int n);

	bool ObstructDir(int dir);
	bool ObstructDirTick(int d);
	void do_wanderzone();
	void do_wanderbox();
	void do_movescript();

	bool visible, active, autoface;
	bool obstruction, obstructable;
	CHR *chr;
	int lucent;
	int x, y;
	int waypointx, waypointy;
	int hotw, hoth;
	int speed, speedct, movecode, delay, wdelay;
	int face, framect, specframe, frame;
	int wx1, wy1, wx2, wy2;
    int movemult;
	int index;
	StringRef description; // Overkill - 2006-05-21
	StringRef hookrender;
	StringRef script;
	char movestr[256];
	int moveofs;
	Entity *follow, *follower;
	int pathx[128], pathy[128], pathf[128];
};
