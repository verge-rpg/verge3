// 2005-12-11, Overkill:
//	I'm a bad person. I stole Ragecage's job. ;__;
//	* Added some falling and movement code similar to the player's.
//	* Things roll when they fall.
//	* Removed fps.vh, because FPS counting is already possible.
// 2005-12-11, Overkill (#2):
//	* Merging of snowballs.
// 2005-12-11, Overkill (#3):
//	* Changed snowball_img to img_snowball and
//		added entry to resource database.
// 2005-12-12, Overkill:
//	* Added ability to stand atop snowballs.
// 2005-12-13, Overkill:
//	* Added growth of snowballs.
//	* Snowballs now move the player if they're standing on top.
// 2005-12-13, Ustor:
//	* Accelerated growth for rolling up inclines.
// 2005-12-13, Overkill:
//	* Fixed some issues with falling off as the snowball moves.
//	* Cleaned things up a bit, nearly done.
// 2005-12-13, Ustor:
//	* Proper growth acceleration for inclines, independent for up/down.
//	* Re-fixed player dropping through ball while riding up an incline issue.

#define SNOWBALL_MAX_SIZE		20000			
#define SNOWBALL_GROWTH_MULTIPLIER	40
#define SNOWBALL_GROWTH_DIVISOR 	2
#define UPHILL_GROWTH_MULTIPLIER	800
#define DOWNHILL_GROWTH_MULTIPLIER	600

#define SNOWBALL_DECAY			1

#define ROLL_STATE_UPHILL		-1
#define ROLL_STATE_EVEN			0
#define ROLL_STATE_DOWNHILL		1

#define MAXIMUM_SNOWBALL_SPEED		150
#define SNOWBALL_ACCELERATION		1
#define SNOWBALL_FRICTION		3

#define SNOWBALL_NOT_MELTING		0
#define SNOWBALL_MELT_ENVIRONMENT	1
#define SNOWBALL_MELT_FORCED		2

struct snowball_type
{
	int x;
	int y;
	int r;
	int x100, y100, r100;
	int hspeed, vspeed;
	int fall_flag;
	int melt_flag;
	int roll_state;
	int ang;
	int belongs_player;
};
snowball_type snowball[MAX_SNOWBALLS];
int player_snowballs;
int snowballs;
// Overkill: now loaded by resource.dat manager.
// Changed to img_snowball so the resourceLoad.vh could parse properly.
int img_snowball;

void ProcessSnowballs()
{
	int i;
	for(i = snowballs - 1; i >= 0; i--)
	{
		ProcessSnowball(i);
	}
}

void BlitSnowballs()
{
	int i;
	for(i = snowballs - 1; i >= 0; i--)
	{
		BlitSnowball(i);
	}
}

int GetPlayerSnowball(int player_ball)
{
	int i;
	int j;
	for (i = 0; i < snowballs; i++)
	{
		if (snowball[i].belongs_player)
		{
			if (j == player_ball)
			{
				return i;
			}
			j++;
		}
	}
	return 0;
}

//---Start core snowball code---//

void InsertSnowball(int x, int y)
{
	if(snowballs==MAX_SNOWBALLS) return;
	
	//if (GetObsPixel(x,y))
	//{
	//	return;
	//}
	snowball[snowballs].x=x;
	snowball[snowballs].y=y;
	snowball[snowballs].x100=x*100;
	snowball[snowballs].y100=y*100;
	snowball[snowballs].hspeed=0;
	snowball[snowballs].vspeed=0;
	snowball[snowballs].r=10;
	snowball[snowballs].r100=snowball[snowballs].r * 100;
	snowball[snowballs].ang=0;
	snowball[snowballs].fall_flag=0;
	snowball[snowballs].melt_flag=SNOWBALL_NOT_MELTING;
	snowballs++;
}

void DeleteSnowball(int ball){
	snowballs--;
	if (snowball[ball].belongs_player)
	{
		player_snowballs--;
	}
	snowball[ball].x=snowball[snowballs].x;
	snowball[ball].y=snowball[snowballs].y;
	snowball[ball].x100=snowball[snowballs].x100;
	snowball[ball].y100=snowball[snowballs].y100;
	snowball[ball].hspeed=snowball[snowballs].hspeed;
	snowball[ball].vspeed=snowball[snowballs].vspeed;	
	snowball[ball].r=snowball[snowballs].r;
	snowball[ball].r100=snowball[snowballs].r100;
	snowball[ball].ang=snowball[snowballs].ang;
	snowball[ball].fall_flag=snowball[snowballs].fall_flag;
	snowball[ball].melt_flag=snowball[snowballs].melt_flag;
	snowball[ball].belongs_player=snowball[snowballs].belongs_player;
}

