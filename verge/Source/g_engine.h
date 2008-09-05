/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


extern Entity *entity[256];
extern Entity *myself;
extern int entities, player;
extern int xwin, ywin;
extern int cameratracking, cameratracker, entitiespaused, lastentitythink;
extern bool done;
extern int playerstep, playerdiagonals;
extern MAP *current_map;

void Engine_Start(char *map);
void ProcessControls();
void ProcessEntities();
void TimedProcessEntities();
void TimedProcessSprites();
void Render();
void RenderEntities();
int AllocateEntity(int x, int y, const char *chr);
bool ObstructAt(int x, int y);

// returns if it managed to clean up at least one spot
bool CleanupCHRs();
CHR *RequestCHR(const char *fname);

void onEntityCollision();