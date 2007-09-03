function testkey()
	log("wtf");
	mapalert("you hit w key!");
end

function testloop()
	local imMouseCursor = LoadImage( "mouse-cursor.pcx" );

	while(true) do
		rectfill(0,0,320,240,makecolor(0,100,200),screen);
		printstring(0,0,screen,0,tostring(joy[0].button[2]));
		tblit(
			mouse.x,mouse.y,
			imMouseCursor,
			screen
		);
		showpage();
	end
	
end

local sfx = loadsound("slidewhistledown.wav");

function whizz()
	log(str(playsound(sfx,100)));
end

function spam()
	StopMusic();
	StopSong(musics[1]);StopSong(musics[2]);StopSong(musics[3]);StopSong(musics[4]);StopSong(musics[5]);
	if(lastpressed == SCAN_1) then PlaySong(musics[1]); end
	if(lastpressed == SCAN_2) then PlaySong(musics[2]); end
	if(lastpressed == SCAN_3) then PlaySong(musics[3]); end
	if(lastpressed == SCAN_4) then PlaySong(musics[4]); end
	if(lastpressed == SCAN_5) then PlaySong(musics[5]); end
end

function autoexec()
	musics = {}
--	musics[1] = LoadSong("cd-level1.it");
--	musics[2] = LoadSong("cd-level2.it");
--	musics[3] = LoadSong("cd-level3.it");
--	musics[4] = LoadSong("cd-level4.it");
--	musics[5] = LoadSong("cd-level5.it");
	
	--playmusic("cd-credits.it");
	HookKey(SCAN_W,"testkey");
	HookKey(SCAN_E,"whizz");
	HookKey(SCAN_1,"spam");
	HookKey(SCAN_2,"spam");
	HookKey(SCAN_3,"spam");
	HookKey(SCAN_4,"spam");
	HookKey(SCAN_5,"spam");
	--entities[3].tx = 9;
	map("bumville.map");
	
	--testloop();
end