void ShrinkSnowBalls()
{
	int i;
	for (i = 0; i < snowballs; i++)
	{
		if (snowball[i].belongs_player)
		{
			//snowball[i].r100 -= 100;
			//snowball[i].r100 = max(snowball[i].r100, 0);
			snowball[i].melt_flag = SNOWBALL_MELT_FORCED;
		}
	}
}

void ClearSnowballs()
{
	int i;
	for (i = 0; i < snowballs; i++)
	{
		snowball[i].x = 0;
		snowball[i].y = 0;
		snowball[i].x100 = 0;
		snowball[i].y100 = 0;
		snowball[i].hspeed = 0;
		snowball[i].vspeed = 0;	
		snowball[i].r = 0;
		snowball[i].r100 = 0;
		snowball[i].ang = 0;
		snowball[i].fall_flag = 0;
		snowball[i].melt_flag = 0;
		snowball[i].belongs_player = 0;
	}
	player_snowballs = 0;
	snowballs = 0;
}

void FileReadSnowballs(int f)
{
	int i;
	player_snowballs = 0;
	snowballs = FileReadByte(f);
	for (i = 0; i < snowballs; i++)
	{
		snowball[i].x = FileReadQuad(f);
		snowball[i].y = FileReadQuad(f);
		snowball[i].x100 = FileReadQuad(f);
		snowball[i].y100 = FileReadQuad(f);
		snowball[i].hspeed = FileReadQuad(f);
		snowball[i].vspeed = FileReadQuad(f);
		snowball[i].r = FileReadQuad(f);
		snowball[i].r100 = FileReadQuad(f);
		snowball[i].ang = FileReadQuad(f);
		snowball[i].fall_flag = FileReadByte(f);
		snowball[i].melt_flag = FileReadByte(f);
		snowball[i].belongs_player = FileReadByte(f);
		if (snowball[i].belongs_player)
		{
			player_snowballs++;
		}
	}
}

void FileWriteSnowballs(int f)
{
	int i;
	FileWriteByte(f, snowballs);
	for (i = 0; i < snowballs; i++)
	{
		 FileWriteQuad(f, snowball[i].x);
		 FileWriteQuad(f, snowball[i].y);
		 FileWriteQuad(f, snowball[i].x100);
		 FileWriteQuad(f, snowball[i].y100);
		 FileWriteQuad(f, snowball[i].hspeed);
		 FileWriteQuad(f, snowball[i].vspeed);
		 FileWriteQuad(f, snowball[i].r);
		 FileWriteQuad(f, snowball[i].r100);
		 FileWriteQuad(f, snowball[i].ang);
		 FileWriteByte(f, snowball[i].fall_flag);
		 FileWriteByte(f, snowball[i].melt_flag);
		 FileWriteByte(f, snowball[i].belongs_player);
	}
}

void CombineSnowballs(int a, int b)
{
	int i;
	int new_radius;
	// Make the radius increase at reasonable rates according to area.
//	new_radius = sqrt((snowball[a].r*snowball[a].r) + (snowball[b].r*snowball[b].r));
	new_radius = cbrt((snowball[a].r*snowball[a].r*snowball[a].r)
				+ (snowball[b].r*snowball[b].r*snowball[b].r));
	
	if (player.snowball == a
		|| player.snowball == b)
	{
		player.snowball = a;
		player.y100 -= new_radius - snowball[a].r * 100;
	}
	for (i = 0; i < enemy_count; i++)
	{
		if (enemy[i].snowball == a
			|| enemy[i].snowball == b)
		{
			enemy[i].snowball = a;
			enemy[i].y100 -= new_radius - snowball[a].r * 100;
		}
	}
	
	snowball[a].r = new_radius;
	snowball[a].r100 = snowball[a].r * 100;
	
	// Floor clipping prevention.
	while (GetObsPixel (snowball[a].x100 / 100, snowball[a].y100 + (snowball[a].r * 100) / 100)
		&& snowball[a].y100 > 0)
	{
		if (player.snowball == a)
		{
			player.y100 -= 100;
		}
		for (i = 0; i < enemy_count; i++)
		{
			if (enemy[i].snowball == a)
			{
				enemy[i].y100 -= 100;
			}
		}
		snowball[a].y100 -= 100;
	}
	// Average the speed.
	snowball[a].hspeed = snowball[a].hspeed + snowball[b].hspeed / 2;
	// Delete the other snowball.
	DeleteSnowball(b);
}

int GetSnowGrowth(int x, int y)
{
	switch(GetTile(x, y, 2))
	{
		case 1: return 10;
		case 2: return 5;
		case 3: return 1;
	}
	return 0;
}

int GetSnowDecay(int x, int y)
{
	switch(GetTile(x, y, 2))
	{
		case 4: return 1;
		case 5: return 5;
		case 6: return 10;
		case 7: return 20;
	}
	return 0;
}

void ProcessSnowball(int ball){
	int i;
	int x = snowball[ball].x;
	int y = snowball[ball].y;
	int r = snowball[ball].r;
	int grow = GetSnowGrowth(x / 16, y / 16);
	int decay = GetSnowDecay(x / 16, y / 16);
	
	for (i = 0; i < snowballs; i++)
	{
		// If the two snow balls are overlapping, combine them
		if (pow(snowball[ball].x - snowball[i].x, 2)
			+ pow(snowball[ball].y - snowball[i].y, 2)
			< pow(snowball[ball].r, 2)
			&& i != ball)
		{
			CombineSnowballs(ball, i);
			return;
		}
	}
	

	if(snowball[ball].r <= 0)
	{
		DeleteSnowball(ball);
		return;
	}
	
	// Detect if there's no floor.
	if (!GetObsPixel(snowball[ball].x100 / 100 + r,
		snowball[ball].y100 + snowball[ball].vspeed) / 100 + r) 
	{
		// Fall.
		snowball[ball].vspeed += GRAVITY_ACCELERATION;
		snowball[ball].fall_flag = 1;
	}
	else
	{
		// Stop the falling.
		snowball[ball].vspeed = 0;
		snowball[ball].fall_flag = 0;
	}
	// Cap the fall speed.
	snowball[ball].vspeed = min(snowball[ball].vspeed, MAXIMUM_FALL_SPEED);
	
	// Makes it roll to the left if there's a right obs slope.
	if ((GetObsPixel(snowball[ball].x100 / 100 + 1, snowball[ball].y100 / 100 + r - 1)
		|| GetObsPixel(snowball[ball].x100 / 100 + 2, snowball[ball].y100 / 100 + r - 1)
		) && snowball[ball].fall_flag)
	{
		snowball[ball].hspeed -= SNOWBALL_ACCELERATION;
		
		if(snowball[ball].hspeed > 0) snowball[ball].roll_state = ROLL_STATE_UPHILL;			
		else if(snowball[ball].hspeed < 0) snowball[ball].roll_state = ROLL_STATE_DOWNHILL;
	}
	// Makes it roll to the right if there's a left obs slope.
	else if ((GetObsPixel(snowball[ball].x100 / 100 - 1, snowball[ball].y100 / 100 + r - 1)
		|| GetObsPixel(snowball[ball].x100 / 100 - 2, snowball[ball].y100 / 100 + r - 1)
		) && snowball[ball].fall_flag)
	{
		snowball[ball].hspeed += SNOWBALL_ACCELERATION;
		
		if(snowball[ball].hspeed > 0) snowball[ball].roll_state = ROLL_STATE_DOWNHILL;			
		else if(snowball[ball].hspeed < 0) snowball[ball].roll_state = ROLL_STATE_UPHILL;
	}
	// If there's nothing to roll away from, friction.
	else
	{
		// Friction is in 100ths.
		snowball[ball].hspeed = snowball[ball].hspeed * (100 - SNOWBALL_FRICTION) / 100;
		snowball[ball].roll_state = ROLL_STATE_EVEN;
	}


	// Push the snowball
	if (!player.jump_flag && !b3)
		{
			if (snowball[ball].r100 + snowball[ball].x100 >= player.x100
			&& -snowball[ball].r100 + snowball[ball].x100 <= entity.hotw[player.ent] * 100 + player.x100
			&& snowball[ball].r100 + snowball[ball].y100 >= player.y100
			&& snowball[ball].r100 / 2 + snowball[ball].y100 <= entity.hoth[player.ent] * 100 + player.y100)
			{
				player.pushball = ball;
				if (player.move_flag)
				{
					player.frame = "PUSH";
				}
				snowball[ball].hspeed = player.hspeed * 2;
				snowball[ball].ang += snowball[ball].hspeed / 100;
				// Stop when you hit a wall.
				if (GetObsPixel(snowball[ball].x100 + snowball[ball].hspeed / 100 + r, y) && snowball[ball].hspeed > 0)
				{
					snowball[ball].vspeed = -abs(snowball[ball].hspeed);
					snowball[ball].roll_state = ROLL_STATE_UPHILL;
				}
				else if (GetObsPixel(snowball[ball].x100 + snowball[ball].hspeed / 100 - r, y) && snowball[ball].hspeed < 0)
				{
					snowball[ball].vspeed = -abs(snowball[ball].hspeed);
					snowball[ball].roll_state = ROLL_STATE_UPHILL;
				}
				
				
			}
		}

	
	// Cap the rolling speed.
			//snowball[ball].hspeed = max (min(snowball[ball].hspeed, MAXIMUM_SNOWBALL_SPEED), -MAXIMUM_SNOWBALL_SPEED);
	if (abs(snowball[ball].hspeed) > MAXIMUM_SNOWBALL_SPEED)
	{
		// Normal friction
		snowball[ball].hspeed = snowball[ball].hspeed * (100 - SNOWBALL_FRICTION) / 100;
	}
	
	if (snowball[ball].melt_flag != SNOWBALL_MELT_FORCED)
	{
		snowball[ball].melt_flag = SNOWBALL_NOT_MELTING;
		// Snowball decay
		if (decay)
		{
			snowball[ball].r100 -= SNOWBALL_DECAY * decay;

			snowball[ball].melt_flag = SNOWBALL_MELT_ENVIRONMENT;		
		}
	}
	else
	{
		snowball[ball].melt_flag = SNOWBALL_MELT_ENVIRONMENT;
		snowball[ball].r100 -= 10;
		if (snowball[ball].r100 < 0)
		{
			snowball[ball].r100 = 0;
		}
	}
	
	// Snowball growth
	if (snowball[ball].hspeed && snowball[ball].melt_flag != SNOWBALL_MELT_FORCED)
	{	
		if (snowball[ball].roll_state == ROLL_STATE_UPHILL)
		{
			snowball[ball].r100 += abs(snowball[ball].hspeed) * grow * UPHILL_GROWTH_MULTIPLIER / (snowball[ball].r100 * (snowball[ball].r100 / 500) * SNOWBALL_GROWTH_DIVISOR);
		}
		else if (snowball[ball].roll_state == ROLL_STATE_DOWNHILL)
		{
			snowball[ball].r100 += abs(snowball[ball].hspeed) * grow * DOWNHILL_GROWTH_MULTIPLIER / (snowball[ball].r100 * (snowball[ball].r100 / 500) * SNOWBALL_GROWTH_DIVISOR);
		}
		else
		{
			snowball[ball].r100 += abs(snowball[ball].hspeed) * grow * SNOWBALL_GROWTH_MULTIPLIER / (snowball[ball].r100 * (snowball[ball].r100 / 500) * SNOWBALL_GROWTH_DIVISOR);
		}
			
		snowball[ball].r100 = min(snowball[ball].r100, SNOWBALL_MAX_SIZE);
		
	}
	snowball[ball].r = snowball[ball].r100 / 100;
	
	// Move unless it hits a wall.
	if (!GetObsPixel(snowball[ball].x100 + snowball[ball].hspeed / 100 + r, y) && snowball[ball].hspeed > 0)
	{
		snowball[ball].x100 += snowball[ball].hspeed;
		for (i = 0; i < enemy_count; i++)
		{
			if (enemy[i].snowball == ball)
			{
				enemy[i].x100 += snowball[ball].hspeed;
				if (snowball[ball].roll_state == ROLL_STATE_UPHILL)
				{
					if(enemy[i].y100 > snowball[ball].y100 - snowball[ball].r100 - (entity.hoth[enemy[i].ent]+1*100)) 
						enemy[i].y100 = snowball[ball].y100 - snowball[ball].r100 - (entity.hoth[enemy[i].ent]+1*100);
				}
			}
			if (!enemy[i].hurt_flag && entity.visible[enemy[i].ent])
			{
				if (snowball[ball].r100 * 3 / 4 + snowball[ball].x100 >= enemy[i].x100
					&& -snowball[ball].r100 * 3 / 4 + snowball[ball].x100 <= entity.hotw[enemy[i].ent] * 100 + enemy[i].x100
					&& snowball[ball].r100 + snowball[ball].y100 >= enemy[i].y100
					&& snowball[ball].r100 / 2 + snowball[ball].y100 <= entity.hoth[enemy[i].ent] * 100 + enemy[i].y100
					)
				{
					SnowballDamageEnemy(ball, i, 1);
					PlaySound(sfx_run_over, 100);
				}
			}
		}
		if (player.snowball == ball)
		{
			player.x100 += snowball[ball].hspeed;
		}
	}
	else if (!GetObsPixel(snowball[ball].x100 + snowball[ball].hspeed / 100 - r, y) && snowball[ball].hspeed < 0)
	{
		snowball[ball].x100 += snowball[ball].hspeed;
		for (i = 0; i < enemy_count; i++)
		{
			if (enemy[i].snowball == ball)
			{
				enemy[i].x100 += snowball[ball].hspeed;
				if (snowball[ball].roll_state == ROLL_STATE_UPHILL)
				{
					if(enemy[i].y100 > snowball[ball].y100 - snowball[ball].r100 - (entity.hoth[enemy[i].ent]+1*100)) 
						enemy[i].y100 = snowball[ball].y100 - snowball[ball].r100 - (entity.hoth[enemy[i].ent]+1*100);
				}
			}
			if (!enemy[i].hurt_flag && entity.visible[enemy[i].ent])
			{
				if (snowball[ball].r100 * 3 / 4 + snowball[ball].x100 >= enemy[i].x100
					&& -snowball[ball].r100 * 3 / 4 + snowball[ball].x100 <= entity.hotw[enemy[i].ent] * 100 + enemy[i].x100
					&& snowball[ball].r100 + snowball[ball].y100 >= enemy[i].y100
					&& snowball[ball].r100 / 2 + snowball[ball].y100 <= entity.hoth[enemy[i].ent] * 100 + enemy[i].y100
					)
				{
					SnowballDamageEnemy(ball, i, -1);
					PlaySound(sfx_run_over, 100);
				}
			}
		}
		if (player.snowball == ball)
		{
			player.x100 += snowball[ball].hspeed;
		}
	}

	// As long as it doesn't risk hitting a ceiling, it can move vertically.
	if (!GetObsPixel (snowball[ball].x100 / 100,   snowball[ball].y100 - snowball[ball].r100 + snowball[ball].vspeed / 100)
		|| snowball[ball].vspeed > 0)
	{
		if (player.snowball == ball)
		{
			player.y100 += snowball[ball].vspeed;
		}
		snowball[ball].y100 += snowball[ball].vspeed;	
	}
	else if (snowball[ball].vspeed < 0)
	{
		snowball[ball].vspeed = 0;
	}
	
	// Floor clipping prevention.
	while (GetObsPixel (snowball[ball].x100 / 100, snowball[ball].y100 + (r * 100) / 100)
		&& snowball[ball].y100 > 0)
	{
		snowball[ball].y100 -= 100;
		if (player.snowball == ball)
		{
			player.y100 -= 100;
		}
		snowball[ball].fall_flag = 0;
	}
	if (snowball[ball].y100 <= 0)
	{
		DeleteSnowball(ball);
	}
	// Roll when the ball moves horizontally.
	snowball[ball].ang -= snowball[ball].hspeed / 10;
	
	// Round the (x, y) for stuff.
	snowball[ball].x = snowball[ball].x100 / 100;
	snowball[ball].y = snowball[ball].y100 / 100;
}

void SnowballDamageEnemy(int ball, int ent, int dir)
{
	enemy[ent].hspeed = 400 * sgn(dir);
	enemy[ent].hurt_flag = 80;
	enemy[ent].damage += abs(snowball[ball].hspeed) * snowball[ball].r100 / 500;
	if (enemy[ent].damage > 250)
	{
		enemy[ent].hurt_flag = enemy[ent].hurt_flag / 2;
	}
}

void BlitSnowball(int ball)
{
	int x = snowball[ball].x;
	int y = snowball[ball].y;
	int r = snowball[ball].r;
	int scale_factor = ImageWidth(img_snowball) * 1000 / 2 / r;
	
	if (x - xwin + r > 0 
		&& x -xwin - r < ImageWidth(screen)
		&& y -ywin + r > 0
		&& y - ywin - r < ImageWidth(screen))
	{

		RotScale(x - xwin, y - ywin, snowball[ball].ang, scale_factor, img_snowball, screen);
	}
}

/*

rotscale
r = imageWidth *1000 /2 /scale
scale = (imageWidth *1000 /2) /r


*/